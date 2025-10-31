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

	e_contentType						_contentType;
	std::string							_boundary;
	std::map<std::string, std::string>	_contentTypeElements;
	std::map<std::string, std::string>	_contentDisposition;
	std::string							_saveFile;
	std::string							_fileName;
	std::string							_mime;

	std::string		getFileName();
	std::string		parseNameFromMultipart();
	e_contentType	extractContentType();

	void	printContentTypeElements();
	void	makeUnicIde(std::string& fileName, std::string const& type);
	bool	setOrCreatePath(std::string const& path);

	void	buildFromMultipart();

public:
	ResponsePost(ServerCfg const& cfg, Request const& req);
	virtual ~ResponsePost();

	virtual void	buildResponse();
};

