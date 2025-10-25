#pragma once

// #include <iostream>
// #include <fstream>
// #include <string>
// #include <sstream>
// #include <stdexcept>
// #include <unistd.h>
// #include <sys/stat.h>
// #include <fstream>
// #include <cctype>
#include "header.hpp"

class Response
{
	protected:
		ServerCfg const&	_cfg;
		Request const&		_req;
		std::string	const	_version;

		int				_statusCode;
		std::string		_statusMsg;
		std::map<std::string, std::string>	_headers;
		std::string		_body;

		bool			_bodyIsFile;
		std::string		_bodyFilePath;

		std::string		getReasonPhrase(int errCode) const;
		std::string		getContentType(std::string const& path) const;

		void	setStatus(int code);
		void	setBody(std::string const& bodyContent, std::string const& contentType);
		void	addHeader(std::string const& key, std::string const& value);
		void	sendFileAsBody(std::string const& path);
		bool	sendCustomErrorPage(int errCode);

		bool		isSecurePath(std::string const& path);
		std::string	normalizePath(std::string const& root, std::string const& uri);
		// std::string resolvePathSafely(std::string const& uri, std::string const& root);
		// bool		isPathInsideRoot(std::string const& resolved, std::string const& root);
		void		responseIsErrorPage(int errCode);
		off_t		validateFilePath(std::string const& path);

	private:
		Response(const Response &copy);
		Response &operator=(const Response &copy);

	public:
		Response(ServerCfg const& config,  Request const& request);
		virtual ~Response();

		virtual void	buildResponse() = 0;
		std::string		getRawResponse() const;

		void		printResponse() const;
};
