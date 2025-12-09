#include "Response_headers/ResponseGet.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseGet::ResponseGet(ServerCfg const& cfg, Request const& req)
: Response(cfg, req) {}

ResponseGet::~ResponseGet() {}

/*============== member function =============*/

void	ResponseGet::buildResponse(void)
{
	//build the path to the requested file using the root of the config
	std::string	reqPath = this->_req.getUri();
	std::string path;

	if (!this->_isSecurePath(reqPath))
	{
		this->_responseIsErrorPage(FORBIDDEN);
		LOG_HIGH_WARNING("Directory traversal attack detected: [" + reqPath + "] sending 403 no permission");
		return ;
	}

	Location const* location = this->_cfg.getBestMatchLocation(reqPath);

	if (!location)
	{
		if (reqPath == "/")
		{
			LOG_INFO_LINK("No specific path requested, trying to send index.html");
			path = this->_normalizePath( this->_cfg.getRoot(), this->_cfg.getIndex());
		}
		else
			path =  this->_normalizePath(this->_cfg.getRoot(), reqPath);
	}
	else
	{
		this->_autoindex = location->getAutoindex();

		std::string locationPath = location->getLocationPath();
		std::string relativePath = reqPath;

		if (reqPath.compare(0, locationPath.size(), locationPath) == 0)
		{
			relativePath = reqPath.substr(locationPath.size());

			if (relativePath.empty())
				relativePath = "/";
			else if (relativePath[0] != '/')
				relativePath = "/" + relativePath;
		}
		if (relativePath == "/" || relativePath.empty())
		{
			LOG_INFO_LINK("No specific path requested, trying to send index.html from location: " + location->getLocationPath());
			path = this->_normalizePath(location->getRoot(), location->getIndex());
		}
		else
			path =  this->_normalizePath(location->getRoot(), relativePath);
	}

	this->_addStandardHeaders();
	//set the file requested as body if exits, error page if not, fill first line and headers accordingly
	this->_sendFileAsBody(path);//TODO BUG if it sends a custom error page, it won't work because all errors sets bodyIsFile to false;

}

