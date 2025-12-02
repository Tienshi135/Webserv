#pragma once

#include "Response.hpp"

class ResponseDelete : public Response
{
	private:

	public:
		ResponseDelete(ServerCfg const& cfg, Request const& req);
		virtual ~ResponseDelete();

		virtual void	buildResponse();
};

