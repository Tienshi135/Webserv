#pragma once

#include "header.hpp"

class Response;

class ResponseFactory
{
private:
	ResponseFactory();
	~ResponseFactory();

	static bool	_isAllowedMethod(std::string const& method, ServerCfg const& cfg, Request const& req);

public:

	static Response*	createResponse(ServerCfg const& cfg, Request const& req);
};

