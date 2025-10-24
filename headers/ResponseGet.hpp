#pragma once

#include "Response.hpp"

class ResponseGet : public Response
{

public:
	ResponseGet(ServerCfg const& cfg, Request const& req);
	~ResponseGet();

	virtual void	buildResponse();
};

