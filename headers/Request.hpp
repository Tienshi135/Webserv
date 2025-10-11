#pragma once

#include "header.hpp"

typedef enum e_request
{
    GET,
    POST,
    DELETE
}   t_request;

class Request {

    private:
        std::string     _version;
        t_request       _request;
		std::string		_path;
		
	public:
		Request(std::string received);
        Request(const Request &copy);
        Request &operator=(const Request &copy);
        ~Request();
        
        std::string     getVersion() const;
        t_request       getRequest() const;
        std::string     getPath() const;
        
        void            setVersion(const std::string &version);
        void            setRequest(t_request request);
        void            setPath(const std::string &path);
        
        void            printRequest() const;
        std::string     requestTypeToString() const;
};