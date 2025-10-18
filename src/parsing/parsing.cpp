#include "header.hpp"
#include <fstream>
#include <algorithm>

/*TODO: find a better convention for multiple tokens*/
/*TODO: missing closing brace at the end of the file is not detected*/
Server	configServer(File& file)
{
	std::string	line;
	Server		server;

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
		if (tknLine.size() > 2)//We manage only 2 tokens per directive for convenience, with the exepcion of return and error page
		{
			if (tknLine.front() == "location")
			{
				parseLocation(server, tknLine, file);
				continue;
			}
			else if (tknLine.front() == "return" && tknLine.size() == 3)//special case for return, we manage 3 token, ex: return 301 /new-locaton
			{
				std::string returnValue = tknLine[1] + " " + tknLine[2];
				server.setReturn(returnValue);
				continue;
			}
			else if (tknLine.front() == "error_page" && tknLine.size() == 3)//special case for error_page, we manage 3 token, ex: error_page 404 /error/404.html
			{
				server.setErrorPage(tknLine[2]);// Store the path
				continue;
			}
			else
				throw ERR_PARS_CFGLN("Too many arguments for directive", file.nLines);
		}

		std::string directive = tknLine.front();
		std::string value = tknLine.back();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
		{
			std::cerr << directive << " at: "<< __FILE__ << __LINE__ << std::endl;
			throw ERR_PARS_CFGLN("Unknown directive", file.nLines);
		}

		setDirective(server, directiveType, value);
	}
	if (line.find("}") == std::string::npos)
		throw ERR_PARS_CFGLN("server has no closing braces", file.nLines);
	return server;
}

/*TODO: find a better convention for multiple tokens*/
void	parseLocation(Server& server, std::vector<std::string>& locationLine, File& file)
{
	std::string line;
	std::string locationPath;

	std::vector<std::string>::iterator	it = locationLine.begin();
	if (!isLocation(locationLine ,it, file, locationPath))
		throw ERR_PARS_CFGLN("Location has no path or open braces", file.nLines);

	std::vector<std::string> badToken;
	badToken.push_back("location");
	badToken.push_back("server");
	badToken.push_back("{");

	while (std::getline(file.file, line))
	{
		file.nLines++;
		std::vector<std::string> tknLine = tokenizeLine(line, file.nLines);

		if (tknLine.empty())
			continue;
		if (tknLine.front() == "}")
			break;
		if (hasCommonElement(tknLine, badToken))
			throw ERR_PARS_CFGLN("Location has no closing braces", file.nLines);
		if (tknLine.size() > 2)
		{
			if (tknLine.front() == "return" && tknLine.size() == 3)
			{
				std::string returnValue = tknLine[1] + " " + tknLine[2];
				e_configtype directiveType = RETURN;
				setLocationDirective(server, directiveType, returnValue, locationPath);
				continue;
			}
			else if (tknLine.front() == "error_page" && tknLine.size() == 3)
			{
				std::string returnValue = tknLine[1] + " " + tknLine[2];
				e_configtype directiveType = ERROR_PAGE;
				setLocationDirective(server, directiveType, returnValue, locationPath);
				continue;
			}
			else
				throw ERR_PARS_CFGLN("Too many elements for directive", file.nLines);
		}

		std::string directive = tknLine.front();
		std::string value = tknLine.back();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
		{
			std::cerr << directive << " at: "<< __FILE__ << __LINE__ << std::endl;
			throw ERR_PARS_CFGLN("Unknown directive", file.nLines);
		}

		setLocationDirective(server, directiveType, value, locationPath);
	}
	if (line.find("}") == std::string::npos)
		throw ERR_PARS_CFGLN("Location has no closing braces", file.nLines);

	std::cout << "Parsed location: " << locationPath << std::endl;
}

void parse(std::map<std::string, Server> &buffer, char *path)
{
	File				file;
	std::string			line;
	std::vector<Server>	serverList;
	Server serverInstance;

	file.nLines = 0;
	file.file.open(path);
	if (!file.file.is_open())
		throw ERR_PARS("Could'nt open config file");

	while (std::getline(file.file, line))//main loop. if a server with open brace is found, store the content in a Server and add it to map
	{
		file.nLines++;
		std::vector<std::string> tknLine = tokenizeLine(line, file.nLines);//Line string is properly divided in "tokens"
		if (tknLine.empty() ||  tknLine.front().empty())//skipt blank lines
			continue;

		std::vector<std::string>::iterator	it;
		it = std::find(tknLine.begin(), tknLine.end(), "server");
		if (it != tknLine.end())//server found, manage errors and fill it
		{
			if (!isServer(tknLine, it, file))
				throw ERR_PARS_CFGLN("No opening brace found for server block", file.nLines);
			serverInstance = configServer(file);//fills creates an instance of server and fills it with all the directives and locations
			buffer.insert(std::pair<std::string, Server>(serverInstance.getName(), serverInstance));//add server to the pool of servers, restart the loop
		}
	}
	/* TODO: check for duplicated servers and/or ip ,using map logic ? need documentation to check if it is needed */
	file.file.close();
}
