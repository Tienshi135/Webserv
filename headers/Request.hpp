#pragma once

#include "header.hpp"

class Request {

    private:
        std::string     _method;
		std::string		_uri;
        std::string     _version;
        std::map<std::string, std::string> _headers;
        std::string     _body;
        size_t          _bodySize;
        ssize_t          _expectedReadBytes;
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
        size_t          getBodySize() const;
        bool            isValid() const;

        void            setVersion(const std::string &version);
        void            setMethod(std::string const& method);
        void            setUri(const std::string &path);
        void            setBody(std::string const& newBody);

        void            printRequest() const;

        bool	        fillFirstLine(std::vector<std::string>& firstLine);
        bool            validateRequest();

        void          expectedReadBytes(ssize_t bytesReceived);

};
