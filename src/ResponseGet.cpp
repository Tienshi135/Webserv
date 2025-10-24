#include "ResponseGet.hpp"
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

	if (!this->isSecurePath(reqPath))
	{
		this->responseIsErrorPage(403);
		LOG_HIGH_WARNING("Directory traversal attack detected: [" + reqPath + "] sending 403 no permission");
		return ;
	}

	if (reqPath == "/")
	{
		LOG_INFO_LINK("No specific path requested, trying to send index.html");
		path = this->normalizePath( this->_cfg.getRoot(), this->_cfg.getIndex());
	}
	else
		path =  this->normalizePath(this->_cfg.getRoot(), this->_req.getUri());

	//set the file requested as body if exits, error page if not, fill first line and headers accordingly
	this->_bodyIsFile = this->sendFileAsBody(path);

}

