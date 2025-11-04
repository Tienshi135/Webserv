#include "ResponseDelete.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseDelete::ResponseDelete(ServerCfg const& cfg, Request const& req)
: Response(cfg, req)
{

}

ResponseDelete::~ResponseDelete() {}

/*============== private member functions =============*/


/*============== member function =============*/

void	ResponseDelete::buildResponse(void)
{
	if (this->_req.getBodySize() > this->_cfg.getMaxBodySize())
	{
		this->responseIsErrorPage(413);
		return;
	}

	std::string deletePath;

	Location const* location = this->_cfg.findMatchingLocation(this->_req.getUri());
	if (!location)
		deletePath = normalizePath(this->_cfg.getRoot(), this->_req.getUri());
	else
		deletePath = normalizePath(location->getRoot(), this->_req.getUri());

	if (this->validateFilePath(deletePath) < 0)
		return;
	//TODO check if DELETE is allowed in this location, if not send 405 method nod allowed. We have refactor to properly store allowed methods first
	if (!this->isSecurePath(deletePath))
	{
		this->responseIsErrorPage(403);
		LOG_HIGH_WARNING("Directory traversal attack detected: [" + deletePath + "] sending 403 no permission");
		return ;
	}

	if (std::remove(deletePath.c_str()) != 0)
	{
		this->responseIsErrorPage(500);
		LOG_HIGH_WARNING("DELETE failed: remove() error [" + deletePath + "]");
		return;
	}

	std::string httpMsg = "<html><body><h1>Deleted " + deletePath + "</h1></body></html>";
	// this->addHeader("Location", resourceUri);
	this->setStatus(200);
	this->setBody(httpMsg, "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload
	LOG_INFO("DELETE successful: [" + deletePath + "]");
}
