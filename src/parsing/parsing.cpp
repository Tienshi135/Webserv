#include "header.hpp"
#include <fstream>
#include <algorithm>

ServerCfg	parseServer(File& file)
{
	std::string	line;
	ServerCfg		server;

	while (std::getline(file.file, line))
	{
		file.nLines++;
		std::vector<std::string> tknLine = tokenizeLine(line, file.nLines);

		if (tknLine.empty() ||  tknLine.front().empty())
			continue;
		if (tknLine.front() == "}")
			break;
		if (tknLine.front() == "server")
			throw ERR_PARS_CFGLN("server has no closing braces", file.nLines);

		if (foundLocation(tknLine, file))
		{
			parseLocation(server, tknLine, file);
			continue;
		}

		std::string directive = tknLine.front();
		e_configtype directiveType = findType(directive);
		if (directiveType == UNKNOWN)
			throw ERR_PARS_CFGLN("Unknown directive", file.nLines);

		if (tknLine.size() < 2)
			throw ERR_PARS_CFGLN("Directive [" + directive + "] has no value", file.nLines);
		std::vector<std::string> value;
		value.assign(tknLine.begin() + 1, tknLine.end());
		setDirective(server, directiveType, value);
	}
	if (line.find("}") == std::string::npos)
		throw ERR_PARS_CFGLN("server has no closing braces", file.nLines);
	return server;
}

void	parseLocation(ServerCfg& server, std::vector<std::string>& locationLine, File& file)
{
	std::string line;
	std::string locationPath;

	std::vector<std::string> badToken;
	badToken.push_back("location");
	badToken.push_back("server");
	badToken.push_back("{");

	if (locationLine.size() < 2)
		throw ERR_PARS_CFGLN("location directive missing path", file.nLines);
	locationPath = locationLine[1];
	if (!isValidLocationPath(locationPath))
		throw ERR_PARS_CFGLN("Invalid location path", file.nLines);

	while (std::getline(file.file, line))
	{
		file.nLines++;
		std::vector<std::string> tknLine = tokenizeLine(line, file.nLines);

		if (tknLine.empty() || tknLine.front().empty())
			continue;
		if (tknLine.front() == "}")
			break;
		if (hasCommonElement(tknLine, badToken))
			throw ERR_PARS_CFGLN("Location has no closing braces", file.nLines);

		std::string directive = tknLine.front();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
			throw ERR_PARS_CFGLN("Unknown location directive", file.nLines);

		if (tknLine.size() < 2)
			throw ERR_PARS_CFGLN("Directive [" + directive + "] has no value", file.nLines);
		std::vector<std::string> value;
		value.assign(tknLine.begin() + 1, tknLine.end());

		setLocationDirective(server, directiveType, value, locationPath);
	}
	if (line.find("}") == std::string::npos)
		throw ERR_PARS_CFGLN("Location has no closing braces", file.nLines);
}

void parse(std::vector<ServerCfg> &buffer, char *path)
{
	File				file;
	std::string			line;
	ServerCfg			serverInstance;

	file.nLines = 0;
	file.file.open(path);
	if (!file.file.is_open())
		throw ERR_PARS("Could'nt open config file");

	while (std::getline(file.file, line))//main loop. if a server with open brace is found, store the content in a Server and add it to servers map
	{
		file.nLines++;
		std::vector<std::string> tknLine = tokenizeLine(line, file.nLines);//Line string is properly divided in "tokens"
		if (tknLine.empty() ||  tknLine.front().empty())//skipt blank lines
			continue;

		if (foundServer(tknLine, file))//server found, manage errors and fill it
		{
			serverInstance = parseServer(file);//fills creates an instance of server and fills it with all the directives and locations
			setDefaults(serverInstance);
			if (!serverInstance.minValidCfg())//minimun settings check
				throw ERR_PARS("Abort");
			buffer.push_back(serverInstance);//add server to the pool of servers, restart the loop
		}
	}
	/* TODO: check for duplicated servers and/or ip ,using map logic ? need documentation to check if it is needed */
	file.file.close();
}
