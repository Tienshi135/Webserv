#pragma once

#include "header.hpp"

typedef enum e_method
{
    GET,
    POST,
    DELETE
}   t_method;

class Request {

    private:
        std::string     _version;
        t_method       _method;
		std::string		_path;

	public:
		Request(std::string received);
        Request(const Request &copy);
        Request &operator=(const Request &copy);
        ~Request();

        std::string     getVersion() const;
        t_method       getRequest() const;
        std::string     getPath() const;

        void            setVersion(const std::string &version);
        void            setRequest(t_method request);
        void            setPath(const std::string &path);

        void            printRequest() const;
        std::string     requestTypeToString() const;
};
