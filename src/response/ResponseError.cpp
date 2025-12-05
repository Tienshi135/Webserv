#include "Response_headers/ResponseError.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseError::ResponseError(ServerCfg const& cfg, Request const& req, e_errorcode errorCode)
: Response(cfg, req), _errorCode(errorCode) {}

ResponseError::~ResponseError() {}

/*============== member function =============*/

void	ResponseError::buildResponse(void)
{
	this->_addStandardHeaders();
	this->_responseIsErrorPage(this->_errorCode);
}

