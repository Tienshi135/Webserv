#include "Response.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

Response::Response(const Server &config, const Request &request) : _version("1.0")//need update
{
    std::string path;

    path += "." + config.getRoot();
    if (request.getPath().compare("/") == 0)
        path += "/" + config.getIndex();
    else
        path += request.getPath();
    std::cout << path << std::endl;
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		this->_code = 404;
		this->_code_str = "Can't fine page";
		this->_content_type = "text/html";
		this->_content = "<html><body><h1>404 Couldn't find page</h1></body></html>";
		this->_content_length = this->_content.length();
		this->_connection_status = "close";
		return;
	}
	std::string html_content;
	std::string line;
	while (std::getline(file, line))
	{
		html_content += line + "\n";
	}
	file.close();
	
	this->_code = 200;
	this->_code_str = "OK";
	this->_content_type = "text/html";//to change
	this->_content = html_content;
	this->_content_length = html_content.length();
	this->_connection_status = "close";
}

Response::Response(const Response &copy) : _version(copy._version), _code(copy._code), _code_str(copy._code_str), _content_type(copy._content_type), _content(copy._content), _content_length(copy._content_length), _connection_status(copy._connection_status)
{
}

Response &Response::operator=(const Response &copy)
{
	if (this != &copy)
	{
		this->_version = copy._version;
		this->_code = copy._code;
		this->_code_str = copy._code_str;
		this->_content_type = copy._content_type;
		this->_content = copy._content;
		this->_content_length = copy._content_length;
		this->_connection_status = copy._connection_status;
	}
	return (*this);
}

Response::~Response()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Response::getVersion() const
{
	return (this->_version);
}

unsigned int Response::getCode() const
{
	return (this->_code);
}

std::string Response::getCodeStr() const
{
	return (this->_code_str);
}

std::string Response::getContentType() const
{
	return (this->_content_type);
}

std::string Response::getContent() const
{
	return (this->_content);
}

unsigned int Response::getContentLength() const
{
	return (this->_content_length);
}

std::string Response::getConnectionStatus() const
{
	return (this->_connection_status);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Response::setVersion(const std::string &version)
{
	this->_version = version;
}

void Response::setCode(unsigned int code)
{
	this->_code = code;
}

void Response::setCodeStr(const std::string &code_str)
{
	this->_code_str = code_str;
}

void Response::setContentType(const std::string &content_type)
{
	this->_content_type = content_type;
}

void Response::setContent(const std::string &content)
{
	this->_content = content;
	this->_content_length = content.length();//might want to change it
}

void Response::setContentLength(unsigned int content_length)
{
	this->_content_length = content_length;
}

void Response::setConnectionStatus(const std::string &connection_status)
{
	this->_connection_status = connection_status;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Response::printResponse() const
{
	std::cout << "=== Response Information ===" << std::endl;
	std::cout << "HTTP Version: " << this->_version << std::endl;
	std::cout << "Status Code: " << this->_code << " " << this->_code_str << std::endl;
	std::cout << "Content-Type: " << this->_content_type << std::endl;
	std::cout << "Content-Length: " << this->_content_length << std::endl;
	std::cout << "Connection: " << this->_connection_status << std::endl;
	std::cout << "Content Preview: " << (this->_content.length() > 50 ? this->_content.substr(0, 50) + "..." : this->_content) << std::endl;
	std::cout << "=========================" << std::endl;
}

std::string Response::buildResponse() const
{
	std::string response;
	
	response = "HTTP/" + this->_version + " ";
	if (this->_code < 100)
		response += "0";
	if (this->_code < 10)
		response += "0";
	
	std::ostringstream oss;
	oss << this->_code;
	response += oss.str() + " " + this->_code_str + "\r\n";
	
	response += "Content-Type: " + this->_content_type + "\r\n";
	
	std::ostringstream len_oss;
	len_oss << this->_content_length;
	response += "Content-Length: " + len_oss.str() + "\r\n";
	
	response += "Connection: " + this->_connection_status + "\r\n";
	
	response += "\r\n";
	
	response += this->_content;
	
	return (response);
}
