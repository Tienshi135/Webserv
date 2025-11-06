#include "Request.hpp"

/*============================= Constructors and destructor =====================================*/

Request::Request(std::string received) : _bodySize(0), _expectedReadBytes(0) , _valid(false)
{
	std::stringstream			iss(received);
	std::string					line;
	std::vector<std::string>	firstLine;
	std::vector<std::string>	headerLine;


	//REQUEST LINE
	std::getline(iss, line);
	this->_expectedReadBytes += line.size();//+2
	firstLine = tokenizeLine(line);
	this->_valid = this->fillFirstLine(firstLine);//fills method, uri, and version, returns false if not HTTP/1.1 complying
	if (!this->_valid)
		return;

	//HEADERS
	std::getline(iss, line);
	this->_expectedReadBytes += line.size();//+2
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
		this->_expectedReadBytes += line.size();//+2
		headerLine = tokenizeLine(line);
	}

	// this->_expectedReadBytes += 4;
	//BODY (if it exists)
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		// TODO check if content-length is greater than client_body_size_max here and send error payload to large if so
		size_t contentLength = static_cast<size_t>(std::atol(this->_headers["Content-Length"].c_str()));
		this->_expectedReadBytes += contentLength;
		if (contentLength > 0)
		{
			this->_body.resize(contentLength);
			iss.read(&this->_body[0], contentLength);
		}
	}

	//request validation
	this->_valid = this->validateRequest();
}

Request::Request(const Request &copy)
: _method(copy._method),
_uri(copy._uri),
_version(copy._version),
_headers(copy._headers),
_body(copy._body),
_bodySize(copy._bodySize),
_valid(copy._valid) {}


Request::~Request() {}

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
	size_t size;

	if (!this->_body.empty())
	{
		if (this->_headers.find("Content-Length") == this->_headers.end())
		{
			LOG_WARNING_LINK("Invalid request has body but not header [Content-lenght], sending error page");
			return false;
		}
		size = static_cast<size_t>(std::atol(this->_headers["Content-Length"].c_str()));
		if (this->_body.size() != size)
		{
			LOG_WARNING_LINK("Invalid request: Body size mismatch: got " + numToString(this->_body.size()) + ", expected " + numToString(size));
			return false;
		}
		this->_bodySize = size;
	}
	if (this->_headers.find("Host") == this->_headers.end() || this->_headers["Host"].empty())
	{
		LOG_WARNING_LINK("Invalid request: header [Host] not found");
		return false;
	}
	return true;
}

