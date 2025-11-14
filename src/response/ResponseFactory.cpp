#include "ResponseFactory.hpp"
#include "Response.hpp"
#include "ResponseError.hpp"
#include "ResponseGet.hpp"
#include "ResponsePost.hpp"
#include "ResponseDelete.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

Response*	ResponseFactory::createResponse(ServerCfg const& cfg, Request const& req)
{
	if (!req.isValid())
	{
		LOG_WARNING("Invalid request received, returning 400 Bad Request");
		return new ResponseError(cfg, req, 400);
	}

	//TODO add a check for location allowed method here. Is already checked in the clases but is too late there and duplicates code.

	if (req.getMethod() == "GET")
		return new ResponseGet(cfg, req);
	if (req.getMethod() == "POST")
		return new ResponsePost(cfg, req);
	if (req.getMethod() == "DELETE")
		return new ResponseDelete(cfg, req);

	LOG_WARNING_LINK("Requested method: [" + req.getMethod() + "] not recognized, sendind error page 405");
	return new ResponseError(cfg, req, 405);
}
