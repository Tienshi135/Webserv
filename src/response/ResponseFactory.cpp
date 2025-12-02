#include "Response_headers/ResponseFactory.hpp"
#include "Response_headers/Response.hpp"
#include "Response_headers/ResponseError.hpp"
#include "Response_headers/ResponseGet.hpp"
#include "Response_headers/ResponsePost.hpp"
#include "Response_headers/ResponseDelete.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

bool	ResponseFactory::_isAllowedMethod(std::string const& method, ServerCfg const& cfg, Request const& req)
{
	Location const* location = cfg.getBestMatchLocation(req.getUri());
	std::vector<std::string> allowed;
	if (location)
		allowed = location->getMethods();
	else
		allowed = cfg.getMethods();

	if (std::find(allowed.begin(), allowed.end(), method) != allowed.end())
		return true;
	return false;
}

Response*	ResponseFactory::createResponse(ServerCfg const& cfg, Request const& req)
{
	if (!req.isValid())
	{
		LOG_WARNING_LINK("Invalid request received, returning 400 Bad Request");
		return (new ResponseError(cfg, req, 400));
	}

	if (req.isTooBig())
	{
		LOG_WARNING_LINK("Request too big, sending 413 payload to large");
		return (new ResponseError(cfg, req, 413));
	}

	std::string method = req.getMethod();

	if (!_isAllowedMethod(method, cfg, req))
		return (new ResponseError(cfg, req, 405));

	if (method == "GET")
		return (new ResponseGet(cfg, req));
	if (method == "POST")
		return (new ResponsePost(cfg, req));
	if (method == "DELETE")
		return (new ResponseDelete(cfg, req));

	LOG_WARNING_LINK("Requested method: [" + method + "] not recognized, sendind error page 405");
	return (new ResponseError(cfg, req, 405));
}
