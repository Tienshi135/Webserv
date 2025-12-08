#pragma once

#include "Response_headers/Response.hpp"

class ResponseCgi : public Response
{
	private:
		enum e_exectype
		{
			BASH,
			PYTHON,
			PHP,
			UNKNOWNEXECT
		};

		std::string 						_scriptName;
		std::string 						_scriptPath;
		std::string							_ext;
		e_exectype							_execType;
		std::string							_executor;
		std::map<std::string, std::string>	_enviroment;

	//builders
		void	_buildFromBash(void);
		void	_buildFromPython(void);
		void	_buildFromPhp(void);

	//tools
		e_exectype	_getExecType(void);
		std::string	_getExecutor(void);
		std::string _extractQueryString(void);
		char**		_mapToTable(std::map<std::string, std::string> const& envMap);
		void		_parseAndSend(std::string const& output);

	public:
		ResponseCgi(ServerCfg const& cfg, Request const& req);
		virtual ~ResponseCgi(void);
		void generateEnvironment(std::map<std::string, std::string>& env);

		virtual void	buildResponse(void);
};

