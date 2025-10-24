#pragma once

#include "Response.hpp"

class ResponseError : public Response
{

public:
	ResponseError(ServerCfg const& cfg, Request const& req);
	~ResponseError();

	virtual void	buildResponse();
};

