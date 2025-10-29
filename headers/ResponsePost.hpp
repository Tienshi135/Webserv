#pragma once

#include "Response.hpp"

class ResponsePost : public Response
{
private:

	std::string							_contentType;
	std::map<std::string, std::string>	_contentTypeElements;
	std::map<std::string, std::string>	_contentDisposition;

	std::string	getFileName();
	std::string	parseNameFromMultipart();

	void	printContentTypeElements();
	void	makeUnicIde(std::string& fileName, std::string const& type);
	bool	setOrCreatePath(std::string const& path);

public:
	ResponsePost(ServerCfg const& cfg, Request const& req);
	virtual ~ResponsePost();

	virtual void	buildResponse();
};

