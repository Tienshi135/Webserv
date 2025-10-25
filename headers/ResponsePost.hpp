#pragma once

#include "Response.hpp"

class ResponsePost : public Response
{

public:
	ResponsePost(ServerCfg const& cfg, Request const& req);
	virtual ~ResponsePost();

	virtual void	buildResponse();
};

