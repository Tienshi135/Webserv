#include "ResponseError.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseError::ResponseError(ServerCfg const& cfg, Request const& req, int errorCode)
: Response(cfg, req), _errorCode(errorCode) {}

ResponseError::~ResponseError() {}

/*============== member function =============*/

void	ResponseError::buildResponse(void)
{
	this->responseIsErrorPage(this->_errorCode);
}

