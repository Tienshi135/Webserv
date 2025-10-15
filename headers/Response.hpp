#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

class Request;
class Server;

class Response
{
	private:
		std::string     _version;
		unsigned int    _code;
		std::string     _code_str;
		std::string     _content_type;
		std::string     _content;
		unsigned int    _content_length;
		std::string		_connection_status;
	
	public:
		Response(const Server &config, const Request &request);
		Response(const Response &copy);
		Response &operator=(const Response &copy);
		~Response();
		
		std::string		getVersion() const;
		unsigned int	getCode() const;
		std::string		getCodeStr() const;
		std::string		getContentType() const;
		std::string		getContent() const;
		unsigned int	getContentLength() const;
		std::string		getConnectionStatus() const;
		
		void			setVersion(const std::string &version);
		void			setCode(unsigned int code);
		void			setCodeStr(const std::string &code_str);
		void			setContentType(const std::string &content_type);
		void			setContent(const std::string &content);
		void			setContentLength(unsigned int content_length);
		void			setConnectionStatus(const std::string &connection_status);
		
		void			printResponse() const;
		std::string		buildResponse() const;
};