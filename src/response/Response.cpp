#include "Response_headers/Response.hpp"
#include "Request.hpp"

/*========================= Constructor and destructor ================================*/


Response::Response(const ServerCfg &config, const Request &request)
: _cfg(config),
_req(request),
_version("HTTP/1.0"),
_statusCode(OK),
_statusMsg("OK"),
_autoindex(_cfg.getAutoindex()),
_bodyIsFile(true)
{}

Response::~Response(){}

/*========================= getters and setters  ================================*/

/*========================= Protected member funcions  ================================*/

void Response::_addStandardHeaders(void)
{
	time_t now = time(NULL);
	struct tm* gmt = gmtime(&now);
	char dateBuffer[128];
	strftime(dateBuffer, sizeof(dateBuffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	this->_addHeader("Date", std::string(dateBuffer));
	this->_addHeader("Server", this->_cfg.getName());
	this->_addHeader("Connection", "close");
}

std::string	Response::_getReasonPhrase(e_errorcode errCode) const
{
	static std::map<e_errorcode, std::string> errorCodes;

	if (errorCodes.empty())
	{
		// 1xx Informational
		errorCodes[CONTINUE] = "Continue";
		errorCodes[SWITCHING_PROTOCOLS] = "Switching Protocols";

		// 2xx Success
		errorCodes[OK] = "OK";
		errorCodes[CREATED] = "Created";
		errorCodes[ACCEPTED] = "Accepted";
		errorCodes[NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
		errorCodes[NO_CONTENT] = "No Content";
		errorCodes[RESET_CONTENT] = "Reset Content";
		errorCodes[PARTIAL_CONTENT] = "Partial Content";

		// 3xx Redirection
		errorCodes[MULTIPLE_CHOICES] = "Multiple Choices";
		errorCodes[MOVED_PERMANENTLY] = "Moved Permanently";
		errorCodes[FOUND] = "Found";
		errorCodes[SEE_OTHER] = "See Other";
		errorCodes[NOT_MODIFIED] = "Not Modified";
		errorCodes[USE_PROXY] = "Use Proxy";
		errorCodes[TEMPORARY_REDIRECT] = "Temporary Redirect";

		// 4xx Client Errors
		errorCodes[BAD_REQUEST] = "Bad Request";
		errorCodes[UNAUTHORIZED] = "Unauthorized";
		errorCodes[PAYMENT_REQUIRED] = "Payment Required";
		errorCodes[FORBIDDEN] = "Forbidden";
		errorCodes[NOT_FOUND] = "Not Found";
		errorCodes[METHOD_NOT_ALLOWED] = "Method Not Allowed";
		errorCodes[NOT_ACCEPTABLE] = "Not Acceptable";
		errorCodes[PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
		errorCodes[REQUEST_TIMEOUT] = "Request Timeout";
		errorCodes[CONFLICT] = "Conflict";
		errorCodes[GONE] = "Gone";
		errorCodes[LENGTH_REQUIRED] = "Length Required";
		errorCodes[PRECONDITION_FAILED] = "Precondition Failed";
		errorCodes[PAYLOAD_TOO_LARGE] = "Payload Too Large";
		errorCodes[URI_TOO_LONG] = "URI Too Long";
		errorCodes[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
		errorCodes[RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
		errorCodes[EXPECTATION_FAILED] = "Expectation Failed";

		// 5xx Server Errors
		errorCodes[INTERNAL_SERVER_ERROR] = "Internal Server Error";
		errorCodes[NOT_IMPLEMENTED] = "Not Implemented";
		errorCodes[BAD_GATEWAY] = "Bad Gateway";
		errorCodes[SERVICE_UNAVAILABLE] = "Service Unavailable";
		errorCodes[GATEWAY_TIMEOUT] = "Gateway Timeout";
		errorCodes[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	}

	std::map<e_errorcode, std::string>::const_iterator it = errorCodes.find(errCode);
	if (it != errorCodes.end())
	return it->second;
	return "Unknown Status";
}

void	Response::_responseIsErrorPage(e_errorcode errCode)
{
	static std::map<e_errorcode, std::string> errorPages;

	if (this->_sendCustomErrorPage(errCode))
		return;

	if (errorPages.empty())
	{
		// 4xx Client Errors
		errorPages[BAD_REQUEST] = "<!DOCTYPE html><html><body><h1>400 Bad Request</h1><p>The request could not be understood by the server.</p></body></html>";
		errorPages[UNAUTHORIZED] = "<!DOCTYPE html><html><body><h1>401 Unauthorized</h1><p>Authentication is required to access this resource.</p></body></html>";
		errorPages[FORBIDDEN] = "<!DOCTYPE html><html><body><h1>403 Forbidden</h1><p>Access to this resource is forbidden.</p></body></html>";
		errorPages[NOT_FOUND] = "<!DOCTYPE html><html><body><h1>404 Not Found</h1><p>The requested resource could not be found.</p></body></html>";
		errorPages[METHOD_NOT_ALLOWED] = "<!DOCTYPE html><html><body><h1>405 Method Not Allowed</h1><p>The method is not allowed for this resource.</p></body></html>";
		errorPages[REQUEST_TIMEOUT] = "<!DOCTYPE html><html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
		errorPages[CONFLICT] = "<!DOCTYPE html><html><body><h1>409 Conflict</h1><p>The request conflicts with the current state of the server.</p></body></html>";
		errorPages[GONE] = "<!DOCTYPE html><html><body><h1>410 Gone</h1><p>The requested resource is no longer available.</p></body></html>";
		errorPages[LENGTH_REQUIRED] = "<!DOCTYPE html><html><body><h1>411 Length Required</h1><p>Content-Length header is required.</p></body></html>";
		errorPages[PAYLOAD_TOO_LARGE] = "<!DOCTYPE html><html><body><h1>413 Payload Too Large</h1><p>The request entity is too large.</p></body></html>";
		errorPages[URI_TOO_LONG] = "<!DOCTYPE html><html><body><h1>414 URI Too Long</h1><p>The request URI is too long.</p></body></html>";
		errorPages[UNSUPPORTED_MEDIA_TYPE] = "<!DOCTYPE html><html><body><h1>415 Unsupported Media Type</h1><p>The media type is not supported.</p></body></html>";

		// 5xx Server Errors
		errorPages[INTERNAL_SERVER_ERROR] = "<!DOCTYPE html><html><body><h1>500 Internal Server Error</h1><p>The server encountered an unexpected condition.</p></body></html>";
		errorPages[NOT_IMPLEMENTED] = "<!DOCTYPE html><html><body><h1>501 Not Implemented</h1><p>The server does not support the functionality required.</p></body></html>";
		errorPages[BAD_GATEWAY] = "<!DOCTYPE html><html><body><h1>502 Bad Gateway</h1><p>The server received an invalid response from an upstream server.</p></body></html>";
		errorPages[SERVICE_UNAVAILABLE] = "<!DOCTYPE html><html><body><h1>503 Service Unavailable</h1><p>The server is temporarily unable to handle the request.</p></body></html>";
		errorPages[GATEWAY_TIMEOUT] = "<!DOCTYPE html><html><body><h1>504 Gateway Timeout</h1><p>The server did not receive a timely response from an upstream server.</p></body></html>";
		errorPages[HTTP_VERSION_NOT_SUPPORTED] = "<!DOCTYPE html><html><body><h1>505 HTTP Version Not Supported</h1><p>The HTTP version is not supported by the server.</p></body></html>";
	}

	this->_setStatus(errCode);
	this->_body = errorPages[errCode];
	this->_bodyIsFile = false;

	this->_addHeader("Content-Type", "text/html; charset=UTF-8");
	this->_addHeader("Content-Length", numToString(this->_body.size()));
}

bool	Response::_sendCustomErrorPage(e_errorcode errCode)
{
	std::map<int, std::string> const& customPages = this->_cfg.getErrorPages();
	std::map<int, std::string>::const_iterator it = customPages.find(errCode);

	if (it == customPages.end())
	{
		LOG_INFO_LINK("No custom error page configured for code [" + numToString(errCode) + "], sending default");
		return false;
	}
	std::string errorPage = this->_normalizePath(this->_cfg.getRoot(), it->second);

	if (!this->_isSecurePath(errorPage))
	{
		LOG_HIGH_WARNING_LINK("Custom error page failed security check: " + errorPage);
		return false;
	}
	if (access(errorPage.c_str(), F_OK) < 0)
	{
		LOG_WARNING_LINK("Custom error page not found: " + errorPage);
		return false;
	}
	if (access(errorPage.c_str(), R_OK) < 0)
	{
		LOG_WARNING_LINK("Custom error page not readable: " + errorPage);
		return false;
	}
	struct stat fileStat;
	if (stat(errorPage.c_str(), &fileStat) != 0)
	{
		LOG_WARNING_LINK("Failed to get stats for custom error page: " + errorPage);
		return false;
	}
	if (!S_ISREG(fileStat.st_mode))
	{
		LOG_WARNING_LINK("Custom error page is not a regular file: " + errorPage);
		return false;
	}

	this->_setStatus(errCode);
	this->_bodyFilePath = errorPage;
	this->_bodyIsFile = true;

	this->_addHeader("Content-Type", "text/html; charset=UTF-8");
	this->_addHeader("Content-Length", numToString(static_cast<size_t>(fileStat.st_size)));
	return true;
}

std::string Response::
_getContentType(std::string const& path) const
{
	// Find file extension
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dotPos + 1);

	// Convert to lowercase
	for (size_t i = 0; i < ext.length(); ++i)
		ext[i] = std::tolower(ext[i]);

	//Map extensions to MIME types
	//Text-based formats (add charset)
	if (ext == "html" || ext == "htm")
		return "text/html; charset=UTF-8";
	if (ext == "css")
		return "text/css; charset=UTF-8";
	if (ext == "js")
		return "application/javascript; charset=UTF-8";
	if (ext == "json")
		return "application/json; charset=UTF-8";
	if (ext == "txt")
		return "text/plain; charset=UTF-8";
	if (ext == "xml")
		return "application/xml; charset=UTF-8";

	//Binary formats (no charset)
	if (ext == "png")
		return "image/png";
	if (ext == "jpg" || ext == "jpeg")
		return "image/jpeg";
	if (ext == "gif")
		return "image/gif";
	if (ext == "svg")
		return "image/svg+xml";
	if (ext == "ico")
		return "image/x-icon";
	if (ext == "pdf")
		return "application/pdf";
	if (ext == "zip")
		return "application/zip";
	if (ext == "mp4")
		return "video/mp4";
	if (ext == "mp3")
		return "audio/mpeg";

	return "application/octet-stream";
}


void	Response::_setStatus(e_errorcode code)
{
	this->_statusCode = code;
	this->_statusMsg = this->_getReasonPhrase(code);
}

void	Response::_setBody(std::string const& bodyContent, std::string const& contentType)
{
	this->_bodyIsFile = false;
	this->_body = bodyContent;
	this->_addHeader("Content-Type", contentType);
	this->_addHeader("Content-Length", numToString(this->_body.size()));
}

void	Response::_addHeader(std::string const& key, std::string const& value)
{
	this->_headers[key] = value;
}

off_t	Response::_validateFilePath(std::string const& path)
{
	//check if the file exists and is readeable
	if (access(path.c_str(), F_OK) < 0)
	{
		this->_responseIsErrorPage(NOT_FOUND);
		LOG_INFO_LINK("File [" + path + "] not found, sending error page 404");
		return -1;
	}
	if (access(path.c_str(), R_OK) < 0)
	{
		this->_responseIsErrorPage(FORBIDDEN);
		LOG_INFO_LINK("Can't read file [" + path + "], forbidden, sending error page 403");
		return -1;
	}
	// Get file stats (size, type, etc.)
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) != 0)
	{
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		LOG_INFO_LINK("Failed to get file [" + path + "] stats, sending error page 500");
		return -1;
	}

	// Check if it's a regular file
	if (!S_ISREG(fileStat.st_mode))
	{
		this->_responseIsErrorPage(FORBIDDEN);
		LOG_INFO_LINK("Path [" + path + "] is not a regular file, sending error page 403");
		return -1;
	}
	return fileStat.st_size;
}

void	Response::_sendFileAsBody(std::string const& path)
{
	off_t contentSize;
	if (pathIsDirectory(path))
	{
		if (this->_autoindex)
		{
			std::string body = this->_generateDirListingHtml(path);
			if (body.empty())
			{
				LOG_INFO_LINK("requested directory listing: [" + path + "] but couldn't open it");
				this->_responseIsErrorPage(PAYLOAD_TOO_LARGE);
				return;
			}
			this->_setBody(body, "text/html");
			this->_setStatus(OK);
		}
		else
		{
			LOG_INFO_LINK("requested directory listing: [" + path + "] but autoindex is off, sending 403 forbidden");
			this->_responseIsErrorPage(FORBIDDEN);
		}

		return ;
	}
	else
		contentSize = this->_validateFilePath(path);

	if (contentSize < 0)
		return ;

	std::string	type = this->_getContentType(path);
	this->_addHeader("Content-Type", type);
	this->_addHeader("Content-Length", numToString(static_cast<size_t>(contentSize)));

	this->_bodyIsFile = true;
	this->_bodyFilePath = path;
	this->_setStatus(OK);
	return ;
}



bool	Response::_isSecurePath(std::string const& path)
{
	if (path.find("../") != std::string::npos)
		return false;
	if (path.find("..\\") != std::string::npos)//windows style
		return false;
	if (path.find("%2e%2e") != std::string::npos)//URL-encoded
		return false;
	if (path.find("%2E%2E") != std::string::npos)
		return false;

	return true;
}

std::string	Response::_normalizePath(std::string const& root, std::string const& uri)
{
	std::string	normalizedRoot = root;
	std::string	normalizedUri = uri;

	if (!root.empty() && root[root.size() - 1] != '/')
		normalizedRoot += "/";


	if (!uri.empty() && uri[0] == '/')
		normalizedUri = uri.substr(1);

	return normalizedRoot + normalizedUri;
}
std::string	Response::_generateDirListingHtml(std::string const& path)
{
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return ("");

	struct dirent *d;
	std::vector<std::string> entries;

	while ((d = readdir(dir)) != NULL)
	{
		std::string name = d->d_name;

		if (name == "." || name == "..")
			continue;

		entries.push_back(name);
	}

	closedir(dir);

	std::sort(entries.begin(), entries.end());

	std::ostringstream html;
	html << "<html><head><title>Index of "
		<< htmlEscape(path)
		<< "</title></head><body>\n";
	html << "<h1>Index of " << htmlEscape(path) << "</h1>\n";
	html << "<ul>\n";

	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::string name = entries[i];
		std::string fullpath = path + "/" + name;

		struct stat st;
		if (stat(fullpath.c_str(), &st) == -1)
			continue;

		bool is_dir = S_ISDIR(st.st_mode);

		std::string href = urlEncode(name);
		if (is_dir)
			href += "/";

		std::string display = htmlEscape(name);
		if (is_dir)
			display += "/";

		html << "<li><a href=\"" << href << "\">"
				<< display
				<< "</a></li>\n";
	}

	html << "</ul></body></html>\n";

	return html.str();
}

/*========================= Public member functions  ================================*/


void Response::printResponse() const
{
	std::cout << GREEN << "\n=== Response Information ===" << RESET << std::endl;
	std::cout << "HTTP Version: " << this->_version << " ";
	std::cout << "Status Code: " << this->_statusCode << " " << this->_statusMsg << std::endl;
	std::map< std::string, std::string >::const_iterator it;
	for (it = this->_headers.begin(); it != this->_headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	if (this->_bodyIsFile)
		std::cout << "Content Preview: sent from the file " << this->_bodyFilePath << std::endl;
	else
	{
		// for (size_t i = 0; i < this->_req.getBodySize(); i++)
		// 	std::cout << this->_body[i];
		std::cout << "Content Preview: " << (this->_body.length() > 50 ? this->_body.substr(0, 50) + "..." : this->_body) << std::endl;
	}
	std::cout << GREEN << "=========== End of Response==============\n"  << RESET<< std::endl;
}


std::string	Response::getRawResponse(void) const
{
	std::ostringstream response;

	//first line
	response << this->_version << " "
			<< this->_statusCode << " "
			<< this->_statusMsg << "\r\n";

	//headers
	std::map<std::string, std::string>::const_iterator it;
	for (it = this->_headers.begin(); it != this->_headers.end(); it++)
		response << it->first << ": " << it->second << "\r\n";

	//empty line
	response << "\r\n";

	//body
	if (this->_bodyIsFile)
	{
		std::ifstream file(this->_bodyFilePath.c_str(), std::ios::binary);
		if (file.is_open())
		{
			response << file.rdbuf();
			file.close();
		}
		else
		{
			response << "<!DOCTYPE html><html><body>"
					<< "<h1>500 Internal Server Error</h1>"
					<< "<p>File temporarily unavailable.</p>"
					<< "</body></html>";
			LOG_WARNING_LINK("Access to a file was unexpectedly lost after first check, sending error page 500");
		}
	}
	else
		response << this->_body;

	return response.str();
}


