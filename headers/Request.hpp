#pragma once

#include "header.hpp"

class Request {

    private:
        std::string     _method;
		std::string		_uri;
        std::string     _version;
        std::map<std::string, std::string> _headers;//TODO: could/need this to be a map<std::string, std::vector> to store multiple values for header?
        std::string     _body;
        bool            _valid;

	public:
		Request(std::string received);
        Request(const Request &copy);
        Request &operator=(const Request &copy);
        ~Request();

        std::string     getVersion() const;
        std::string     getMethod() const;
        std::string     getUri() const;
        std::string     getHeader(const std::string &key) const;
        std::string     getBody() const;
        bool            isValid() const;

        void            setVersion(const std::string &version);
        void            setMethod(std::string const& method);
        void            setUri(const std::string &path);

        void            printRequest() const;

        bool	        fillFirstLine(std::vector<std::string>& firstLine);
        bool            validateRequest();

};
