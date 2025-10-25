#include "Request.hpp"

/*============================= Constructors and destructor =====================================*/

Request::Request(std::string received)
{
	std::stringstream			iss(received);
	std::string					line;
	std::vector<std::string>	firstLine;
	std::vector<std::string>	headerLine;


	//REQUEST LINE
	std::getline(iss, line);
	firstLine = tokenizeLine(line);
	this->_valid = this->fillFirstLine(firstLine);//fills method, uri, and version, returns false if not HTTP/1.1 complying
	if (!this->_valid)
		return;

	//HEADERS
	std::getline(iss, line);
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
		headerLine = tokenizeLine(line);
	}

	//BODY (if it exists)
	std::getline(iss, this->_body, '\0');

	//request validation
	this->_valid = this->validateRequest();
	this->printRequest();
}

Request::Request(const Request &copy)
: _method(copy._method),
_uri(copy._uri),
_version(copy._version),
_headers(copy._headers),
_body(copy._body),
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

std::string Request::getVersion() const
{
	return (this->_version);
}

std::string Request::getMethod() const
{
	return (this->_method);
}

std::string Request::getUri() const
{
	return (this->_uri);
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
<<<<<<< HEAD
=======

bool Request::isValid() const
{
    return this->_valid;
}

/*============================= Member functions =====================================*/


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
		std::cout << this->_body << std::endl;
	}
	std::cout << MAGENTA << "========== end of request ===============\n" << MAGENTA << std::endl;
}

bool	Request::fillFirstLine(std::vector<std::string>& firstLine)
{
	if (firstLine.size() != 3)
	{
		LOG_INFO_LINK("Invalid request: first line size has not 3 elements");
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
		LOG_INFO_LINK("Invalid request: version does not match \"HTTP/\"");
		return false;
	}
	this->_version = it->substr(5);
	if (this->_version != "1.1" && this->_version != "1.0")
	{
		LOG_INFO_LINK("Invalid request: unsuported HTTP/ version");
		return false;
	}
	return true;
}


bool	Request::validateRequest(void)
{
	if (!this->_body.empty())
	{
		if (this->_headers.find("Content-Length") == this->_headers.end())
		{
			LOG_INFO_LINK("Invalid request has body but not header [Content-lenght], sending error page");
			return false;
		}
		size_t size = static_cast<size_t>(std::atol(this->_headers["Content-Length"].c_str()));
		if (this->_body.size() != size)
		{
			LOG_INFO_LINK("Invalid request: header [Content-lenght] does not match body size");
			return false;
		}
	}
	if (this->_headers.find("Host") == this->_headers.end() || this->_headers["Host"].empty())
	{
		LOG_INFO_LINK("Invalid request: header [Host] not found");
		return false;
	}
	return true;
}

>>>>>>> main
