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
	//CGI meta-variables
	env["AUTH_TYPE"] = "";//not used much
	env["CONTENT_LENGTH"] = this->_req.getHeader("Content-Length");
	env["CONTENT_TYPE"] = this->_req.getHeader("Content-Type");
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	//for a request of https://somehost.com/cgi-bin/somescript.cgi/this.is.path;info
	env["PATH_INFO"] = this->_req.getUri();//would be /this.is.path;info
	env["PATH_TRANSLATED"] = this->_req.getUri();//would be /html/cgi-bin/somescript.cgi/this.is.path;info
	env["QUERY_STRING"] = "";//whatever is after the ? in the uri
	env["REMOTE_ADDR"];//TODO get from socket ip address of client
	env["REMOTE_HOST"] = "NULL";//TODO get from socket but can be set to null
	//env["REMOTE_IDENT"];//not used much
	//env["REMOTE_USER"];//not used much but must be set if AUTH_TYPE is set
	env["REQUEST_METHOD"] = this->_req.getMethod();
	env["SCRIPT_NAME"] = "";//example /cgi-bin/somescript.cgi
	env["SERVER_NAME"] = this->_cfg.getName();
	env["SERVER_PORT"] = numToString(static_cast<int>(this->_cfg.getPort()));
	env["SERVER_PROTOCOL"] = this->_req.getVersion();
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	
	//HTTP meta-variables
	env["HTTP_ACCEPT"] = this->_req.getHeader("Accept");
	env["HTTP_ACCEPT_CHARSET"] = this->_req.getHeader("Accept-Charset");
	env["HTTP_ACCEPT_ENCODING"] = this->_req.getHeader("Accept-Encoding");
	env["HTTP_ACCEPT_LANGUAGE"] = this->_req.getHeader("Accept-Language");
	env["HTTP_EXPECT"] = this->_req.getHeader("Expect");
	env["HTTP_FROM"] = this->_req.getHeader("From");
	env["HTTP_IF_MATCH"] = this->_req.getHeader("If-Match");
	env["HTTP_IF_MODIFIED_SINCE"] = this->_req.getHeader("If-modified-Since");
	env["HTTP_IF_NONE_MATCH"] = this->_req.getHeader("If-None-Match");
	env["HTTP_IF_RANGE"] = this->_req.getHeader("If-Range");
	env["HTTP_IF_UNMODIFIED_SINCE"] = this->_req.getHeader("If-Unmodified-Since");
	env["HTTP_MAX_FORWARDS"] = this->_req.getHeader("Max-Forwards");
	env["HTTP_RANGE"] = this->_req.getHeader("Range");
	env["HTTP_REFERER"] = this->_req.getHeader("Referer");
	env["HTTP_USER_AGENT"] = this->_req.getHeader("User-Agent");

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
