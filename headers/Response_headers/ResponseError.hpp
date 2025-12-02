#pragma once

#include "Response.hpp"

class ResponseError : public Response
{
private:
	e_errorcode	_errorCode;

public:
	ResponseError(ServerCfg const& cfg, Request const& req, e_errorcode errorCode);
	virtual ~ResponseError();

	virtual void	buildResponse();
};

