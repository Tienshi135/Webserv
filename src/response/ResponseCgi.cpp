#include "Response_headers/ResponseCgi.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponseCgi::ResponseCgi(ServerCfg const& cfg, Request const& req)
: Response(cfg, req)
{
	std::string uri = req.getUri();
	std::string path = uri.substr(0, uri.find('?'));
	size_t dotPos = path.find_last_of('.');

	this->_ext = path.substr(dotPos + 1);
	this->_execType = this->_getExecType();
	this->_executor = this->_getExecutor();
	this->generateEnvironment(this->_enviroment);
}

ResponseCgi::~ResponseCgi() {}


/*============== Member functions =============*/


ResponseCgi::e_exectype ResponseCgi::_getExecType(void)
{
	std::string ext = this->_ext;

	if (ext == ".sh")
		return (BASH);
	if (ext == ".py")
		return (PYTHON);
	if (ext == ".php")
		return (PHP);

	return (UNKNOWNEXECT);
}

void	ResponseCgi::buildResponse(void)
{
	switch (this->_execType)
	{
	case BASH:
		this->_buildFromBash();
		break;
	case PYTHON:
		this->_buildFromPython();
		break;
	case PHP:
		this->_buildFromPhp();
		break;
	default:
		this->_responseIsErrorPage(NOT_IMPLEMENTED);
	}

}

/*============== private Member functions =============*/

void ResponseCgi::generateEnvironment(std::map<std::string, std::string>& env)
{
	env[""] = "";//TODO this is a placeholder;
}
std::string	ResponseCgi::_getExecutor(void)
{
	Location const *location = this->_cfg.getBestMatchLocation(this->_req.getUri());
	if (!location)
	{
		LOG_HIGH_WARNING_LINK("can't find matching location");
		return ("");
	}

	return location->getExtensionExecutor(this->_ext);
}

void	ResponseCgi::_buildFromBash(void)
{
	this->_responseIsErrorPage(NOT_IMPLEMENTED);//TODO implement
}

void	ResponseCgi::_buildFromPython(void)
{
	this->_responseIsErrorPage(NOT_IMPLEMENTED);//TODO implement
}
void	ResponseCgi::_buildFromPhp(void)
{
	this->_responseIsErrorPage(NOT_IMPLEMENTED);//TODO implement
}
