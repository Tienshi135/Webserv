#include "ResponseDelete.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseDelete::ResponseDelete(ServerCfg const& cfg, Request const& req)
: Response(cfg, req) {}

ResponseDelete::~ResponseDelete() {}

/*============== private member functions =============*/


/*============== member function =============*/

void	ResponseDelete::buildResponse(void)
{
	std::string deletePath;

	Location const* location = this->_cfg.getBestMatchLocation(this->_req.getUri());
	if (!location)
		deletePath = _normalizePath(this->_cfg.getRoot(), this->_req.getUri());
	else
	{
		std::string uriPath = this->_req.getUri().substr(location->getLocationPath().size());
		deletePath = _normalizePath(location->getRoot(), uriPath);
	}

	if (this->_validateFilePath(deletePath) < 0)
		return;

	if (!this->_isSecurePath(deletePath))
	{
		this->_responseIsErrorPage(FORBIDDEN);
		LOG_HIGH_WARNING("Directory traversal attack detected: [" + deletePath + "] sending 403 no permission");
		return ;
	}

	if (std::remove(deletePath.c_str()) != 0)
	{
		this->_responseIsErrorPage(INTERNAL_SERVER_ERROR);
		LOG_HIGH_WARNING("DELETE failed: remove() error [" + deletePath + "]");
		return;
	}

	std::string httpMsg = "<html><body><h1>Deleted " + deletePath + "</h1></body></html>";
	// this->addHeader("Location", resourceUri);
	this->_setStatus(OK);
	this->_setBody(httpMsg, "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload
	this->_bodyIsFile = false;
	LOG_INFO("DELETE successful: [" + deletePath + "]");
}
