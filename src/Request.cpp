#include "Request.hpp"

Request::Request(std::string received)//might need to make it throw tbf
{
	std::size_t pos;
	std::size_t end_l;
	std::string line;

	end_l = received.find_first_of('\n');
	if (end_l == std::string::npos)
		end_l = received.length();
	line = received.substr(0, end_l);
	
	if (line.find("GET") == 0)
		this->_request = GET;
	else if (line.find("POST") == 0)
		this->_request = POST;
	else if (line.find("DELETE") == 0)
		this->_request = DELETE;
	else
	{
		std::cout << "parse error: unknown method" << std::endl;//to change
	}
	
	pos = line.find(' ');
	if (pos != std::string::npos)
	{
		pos++;
		std::size_t path_end = line.find(" HTTP/", pos);
		if (path_end != std::string::npos)
			this->_path = line.substr(pos, path_end - pos);
		else
			this->_path = "/";
	}
	else
		this->_path = "/";//might need to change
	
	pos = line.find("HTTP/");
	if (pos != std::string::npos)
	{
		std::string version_str = line.substr(pos + 5);
		if (!version_str.empty())
		{
			size_t end = version_str.find_first_of(" \t\r\n");
			if (end != std::string::npos)
				version_str = version_str.substr(0, end);
			this->_version = version_str;
		}
		else
		{
			std::cout << "parse error: empty version" << std::endl;// to change
			this->_version = "1.0";
		}
	}
	else
	{
		std::cout << "parse error: no HTTP version found" << std::endl;// to change to opposite logic with if statement
		this->_version = "1.0";
	}
}

Request::Request(const Request &copy) : _version(copy._version), _request(copy._request), _path(copy._path)
{
}

Request &Request::operator=(const Request &copy)
{
	if (this != &copy)
	{
		this->_version = copy._version;
		this->_request = copy._request;
		this->_path = copy._path;
	}
	return (*this);
}

Request::~Request()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Request::getVersion() const		// Changed return type to string
{
	return (this->_version);
}

t_request Request::getRequest() const
{
	return (this->_request);
}

std::string Request::getPath() const
{
	return (this->_path);
}

// Setters
void Request::setVersion(const std::string &version)	// Changed parameter type
{
	this->_version = version;
}

void Request::setRequest(t_request request)
{
	this->_request = request;
}

void Request::setPath(const std::string &path)
{
	this->_path = path;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Request::requestTypeToString() const
{
	switch (this->_request)
	{
		case GET:
			return ("GET");
		case POST:
			return ("POST");
		case DELETE:
			return ("DELETE");
		default:
			return ("UNKNOWN");
	}
}

void Request::printRequest() const
{
	std::cout << "=== Request Information ===" << std::endl;
	std::cout << "Request Type: " << requestTypeToString() << std::endl;
	std::cout << "HTTP Version: " << this->_version << std::endl;
	std::cout << "URL Path: " << this->_path << std::endl;
	std::cout << "=========================" << std::endl;
}