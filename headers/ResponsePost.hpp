#pragma once

#include "Response.hpp"

class ResponsePost : public Response
{
private:
	std::string	getFileName();
	void	makeUnicIde(std::string& fileName, std::string const& type);
	void	trimFileName(std::string& filename);
	bool	setOrCreatePath(std::string const& path);

public:
	ResponsePost(ServerCfg const& cfg, Request const& req);
	virtual ~ResponsePost();

	virtual void	buildResponse();
};

