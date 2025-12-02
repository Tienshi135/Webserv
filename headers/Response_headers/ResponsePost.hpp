#pragma once

#include "Response.hpp"

class ResponsePost : public Response
{
	private:

		enum e_contentType
		{
			TEXT,
			MULTIPART,
			JSON,
			URLENCODED,
			UNKNOWNCT,
		};

	//responsePost atributes
		e_contentType						_contentType;
		std::string							_boundary;
		std::map<std::string, std::string>	_contentTypeElements;
		std::map<std::string, std::string>	_contentDisposition;
		std::string							_saveFile;
		std::string							_fileName;
		std::string							_mime;

	//responsePost tools
		e_contentType	extractContentType();
		std::string		normalizeFilename(std::string const& fileName);
		std::string		saveFilePath();
		void			printContentTypeElements();
		void			makeUnicIde(std::string& fileName, std::string const& type);
		bool			setOrCreatePath(std::string const& path);

	//Build from type functions
		int	buildFromMultipart();

	public:
		ResponsePost(ServerCfg const& cfg, Request const& req);
		virtual ~ResponsePost();

		virtual void	buildResponse();
};

