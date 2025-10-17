#include "header.hpp"
#include <fstream>
#include <algorithm>

/*TODO: find a better convention for multiple tokens*/
Server	configServer(std::ifstream& file)
{
	std::string	line;
	Server		server;

	while (std::getline(file, line))
	{
		std::vector<std::string> tknLine = tokenizeLine(line);

		if (tknLine.empty() ||  tknLine.front().empty() || tknLine.front()[0] == '#')
			continue;
		if (tknLine.front() == "}")
			break;
		if (tknLine.front() == "server")
			throw std::runtime_error("Error: server has no closing braces");
		if (tknLine.size() > 2)//We manage only 2 tokens per directive for convenience, with the exepcion of return and error page
		{
			if (tknLine.front() == "location")
			{
				if (parseLocation(server, tknLine, file) < 0)
					throw std::runtime_error("Error: Location parsing failed");
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
				server.setErrorPage(tknLine[2]);  // Store the path
				continue;
			}
			else
			{
				std::cerr << tknLine[0] << ", " << tknLine[1]<< std::endl;
				std::cerr << tknLine.size() << " at: "<< __FILE__ << ":" << __LINE__ << std::endl;
				throw std::runtime_error("Error: too many arguments for directive");
			}
		}

		std::string directive = tknLine.front();
		std::string value = tknLine.back();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
		{
			std::cerr << directive << " at: "<< __FILE__ << __LINE__ << std::endl;
			throw std::runtime_error("Error: unknown directive");
		}

		setDirective(server, directiveType, value);
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: server has no closing braces");
	return server;
}

/*TODO: find a better convention for multiple tokens*/
int	parseLocation(Server& server, std::vector<std::string>& locationLine, std::ifstream &file)
{
	std::string line;
	std::string locationPath;

	std::vector<std::string>::iterator	it = locationLine.begin();
	if (!isLocation(locationLine ,it, file, locationPath))
	{
		std::cerr << "Error: location has no path or open braces at: " << __FILE__ << ":" << __LINE__ << std::endl;
		return -1;
	}

	std::vector<std::string> badToken;
	badToken.push_back("location");
	badToken.push_back("server");
	badToken.push_back("{");

	while (std::getline(file, line))
	{
		std::vector<std::string> tknLine = tokenizeLine(line);

		if (tknLine.empty() || (!tknLine.empty() && tknLine.front()[0] == '#'))
			continue;
		if (tknLine.front() == "}")
			break;
		if (hasCommonElement(tknLine, badToken))
		{
			std::cerr << "Error: location has no closing braces at: " << __FILE__ << ":" << __LINE__  << std::endl;
			return -1;
		}
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
			{
				std::cerr << "Error: too many elements: [" << tknLine.size() << "] for directive at: " << __FILE__ << ":" << __LINE__  << std::endl;
				return -1;
			}
		}

		std::string directive = tknLine.front();
		std::string value = tknLine.back();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
		{
			std::cerr << directive << " at: "<< __FILE__ << __LINE__ << std::endl;
			throw std::runtime_error("Error: unknown directive");
		}

		setLocationDirective(server, directiveType, value, locationPath);
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: location has no closing braces");

	std::cout << "Parsed location: " << locationPath << std::endl;
	return (0);

}

bool parse(std::map<std::string, Server> &buffer, char *path)
{
	std::ifstream		file;
	std::string			line;
	std::vector<Server>	serverList;
	Server serverInstance;

	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Error could'nt open file" << std::endl;
		buffer.clear();
		return (false);
	}

	while (std::getline(file, line))//main loop. if a server with open brace is found, store the content in a Server and add it to map
	{
		std::vector<std::string> tknLine = tokenizeLine(line);//Line string is properly divided in "tokens"
		if (tknLine.empty() ||  tknLine.front().empty() || tknLine.front()[0] == '#')//skipt comments and blank lines
			continue;

		std::vector<std::string>::iterator	it;
		it = std::find(tknLine.begin(), tknLine.end(), "server");
		if (it != tknLine.end())//server found, manage errors and fill it
		{
			if (!isServer(tknLine, it, file))
			{
				std::cerr << "Error: No opening brace found for server block" << std::endl;
				buffer.clear();
				return false;
			}
			try
			{
				serverInstance = configServer(file);//fills creates an instance of server and fills it with all the directives and locations
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				buffer.clear();
				return false;
			}
			buffer.insert(std::pair<std::string, Server>(serverInstance.getName(), serverInstance));//add server to the pool of servers, restart the loop
		}
	}
	/* TODO: check for duplicated servers and/or ip ,using map logic */
	file.close();
	return (true);
}
