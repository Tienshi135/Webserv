#pragma once

#include "Response.hpp"

class ResponseError : public Response
{
private:
	int	_errorCode;

public:
	ResponseError(ServerCfg const& cfg, Request const& req, int errorCode);
	virtual ~ResponseError();

	virtual void	buildResponse();
};

