#pragma once

#include "header.hpp"

class Response;
class ResponseGet;

class ResponseFactory
{
private:
	ResponseFactory();
	~ResponseFactory();

public:

	static Response*	createResponse(ServerCfg const& cfg, Request const& req);
};

