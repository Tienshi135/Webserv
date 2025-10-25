#include "ResponseFactory.hpp"
#include "Response.hpp"
#include "ResponseError.hpp"
#include "ResponseGet.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

Response*	ResponseFactory::createResponse(ServerCfg const& cfg, Request const& req)
{
	if (req.getMethod() == "GET")
		return new ResponseGet(cfg, req);

	LOG_INFO_LINK("Requested method: [" + req.getMethod() + "] not recognized, sendind error page 400");
	return new ResponseError(cfg, req);
}
