#include "ResponsePost.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponsePost::ResponsePost(ServerCfg const& cfg, Request const& req)
: Response(cfg, req) {}

ResponsePost::~ResponsePost() {}

/*============== member function =============*/

void	ResponsePost::buildResponse(void)
{
	if (this->_req.getBodySize() > this->_cfg.getMaxBodySize())
	{
		this->responseIsErrorPage(403);
		return;
	}

	//get the location that has the path asked in the request
	Location const& location = this->_cfg.getLocationMap()[this->_req.getUri()];

	//build the path where we will create the file acording to location root
	std::string root = this->_cfg.getLocationMap();
	std::string	filePath = this->normalizePath()
	std::ofstream file()
	this->setStatus(201);
}

