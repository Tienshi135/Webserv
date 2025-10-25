#include "ResponseError.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseError::ResponseError(ServerCfg const& cfg, Request const& req)
: Response(cfg, req) {}

ResponseError::~ResponseError() {}

/*============== member function =============*/

void	ResponseError::buildResponse(void)
{
	this->responseIsErrorPage(405);
}

