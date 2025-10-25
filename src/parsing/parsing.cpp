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

<<<<<<< HEAD
bool parse(std::vector<Server> &buffer, char *path)
=======
void parse(std::map<std::string, ServerCfg> &buffer, char *path)
>>>>>>> main
{
	File				file;
	std::string			line;
	std::vector<ServerCfg>	serverList;
	ServerCfg serverInstance;

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
<<<<<<< HEAD
			bool found_brace = false;
			if (line.find('{') != std::string::npos)
				found_brace = true;
			else
			{
				while (std::getline(file, line))
				{
					if (line.find('{') != std::string::npos)
					{
						found_brace = true;
						break;
					}
					pos = line.find_first_not_of(" \t");
					if (pos != std::string::npos && line[pos] != '#')
					{
						std::cout << "Expected '{' after server, found: " << line << std::endl;
						break;
					}
				}
			}
			if (!found_brace)
			{
				std::cout << "Error: No opening brace found for server block" << std::endl;
				continue;
			}
			temp = Server();
			while (std::getline(file, line))
			{
				pos = line.find_first_not_of(" \t");
				if (pos == std::string::npos || line[pos] == '#')
					continue;
				if (line.find('}') != std::string::npos)
					break;
				std::string value;
				if (pos != std::string::npos)
				{
					size_t space_pos = line.find_first_of(" \t", pos);
					if (space_pos != std::string::npos)
					{
						size_t value_start = line.find_first_not_of(" \t", space_pos);
						if (value_start != std::string::npos)
							value = line.substr(value_start);
					}
				}	
				switch (find_type(line))
				{
					case(SERVER_NAME):
						temp.setName(value);
						break;
					case(HOST):
						{
							size_t colonPos = value.find("::");
							if (colonPos != std::string::npos)
							{
								std::string host_part = value.substr(0, colonPos);
								std::string port_part = value.substr(colonPos + 2);
								temp.setHost(host_part);
								temp.setPort(static_cast<unsigned int>(atol(port_part.c_str())));
							}
							else
								temp.setHost(value);// might want to change
						}
						break;
					case(ERROR_PAGE)://to chabge once i have the map
						{
							size_t space_pos = value.find_first_of(" \t");
							if (space_pos != std::string::npos)
							{
								size_t path_start = value.find_first_not_of(" \t", space_pos);
								if (path_start != std::string::npos)
								{
									std::string error_path = value.substr(path_start);
									temp.setErrorPage(error_path);
								}
							}
							else
							{
								temp.setErrorPage(value);
							}
						}
						break;
					case(BODY_SIZE):
						temp.setBodySize(parseSize(value));
						break;
					
					// Configuration case
					case(METHODS):
						temp.setMethods(value);
						break;
					case(RETURN):
						temp.setReturn(value);
						break;
					case(ROOT):
						temp.setRoot(value);
						break;
					case(AUTOINDEX):
						temp.setAutoindex(value == "on" || value == "true" || value == "1");
						break;
					case(INDEX):
						temp.setIndex(value);
						break;
					case(MAX_BODY_SIZE):
						temp.setMaxBodySize(parseSize(value));
						break;
					case(STORE):
						temp.setStore(value);
						break;
					
					// Location case
					case(LOCATION):
						if (parseLocation(file, temp, line) == -1)
						{
							std::cout << "Error parsing location block" << std::endl;
							continue;
						}
						break;
					case(UNKNOWN):
						std::cout << "Unknown directive: " << line << std::endl;
						break;
					default:
						break;
				}
			}
			buffer.push_back(temp);
=======
			serverInstance = parseServer(file);//fills creates an instance of server and fills it with all the directives and locations
			setDefaults(serverInstance);
			if (!serverInstance.minValidCfg())//minimun settings check
				throw ERR_PARS("Abort");
			buffer.insert(std::pair<std::string, ServerCfg>(serverInstance.getName(), serverInstance));//add server to the pool of servers, restart the loop
>>>>>>> main
		}
	}
	/* TODO: check for duplicated servers and/or ip ,using map logic ? need documentation to check if it is needed */
	file.file.close();
}
