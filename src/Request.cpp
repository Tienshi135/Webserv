#include "Request.hpp"

/*============================= Constructors and destructor =====================================*/
//TODO complete constructors with final atributes
Request::Request()
: _expectedBodySize(0),
_bodySize(0),
_bodyPos(0),
_tmpBodyFileSize(0),
_hasBody(false),
_expectedReadBytes(0),
_valid(false),
_headersReceived(false),
_requestCompleted(false)
{}

Request::Request(const Request &copy)
: _method(copy._method),
_uri(copy._uri),
_version(copy._version),
_headers(copy._headers),
_body(copy._body),
_bodySize(copy._bodySize),
_valid(copy._valid) {}


Request::~Request()
{
	if (this->_tmpBodyFile.is_open())
		this->_tmpBodyFile.close();

	if (!this->_bodyFilePath.empty())
	{
		if (std::remove(this->_bodyFilePath.c_str()) == 0)
			LOG_INFO("Deleted temp file: " + this->_bodyFilePath);
		else
			LOG_WARNING("Failed to delete temp file: " + this->_bodyFilePath);
	}
}

/*============================= assing operator =====================================*/

Request &Request::operator=(const Request &copy)
{
	if (this != &copy)
	{
		this->_method = copy._method;
		this->_uri = copy._uri;
		this->_version = copy._version;
		this->_headers = copy._headers;
		this->_body = copy._body;
		this->_valid = copy._valid;
	}
	return (*this);
}

/*============================= getters and setters =====================================*/

std::string Request::getVersion(void) const
{
	return (this->_version);
}

std::string Request::getMethod(void) const
{
	return (this->_method);
}

std::string Request::getUri(void) const
{
	return (this->_uri);
}

size_t	Request::getBodySize(void) const
{
	return (this->_bodySize);
}

ssize_t	Request::getExpectedReadBytes(void) const
{
	return (this->_expectedReadBytes);
}
// Setters
void Request::setVersion(const std::string &version)
{
	this->_version = version;
}

void Request::setMethod(std::string const& method)
{
	this->_method = method;
}

void Request::setUri(const std::string &path)
{
	this->_uri = path;
}

void	Request::setBody(std::string const& newBody)
{
	this->_body = newBody;
}

void	Request::setRequestCompleted(bool completed)
{
	this->_requestCompleted = completed;
}

std::string Request::getHeader(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
    if (it != this->_headers.end())
        return it->second;
    return "";
}

std::string Request::getBody() const
{
    return this->_body;
}

std::string	Request::getBodyFilePath() const
{
	return this->_bodyFilePath;
}

bool	Request::getRequestCompleted(void) const
{
	return this->_requestCompleted;
}

bool Request::isValid() const
{
    return this->_valid;
}

/*============================= Member functions =====================================*/

void	Request::expectedReadBytes(ssize_t bytesReceived)
{
	ssize_t missing = 0;
	double percentage = 0;
	if (bytesReceived < this->_expectedReadBytes)
	{
		percentage = (static_cast<double>(bytesReceived) / this->_expectedReadBytes * 100.0);
		missing = this->_expectedReadBytes - bytesReceived;
	}

    std::cout << ORANGE << "==========================================" << std::endl;
    std::cout << "DATA RECEPTION ANALYSIS" << std::endl;
    std::cout << "==========================================" << RESET << std::endl;
    std::cout << "Received:  " << bytesReceived << " bytes" << std::endl;
    std::cout << "Expected:  " << this->_expectedReadBytes << " bytes (request line + headers + body)" << std::endl;
    std::cout << "Missing:   " << missing << " bytes" << std::endl;
    std::cout << "Progress:  " << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;

    if (bytesReceived < this->_expectedReadBytes)
    {
        std::cout << RED << "WARNING: Only received " << percentage << "% of expected data!" << std::endl;
        std::cout  << missing << " bytes were NOT read from socket!" << RESET << std::endl;
    }
    else
    {
        std::cout << GREEN << "Complete request received!" << RESET << std::endl;
    }
    std::cout << ORANGE << "==========================================" << std::endl;
    std::cout << " END OF DATA ANALYSIS" << std::endl;
    std::cout << "==========================================" << RESET << std::endl;
}

void Request::printRequest() const
{
	std::cout << MAGENTA << "\n=== Request Information ===" << RESET << std::endl;
	std::cout << "Method: " << this->_method << std::endl;
	std::cout << "URI: " << this->_uri << std::endl;
	std::cout << "Version: HTTP/" << this->_version << std::endl;
	std::cout << "Valid: " << (this->_valid ? "Yes" : "No") << std::endl;
	if (!this->_headers.empty())
	{
		std::cout << "\nHeaders:" << std::endl;
		std::map<std::string, std::string>::const_iterator it;
		for (it = this->_headers.begin(); it != this->_headers.end(); ++it)
			std::cout << "  " << it->first << ": " << it->second << std::endl;
	}

	if (!this->_body.empty())
	{
		std::cout << "\nBody (" << this->_body.length() << " bytes):" << std::endl;
		// std::cout << this->_body << std::endl;
	}
	std::cout << MAGENTA << "========== end of request ===============\n" << RESET << std::endl;
}

bool	Request::fillFirstLine(std::vector<std::string>& firstLine)
{
	if (firstLine.size() != 3)
	{
		LOG_WARNING_LINK("Invalid request: first line size has not 3 elements");
		return false;
	}

	std::vector<std::string>::iterator it = firstLine.begin();
	this->_method = *it;
	it++;
	this->_uri = *it;
	it++;

	std::string	validVersion = it->substr(0, 5);
	if (validVersion != "HTTP/")
	{
		LOG_WARNING_LINK("Invalid request: version does not match \"HTTP/\"");
		return false;
	}
	this->_version = it->substr(5);
	if (this->_version != "1.1" && this->_version != "1.0")
	{
		LOG_WARNING_LINK("Invalid request: unsuported HTTP/ version");
		return false;
	}
	return true;
}


bool	Request::validateRequest(void)
{
	if (this->_hasBody)
	{
		if (this->_headers.find("Content-Length") == this->_headers.end())
		{
			LOG_WARNING_LINK("Invalid request has body but not header [Content-lenght], sending error page");
			return false;
		}

		size_t promisedSize = static_cast<size_t>(std::atol(this->_headers["Content-Length"].c_str()));
		ssize_t fileSize = getFileSize(this->_bodyFilePath);
		if (fileSize != static_cast<ssize_t>(promisedSize))
		{
			LOG_WARNING_LINK("Invalid request: Body size mismatch: got " + numToString(static_cast<size_t>(fileSize)) + ", expected " + numToString(promisedSize));
			return false;
		}

		this->_bodySize = fileSize;
	}
	else
	{
		if (this->_method == "POST")
		{
			LOG_WARNING_LINK("Invalid request: method POST has no body");
			return false;
		}
	}

	if (this->_headers.find("Host") == this->_headers.end() || this->_headers["Host"].empty())
	{
		LOG_WARNING_LINK("Invalid request: header [Host] not found");
		return false;
	}
	return true;
}


void	Request::buildHeaders(std::string received)
{
	std::stringstream			iss(received);
	std::string					line;
	std::vector<std::string>	firstLine;
	std::vector<std::string>	headerLine;


	//REQUEST LINE
	std::getline(iss, line);
	this->_expectedReadBytes += line.size() + 1;
	firstLine = tokenizeLine(line);
	this->_valid = this->fillFirstLine(firstLine);//fills method, uri, and version, returns false if not HTTP/1.1 complying
	if (!this->_valid)
		return;

	//HEADERS
	std::getline(iss, line);
	this->_expectedReadBytes += line.size() + 1;
	headerLine = tokenizeLine(line);
	while (!headerLine.empty())
	{
		std::string key = headerLine.front();
		if (!key.empty() && key[key.length() - 1] == ':')
			key.erase(key.length() - 1);

		std::string value;
		for (size_t i = 1; i < headerLine.size(); i++)
		{
			value += headerLine[i];
			if (!value.empty() && value[value.length() - 1] == ',')
				value.erase(value.length() - 1);
			if (i + 1 < headerLine.size())
				value += " ";
		}
		this->_headers[key] = value;

		std::getline(iss, line);
		this->_expectedReadBytes += line.size() + 1;
		headerLine = tokenizeLine(line);
	}

	//BODY (if it exists)
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		size_t headerEnd = received.find("\r\n\r\n");
		this->_bodyPos = (headerEnd != std::string::npos) ? headerEnd + 4 : received.size();
		// TODO check if content-length is greater than client_body_size_max here and send error payload to large if so
		this->_expectedBodySize = static_cast<size_t>(std::atol(this->_headers["Content-Length"].c_str()));
		this->_expectedReadBytes += this->_expectedBodySize;
		this->_hasBody = true;
	}
}
void	Request::generateBodyPath()
{
	std::stringstream ss;
	ss << "/tmp/tmp_file_" << time(NULL) << ".tmp";
	this->_bodyFilePath = ss.str();
}

//TODO should we return an error from this function?
void	Request::buildBody(char const *buffer, int bytes_read, size_t total_bytes_received)
{

	if (!this->_tmpBodyFile.is_open())
	{
		if (this->_bodyFilePath.empty())
			this->generateBodyPath();

		this->_tmpBodyFile.open(this->_bodyFilePath.c_str());
		if (!this->_tmpBodyFile.is_open())
		{
			LOG_HIGH_WARNING_LINK("Failed to create temp file");
			this->_valid = false;
			return;
		}
	}

	size_t bodyChunck;
	size_t start;
	if (this->_bodyPos > 0)
	{
		bodyChunck = total_bytes_received - (this->_bodyPos);
		start = bytes_read - bodyChunck;
		this->_bodyPos = 0;
	}
	else
	{
		start = 0;
		bodyChunck = bytes_read;
	}

	this->_tmpBodyFile.write(&buffer[start], bodyChunck);
	if (this->_tmpBodyFile.fail())
	{
		LOG_HIGH_WARNING("Failed to write to temp file: " + _bodyFilePath);
		this->_valid = false;
		this->_tmpBodyFile.close();
		std::remove(_bodyFilePath.c_str());
		return;
	}
	this->_tmpBodyFileSize += bodyChunck;
}

int	Request::parseInput(char const *buffer, int bytes_read, size_t total_bytes_received)
{

	//TODO safe check for a max request size before continuing reading. otherwise we could end reading an innecesarely ammount of data before checking with the headers
	// if (total_bytes_received > MAX_REQUEST_SIZE) -> not MAX_BODY_SIZE!! request = request line + headers + body
	// {
	// 	stop reading and send an error message
	//	return -1;
	// }


	if (this->_requestCompleted)
	{
		this->_valid = this->validateRequest();
		return 0;
	}

	if (!this->_headersReceived)
	{
		this->_received.append(buffer);
		if (this->_received.find("\r\n\r\n") != std::string::npos)
		{
			this->_headersReceived = true;
			this->buildHeaders(this->_received);
			if (this->_expectedBodySize == 0)
			{
				this->_requestCompleted = true;
				return 0;
			}
		}
	}

	if (this->_headersReceived)
	{

		this->buildBody(buffer, bytes_read, total_bytes_received);
		if (this->_tmpBodyFileSize >= this->_expectedBodySize)
		{
			this->_tmpBodyFile.flush();
			if (this->_tmpBodyFile.fail())
			{
				LOG_HIGH_WARNING("Flush failed, data may be lost");
				this->_valid = false;
				this->_tmpBodyFile.close();
				std::remove(this->_bodyFilePath.c_str());
				return -1;
			}
			this->_tmpBodyFile.close();
			this->_requestCompleted = true;
		}
	}

	return 0;
}
