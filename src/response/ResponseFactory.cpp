#include "Response_headers/ResponseFactory.hpp"
#include "Response_headers/Response.hpp"
#include "Response_headers/ResponseError.hpp"
#include "Response_headers/ResponseGet.hpp"
#include "Response_headers/ResponsePost.hpp"
#include "Response_headers/ResponseDelete.hpp"
#include "Response_headers/ResponseCgi.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

std::string	ResponseFactory::_normalizePath(std::string const& root, std::string const& uri)
{
	std::string	normalizedRoot = root;
	std::string	normalizedUri = uri;

	if (!root.empty() && root[root.size() - 1] != '/')
		normalizedRoot += "/";


	if (!uri.empty() && uri[0] == '/')
		normalizedUri = uri.substr(1);

	return normalizedRoot + normalizedUri;
}

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

bool	ResponseFactory::_isCgi(ServerCfg const& cfg, Request const& req)
{
	//0. cgi is only allowed in GET or POST petitions
	if (req.getMethod() != "GET" && req.getMethod() != "POST")
	{
		LOG_ERROR_LINK("methtod not supported for CGI");//TODO debug pourposes delete later
		return false;
	}

	//1. check if uri is inside the proper location and enabled
	std::string uri = req.getUri();
	Location const* location = cfg.getBestMatchLocation(uri);
	if (!location)
		return (false);

	if (!location->isCgiEnabled())
	{
		LOG_ERROR_LINK("CGI not enabled");//TODO debug pourposes delete later
		return (false);
	}

	//2. sacar la extension
	std::string path = uri.substr(0, uri.find('?'));// extract the actual path from the uri
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
	{
		LOG_WARNING_LINK("executables with no extension are non supported");
		return (false);
	}

	//3. extension is allowed by cfg
	std::string ext = path.substr(dotPos);
	if (!location->isValidExtension(ext))
	{
		LOG_WARNING_LINK("Requested extension [" + ext + "] is not allowed by cfg");
		return (false);
	}

	//4. there is an executable file inside the location root matching the request
	std::string relative = path.substr(location->getLocationPath().size());
	std::string scriptFilePath = ResponseFactory::_normalizePath(location->getRoot(), relative);
	if (!pathIsRegFile(scriptFilePath))
	{
		LOG_WARNING_LINK("[" + scriptFilePath + "] is not a regular file");
		return false;
	}

	LOG_INFO_LINK("CGI detected");
	return (true);
}

Response*	ResponseFactory::createResponse(ServerCfg const& cfg, Request const& req)
{
	if (!req.isValid())
	{
		LOG_WARNING_LINK("Invalid request received, returning 400 Bad Request");
		return (new ResponseError(cfg, req, BAD_REQUEST));
	}

	if (req.isTooBig())
	{
		LOG_WARNING_LINK("Request too big, sending 413 payload to large");
		return (new ResponseError(cfg, req, PAYLOAD_TOO_LARGE));
	}

	std::string method = req.getMethod();

	if (!_isAllowedMethod(method, cfg, req))
		return (new ResponseError(cfg, req, METHOD_NOT_ALLOWED));

	if (method == "POST" && req.getExpectedBodySize() == 0)
		return (new ResponseError(cfg, req, NO_CONTENT));

	if (ResponseFactory::_isCgi(cfg, req))
		return (new ResponseCgi(cfg, req));// this object will fork


	if (method == "GET")
		return (new ResponseGet(cfg, req));
	if (method == "POST")
		return (new ResponsePost(cfg, req));
	if (method == "DELETE")
		return (new ResponseDelete(cfg, req));

	LOG_WARNING_LINK("Requested method: [" + method + "] not recognized, sendind error page 405");
	return (new ResponseError(cfg, req, METHOD_NOT_ALLOWED));
}
