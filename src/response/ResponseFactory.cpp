#include "ResponseFactory.hpp"
#include "Response.hpp"
#include "ResponseError.hpp"
#include "ResponseGet.hpp"
#include "ResponsePost.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

Response*	ResponseFactory::createResponse(ServerCfg const& cfg, Request const& req)
{
	if (req.getMethod() == "GET")
		return new ResponseGet(cfg, req);
	if (req.getMethod() == "POST")
		return new ResponsePost(cfg, req);

	LOG_WARNING_LINK("Requested method: [" + req.getMethod() + "] not recognized, sendind error page 405");
	return new ResponseError(cfg, req);
}
