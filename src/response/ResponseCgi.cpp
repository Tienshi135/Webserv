#include "Response_headers/ResponseCgi.hpp"
#include "Request.hpp"
#include "Configuration.hpp"
#include <sys/wait.h>

/*============== constructor and destructor =============*/

ResponseCgi::ResponseCgi(ServerCfg const& cfg, Request const& req)
: Response(cfg, req)
{
	std::string uri = req.getUri();
	std::string scriptName = uri.substr(0, uri.find('?'));
	size_t dotPos = scriptName.find_last_of('.');

	const Location *location = cfg.getBestMatchLocation(req.getUri());

	if (location == NULL) {
		LOG_HIGH_WARNING_LINK("cgi constructor failed to match location");
		return;
	}

	size_t lastSlash = scriptName.find_last_of('/');
	if (lastSlash != std::string::npos)
		this->_scriptName = scriptName.substr(lastSlash);
	else
		this->_scriptName = scriptName;

	this->_scriptPath = location->getRoot();
	this->_ext = scriptName.substr(dotPos);
	this->_execType = this->_getExecType();
	this->_executor = this->_getExecutor();

	this->generateEnvironment(this->_enviroment);
}

ResponseCgi::~ResponseCgi() {}


/*============== Member functions =============*/


ResponseCgi::e_exectype ResponseCgi::_getExecType(void)
{
	std::string ext = this->_ext;

	if (ext == ".sh")
		return (BASH);
	if (ext == ".py")
		return (PYTHON);
	if (ext == ".php")
		return (PHP);

	return (UNKNOWNEXECT);
}

void	ResponseCgi::buildResponse(void)
{
	switch (this->_execType)
	{
	case BASH:
		this->_buildFromBash();
		break;
	case PYTHON:
		this->_buildFromPython();
		break;
	case PHP:
		this->_buildFromPhp();
		break;
	default:
		this->_responseIsErrorPage(NOT_IMPLEMENTED);
		break;
	}
}

/*============== private Member functions =============*/

char**	ResponseCgi::_mapToTable(std::map<std::string, std::string> const& envMap)
{
	size_t envSize = envMap.size();

	char** envTable = new char*[envSize + 1];

	std::map<std::string, std::string>::const_iterator it;
	size_t i = 0;

	for (it = envMap.begin(); it != envMap.end(); ++it, ++i)
	{
		std::string envEntry = it->first + "=" + it->second;

		envTable[i] = new char[envEntry.length() + 1];
		std::strcpy(envTable[i], envEntry.c_str());
	}

	envTable[envSize] = NULL;

	return envTable;

}

std::string	ResponseCgi::_extractQueryString(void)
{
	std::string uri = this->_req.getUri();

	size_t questionMarkPos = uri.find_first_of('?');
	if (questionMarkPos == std::string::npos)
	{
		LOG_ERROR_LINK("query string not found");
		return ("");
	}

	std::string queryString = urlDecode(uri.substr(questionMarkPos + 1));

	return (queryString);
}

void ResponseCgi::generateEnvironment(std::map<std::string, std::string>& env)
{
	env["CONTENT_TYPE"] = this->_req.getHeader("Content-Type");
	env["CONTENT_LENGTH"] = this->_req.getHeader("Content-Length");
	env["HTTP_COOKIE"] = this->_req.getHeader("Cookie");
	env["HTTP_USER_AGENT"] = this->_req.getHeader("User-Agent");
	env["PATH_INFO"] = this->_scriptPath;
	env["QUERY_STRING"] = this->_extractQueryString();
	env["REMOTE_ADDR"] = "";//TODO extract from socket
	env["REMOTE_HOST"] = "";//TODO extract from socket
	env["REQUEST_METHOD"] = this->_req.getMethod();
	env["SCRIPT_FILENAME"] = this->_normalizePath(this->_scriptPath, this->_scriptName);
	env["SCRIPT_NAME"] = this->_scriptName;
	env["SERVER_NAME"] = this->_cfg.getName();
	env["SERVER_SOFTWARE"] = "Webserver 42";
}

std::string	ResponseCgi::_getExecutor(void)
{
	Location const *location = this->_cfg.getBestMatchLocation(this->_req.getUri());
	if (!location)
	{
		LOG_HIGH_WARNING_LINK("can't find matching location");
		return ("");
	}

	return location->getExtensionExecutor(this->_ext);
}

void	ResponseCgi::_parseAndSend(std::string const& output)
{
	bool usesCRLF = true;

	size_t headerEnd = output.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		headerEnd = output.find("\n\n");
		usesCRLF = false;
		if (headerEnd == std::string::npos)
		{
			// No headers found - treat entire output as body
			LOG_WARNING_LINK("CGI output has no headers, treating as plain body");
			this->_setBody(output, "text/html");
			this->_setStatus(OK);
			return;
		}
	}

	std::string headers = output.substr(0, headerEnd);
	std::string body = output.substr(headerEnd + (usesCRLF ? 4 : 2));

	// Default headers
	std::string contentType = "text/html";
	e_errorcode statusCode = OK;

	std::istringstream headerStream(headers);
	std::string line;

	while (std::getline(headerStream, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		if (line.empty())
			continue;
 		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
		{
			LOG_WARNING_LINK("Invalid CGI header line: " + line);
			continue;
		}

		std::string headerName = line.substr(0, colonPos);
		std::string headerValue = line.substr(colonPos + 1);

		size_t firstNonSpace = headerValue.find_first_not_of(" \t");
		if (firstNonSpace != std::string::npos)
			headerValue = headerValue.substr(firstNonSpace);

		if (headerName == "Content-Type" || headerName == "Content-type")
		{
			contentType = headerValue;
		}
		else if (headerName == "Status")
		{
			std::istringstream statusStream(headerValue);
			int statusInt = 0;
			statusStream >> statusInt;

			if (statusInt < 100 || statusInt > 599)
			{
				LOG_WARNING_LINK("Invalid status code from CGI: " + headerValue);
				statusCode = OK;
			}
			else
			{
				statusCode = static_cast<e_errorcode>(statusInt);
			}
		}
		else if (headerName == "Location")
		{
			this->_addHeader("Location", headerValue);

			if (statusCode == OK)
				statusCode = FOUND;
		}
		else
		{
			// Pass through other headers
			this->_addHeader(headerName, headerValue);
		}
	}

	// Build response
	this->_addStandardHeaders();
	this->_setStatus(statusCode);
	// this->_setBody(body, contentType);
	this->_setBody(body, contentType);
}

/*================================ Builders =====================================================*/

void	ResponseCgi::_buildFromBash(void)
{
	this->_responseIsErrorPage(NOT_IMPLEMENTED);//TODO implement
}

void	ResponseCgi::_buildFromPython(void)
{
	if (this->_executor.empty())
	{
		LOG_HIGH_WARNING_LINK("No executor found for Python scripts");
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}

	std::string fullScriptPath = _normalizePath(this->_scriptPath, this->_scriptName);
	if (!pathIsRegFile(fullScriptPath))
	{
		LOG_HIGH_WARNING_LINK("Can't execute or find CGI script: " + fullScriptPath);
		this->_responseIsErrorPage(NOT_FOUND);
		return;
	}

	int pipeOut[2];
	if (pipe(pipeOut) == -1)
	{
		LOG_HIGH_WARNING_LINK("Failed to create output pipe for CGI");
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}

	int pipeIn[2];
	bool isPost = (this->_req.getMethod() == "POST");
	if (pipe(pipeIn) == -1)
	{
		close(pipeOut[0]);
		close(pipeOut[1]);
		LOG_HIGH_WARNING_LINK("Failed to create input pipe for CGI");
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}


	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipeOut[0]);
		close(pipeOut[1]);
		if (isPost)
		{
			close(pipeIn[0]);
			close(pipeIn[1]);
		}
		LOG_HIGH_WARNING_LINK("Fork failed for CGI execution");
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}

	if (pid == 0)
	{
		close(pipeOut[0]);
		dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeOut[1]);

		if (isPost)
		{
			close(pipeIn[1]);
			dup2(pipeIn[0], STDIN_FILENO);
			close(pipeIn[0]);
		}

		char* executor = const_cast<char*>(this->_executor.c_str());
		char* scriptPath =  const_cast<char*>(fullScriptPath.c_str());
		char* args[] = { executor, scriptPath, NULL };
		char** env = this->_mapToTable(this->_enviroment);

		execve(executor, args, env);
		perror("execve failed");
		exit(1);
	}
	close(pipeOut[1]);

	if (isPost)
	{
		close(pipeIn[0]);  // Close read end of input pipe

		std::string bodyFilePath = this->_req.getBodyFilePath();
		if (!bodyFilePath.empty())
		{
			std::ifstream bodyFile(bodyFilePath.c_str(), std::ios::binary);
			if (bodyFile.is_open())
			{
				char buffer[4096];
				while (bodyFile.read(buffer, sizeof(buffer)) || bodyFile.gcount() > 0)
				{
					ssize_t written = write(pipeIn[1], buffer, bodyFile.gcount());
					if (written == -1)
					{
						LOG_WARNING_LINK("Failed to write POST body to CGI");
						break;
					}
				}
				bodyFile.close();
			}
			else
			{
				LOG_WARNING_LINK("Failed to open body file: " + bodyFilePath);
			}
		}
		else
			LOG_INFO_LINK("CGI request with POST method has no body");

		close(pipeIn[1]);
	}

	std::string cgiOutput;
	char buffer[4096];
	ssize_t bytesRead;

	while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, bytesRead);

	close(pipeOut[0]);

	int status;
	waitpid(pid, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
		LOG_HIGH_WARNING_LINK("CGI script failed with status: " +
								numToString(WEXITSTATUS(status)));
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}

	this->_parseAndSend(cgiOutput);
}

void	ResponseCgi::_buildFromPhp(void)
{
	this->_responseIsErrorPage(NOT_IMPLEMENTED);//TODO implement
}
