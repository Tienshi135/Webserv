#include "header.hpp"
#include <fstream>
#include <algorithm>
#include <map>
#include <sys/stat.h>


bool hasCommonElement(std::vector<std::string>& v1, std::vector<std::string>& v2)
{
	std::vector<std::string>::iterator	it;
	for (it = v1.begin(); it != v1.end(); it++)
	{
		if (std::find(v2.begin(), v2.end(), *it) != v2.end())
			return true;
	}
	return false;
}

bool pathExists(const std::string &path)
{
	struct stat info;
	return (stat(path.c_str(), &info) == 0);
}

bool isValidLocationPath(const std::string &path)
{
	if (path.empty() || path[0] != '/')
		return false;
	for (size_t i = 0; i < path.size(); ++i) {
		char c = path[i];
		if (c == ' ' || c == '\t' || c == '\n')
			return false;
	}
	return true;
}

std::vector<std::string>	tokenizeLine(std::string& line)
{
	std::string					token;
	std::vector<std::string>	tokenized;
	std::stringstream			ss(line);

	while (ss >> std::ws)
	{
		char quote = ss.peek();// check first character for quotes
		if (quote == '"' || quote == '\'')
		{
			ss.get();//we consume the detected quote
			if (!getline(ss, token, quote))// Fill token until reaches the same quote type
				std::runtime_error("Error: unclosed quotes in config file");//better erro msg? keep track of getline number of lines?
			tokenized.push_back(token);
		}
		else
		{
			ss >> token;
			tokenized.push_back(token);
		}
	}
	return tokenized;
}
void	setLocationDirective(Server& server, e_configtype& directive, std::string& value, std::string& locationPath)
{
	Location tempLocation;

	switch (directive)
	{
		case(METHODS):
			tempLocation.setMethods(value);
			break;
		case(RETURN):
			tempLocation.setReturn(value);
			break;
		case(ROOT):
			tempLocation.setRoot(value);
			break;
		case(AUTOINDEX):
			tempLocation.setAutoindex(value == "on" || value == "true" || value == "1");
			break;
		case(INDEX):
			tempLocation.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			tempLocation.setMaxBodySize(parseSize(value));
			break;
		case(STORE):
			tempLocation.setStore(value);
			break;
		case(UNKNOWN):
			std::cout << "Unknown directive in location: " << std::endl;
			break;
		default:
			std::cout << "Directive not allowed in location block: " << std::endl;
			break;
	}

	std::map<std::string, Location> currentMap = server.getLocationMap();
	currentMap[locationPath] = tempLocation;
	server.setLocationMap(currentMap);
}
void	setDirective(Server& server, e_configtype& directive, std::string& value)//review this function logic later to look for optimisation
{
	switch (directive)
	{
		case(SERVER_NAME):
			server.setName(value);
			break;
		case(HOST):
			{
				size_t colonPos = value.find("::");
				if (colonPos != std::string::npos)
				{
					std::string host_part = value.substr(0, colonPos);
					std::string port_part = value.substr(colonPos + 2);
					server.setHost(host_part);
					server.setPort(static_cast<unsigned int>(atol(port_part.c_str())));
				}
				else
					server.setHost(value);// might want to change
			}
			break;
		case(ERROR_PAGE):
			server.setErrorPage(value);
			break;
		case(BODY_SIZE):
			server.setBodySize(parseSize(value));
			break;

		// Configuration case
		case(METHODS):
			server.setMethods(value);
			break;
		case(RETURN):
			server.setReturn(value);
			break;
		case(ROOT):
			server.setRoot(value);
			break;
		case(AUTOINDEX):
			server.setAutoindex(value == "on" || value == "true" || value == "1");
			break;
		case(INDEX):
			server.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			server.setMaxBodySize(parseSize(value));
			break;
		case(STORE):
			server.setStore(value);
			break;
		case(UNKNOWN):
			std::cout << "Unknown directive: " << std::endl;
			break;
		default:
			break;
	}
}

Server	configServer(std::ifstream& file)
{
	std::string	line;
	Server		server;

	while (std::getline(file, line, '}'))
	{
		std::vector<std::string> tkLine = tokenizeLine(line);

		if (tkLine.empty() || (!tkLine.empty() && tkLine.front()[0] == '#'))
			continue;
		if (tkLine.front() == "server")
			throw std::runtime_error("Error: server has no closing braces");
		if (tkLine.size() > 2)
		{
			if (tkLine.front() == "location")
				parseLocation(server, tkLine, file);
			else
			{
				std::cerr << tkLine[0] << ", " << tkLine[1]<< std::endl;
				std::cerr << tkLine.size() << " at: "<< __FILE__ << __LINE__ << std::endl;
				throw std::runtime_error("Error: too many arguments for directive");
			}
		}


		std::string directive = tkLine.front();
		std::string value = tkLine.back();
		e_configtype directiveType = findType(directive);

		if (directiveType == UNKNOWN)
		{
			std::cerr << directive << " at: "<< __FILE__ << __LINE__ << std::endl;
			throw std::runtime_error("Error: unknown directive");
		}

		setDirective(server, directiveType, value);
	}
	if (!line.find("}"))
		throw std::runtime_error("Error: server has no closing braces");
	return server;
}

bool	isServer(std::vector<std::string>::iterator& it, std::ifstream& file)
{
	std::vector<std::string>	tkLine;
	std::string					line;

	it++;
	if (it != tkLine.end() && *it == "{")
		return true;
	if (it != tkLine.end())
		return false;
	int	linesChecked = 0;
	while (std::getline(file, line) && linesChecked <= 1)
	{
		tkLine = tokenizeLine(line);
		if (tkLine.empty() || (!tkLine.empty() && tkLine.front()[0] == '#'))
		{
			linesChecked++;
			continue;
		}
		if (tkLine.front() == "{")
			return true;
		return false;
	}
	return false;
}

bool	isLocation(std::vector<std::string>::iterator& it, std::ifstream& file, std::string& locationPath)
{
	std::vector<std::string>	tkLine;
	std::string					line;

	it++;
	if (!isValidLocationPath(*it))
		return false;
	locationPath = *it;
	it++;
	if (it != tkLine.end() && *it == "{")
		return true;
	if (it != tkLine.end())
		return false;
	int	linesChecked = 0;
	while (std::getline(file, line) && linesChecked <= 1)
	{
		tkLine = tokenizeLine(line);
		if (tkLine.empty() || (!tkLine.empty() && tkLine.front()[0] == '#'))
		{
			linesChecked++;
			continue;
		}
		if (tkLine.front() == "{")
			return true;
		return false;
	}
	return false;
}
