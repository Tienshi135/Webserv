#pragma once

#include "header.hpp"

/*Non instantiable class*/
class Response
{
	protected:
		ServerCfg const&	_cfg;
		Request const&		_req;

	//response line
		std::string	const	_version;
		int					_statusCode;
		std::string			_statusMsg;

	//headers and body
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		bool			_bodyIsFile;
		std::string		_bodyFilePath;

	//private functions
		std::string		_getReasonPhrase(int errCode) const;
		std::string		_getContentType(std::string const& path) const;

	//setting atributes
		void	_setStatus(int code);
		void	_setBody(std::string const& bodyContent, std::string const& contentType);
		void	_addHeader(std::string const& key, std::string const& value);
		void	_sendFileAsBody(std::string const& path);
		bool	_sendCustomErrorPage(int errCode);

	//tools
		bool		_isSecurePath(std::string const& path);
		std::string	_normalizePath(std::string const& root, std::string const& uri);
		void		_responseIsErrorPage(int errCode);
		off_t		_validateFilePath(std::string const& path);

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
