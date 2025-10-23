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
		Request const&		_req;
		ServerCfg const&	_cfg;
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
		bool	sendFileAsBody(std::string const& path);


		std::string resolvePathSafely(std::string const& uri, std::string const& root);
		bool		isPathInsideRoot(std::string const& resolved, std::string const& root);
		void		responseIsErrorPage(int errCode);


	private:
		Response(const Response &copy);
		Response &operator=(const Response &copy);

	public:
		Response(ServerCfg const& config,  Request const& request);
		~Response();

		virtual void	buildResponse() = 0;
		std::string		getRawResponse() const;

		void		printResponse() const;
};
