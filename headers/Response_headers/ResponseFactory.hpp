#pragma once

#include "header.hpp"

class Response;

class ResponseFactory
{
private:
	ResponseFactory();
	~ResponseFactory();

	static bool			_isAllowedMethod(std::string const& method, ServerCfg const& cfg, Request const& req);
	static bool			_isCgi(ServerCfg const& cfg, Request const& req);
	static std::string	_normalizePath(std::string const& root, std::string const& uri);

public:

	static Response*	createResponse(ServerCfg const& cfg, Request const& req);
};

