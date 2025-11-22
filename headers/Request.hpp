#pragma once

#include "header.hpp"

class Request {

    private:
    //request atributes
        std::string                         _bufferString;
        std::string                         _method;
		std::string                         _uri;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;

    //body management
        std::ofstream   _tmpBodyFileStream;
        std::string     _tmpBodyFilePath;
        size_t          _tmpBodyFileSize;
        size_t          _expectedBodySize;
        size_t          _bodyPos;
        ssize_t         _expectedReadBytes;

    //completion checks
        bool    _headersReceived;
        bool    _requestCompleted;
        bool    _valid;
        bool    _tooBig;

    //private member functions
        void    generateBodyPath();
        void    buildHeaders(std::string received);
        void    buildBody(char const *buffer, int bytes_read, size_t total_bytes_received);
        bool    fillFirstLine(std::vector<std::string>& firstLine);

    //Request should be passed as reference and not as a copy.
        Request(const Request &copy);
        Request &operator=(const Request &copy);
	public:
		Request();
        ~Request();

    //getters simple
        std::string     getVersion() const { return (this->_version); };
        std::string     getMethod() const { return (this->_method); };
        std::string     getUri() const { return (this->_uri); };
        std::string     getBodyFilePath() const { return this->_tmpBodyFilePath; };
        size_t          getTmpBodySize() const { return this->_tmpBodyFileSize; };
        size_t          getExpectedBodySize() const { return this->_expectedBodySize; };
        ssize_t         getExpectedReadBytes() const { return (this->_expectedReadBytes); };
        bool            getRequestCompleted() const { return this->_requestCompleted; };

    //getters complex
        std::string     getHeader(const std::string &key) const;

    //setters
        void    setVersion(const std::string &version) { this->_version = version; };
        void    setMethod(std::string const& method) { this->_method = method; };
        void    setUri(const std::string &path) { this->_uri = path; };
        void    setRequestCompleted(bool completed) { this->_requestCompleted = completed; };
        void    setTooBig(bool tooBig) { this->_tooBig = tooBig; };

    //member functions
        int     parseInput(char const *buffer, int bytes_read, size_t total_bytes_received);
        bool    isValid() const;
        bool    isTooBig() const;
        bool    validateRequest();

    //Data analisis tools
        void    printRequest() const;
        void    printRecepAnalisis(ssize_t bytesReceived) const;
};
