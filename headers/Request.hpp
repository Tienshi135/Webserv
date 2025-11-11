#pragma once

#include "header.hpp"

class Request {

    private:
        std::string                         _received;
        std::string                         _method;
		std::string                         _uri;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _expectedBodySize;
        size_t                              _bodySize;
        size_t                              _bodyPos;
        std::ofstream                       _tmpBodyFile;
        std::string                         _bodyFilePath;
        bool                                _hasBody;
        ssize_t                             _expectedReadBytes;
        bool                                _valid;
        bool                                _headersReceived;
        bool                                _requestCompleted;

	public:
		Request();
        Request(const Request &copy);
        Request &operator=(const Request &copy);
        ~Request();

        std::string     getVersion() const;
        std::string     getMethod() const;
        std::string     getUri() const;
        std::string     getHeader(const std::string &key) const;
        std::string     getBody() const;
        size_t          getBodySize() const;
        ssize_t         getExpectedReadBytes() const;
        bool            getRequestCompleted() const;

        void            setVersion(const std::string &version);
        void            setMethod(std::string const& method);
        void            setUri(const std::string &path);
        void            setBody(std::string const& newBody);
        void            setRequestCompleted(bool completed);

        void            printRequest() const;

        void            generateBodyPath();
        void            buildHeaders(std::string received);
        void            buildBody(std::vector<char> const& readBuffer);
        int             parseInput(std::vector<char> const& readBuffer);
        bool	        fillFirstLine(std::vector<std::string>& firstLine);
        bool            validateRequest();
        bool            isValid() const;

        void          expectedReadBytes(ssize_t bytesReceived);

};
