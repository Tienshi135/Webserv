#include "header.hpp"
#include <fstream>
#include <algorithm>


unsigned int parseSize(const std::string &value)
{
	if (value.empty())
		return 0;

	std::string numStr;
	char suffix = '\0';

	for (size_t i = 0; i < value.length(); ++i)
	{
		if (std::isdigit(value[i]) || value[i] == '.')
			numStr += value[i];
		else
		{
			suffix = std::toupper(value[i]);
			break;
		}
	}

	if (numStr.empty())
		return 0;

	unsigned int num = static_cast<unsigned int>(atol(numStr.c_str()));

	switch (suffix)
	{
		case 'K':
			return num * 1024;
			break;
		case 'M':
			return num * 1024 * 1024;
			break;
		case 'G':
			return num * 1024 * 1024 * 1024;
			break;
		case '\0':
			return num;
			break;
		default:
			return num;
	}
}

e_configtype	findType(std::string directive)
{
	static std::map<std::string, e_configtype> typeMap;

	if (typeMap.empty())
	{
		typeMap["server_name"] = SERVER_NAME;
		typeMap["host"] = HOST;
		typeMap["error_page"] = ERROR_PAGE;
		typeMap["body_size"] = BODY_SIZE;

		typeMap["methods"] = METHODS;
		typeMap["allow_methods"] = METHODS;
		typeMap["return"] = RETURN;
		typeMap["redirect"] = RETURN;
		typeMap["root"] = ROOT;
		typeMap["autoindex"] = AUTOINDEX;
		typeMap["index"] = INDEX;
		typeMap["client_max_body_size"] = MAX_BODY_SIZE;
		typeMap["max_body_size"] = MAX_BODY_SIZE;
		typeMap["store"] = STORE;

		typeMap["location"] = LOCATION;
	}

	std::map<std::string, e_configtype>::iterator it = typeMap.find(directive);
	if (it != typeMap.end())
		return (it->second);

	// std::string directive;
	// size_t pos = line.find_first_not_of(" \t");//can change to all whitespace
	// if (pos != std::string::npos)
	// {
	// 	size_t end = line.find_first_of(" \t:", pos);//can change
	// 	if (end != std::string::npos)
	// 		directive = line.substr(pos, end - pos);
	// 	else
	// 		directive = line.substr(pos);
	// }
	// std::map<std::string, e_configtype>::iterator it = typeMap.find(directive);
	// if (it != typeMap.end())
	// 	return (it->second);

	return (UNKNOWN);
}

// int	parseLocation(std::ifstream &file, Server &buff, const std::string &currentLine)
int	parseLocation(Server& server, std::vector<std::string>& locationLine, std::ifstream &file)
{
	std::string line;
	std::string locationPath;
	// bool found_brace = false;

	std::vector<std::string>::iterator	it = locationLine.begin();
	if (!isLocation(it, file, locationPath))
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
		std::vector<std::string> tkLine = tokenizeLine(line);

		if (tkLine.empty() || (!tkLine.empty() && tkLine.front()[0] == '#'))
			continue;
		if (tkLine.front() == "}")
			break;
		if (hasCommonElement(tkLine, badToken))
		{
			std::cerr << "Error: location has no closing braces at: " << __FILE__ << ":" << __LINE__  << std::endl;
			return -1;
		}
		if (tkLine.size() > 2)
		{
			if (tkLine.front() == "return" && tkLine.size() == 3)
			{
				// Handle return directive with status code and URL
				// Store both tkLine[1] (status) and tkLine[2] (URL)
				std::string returnValue = tkLine[1] + " " + tkLine[2];
				server.setReturn(returnValue);
				continue;
			}
			else if (tkLine.front() == "error_page" && tkLine.size() == 3)
			{
				// error_page 404 /404.html
				server.setErrorPage(tkLine[2]);  // Store the path
				continue;
			}
			else
			{
				std::cerr << "Error: too many elements: [" << tkLine.size() << "] for directive at: " << __FILE__ << ":" << __LINE__  << std::endl;
				return -1;
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

		setLocationDirective(server, directiveType, value, locationPath);
	}
	if (line.find("}") == std::string::npos)
		throw std::runtime_error("Error: location has no closing braces");
	// size_t pos = currentLine.find("location");
	// if (pos != std::string::npos)
	// {
	// 	size_t path_start = currentLine.find_first_not_of(" \t", pos + 8);
	// 	if (path_start != std::string::npos)
	// 	{
	// 		size_t path_end = currentLine.find_first_of(" \t{", path_start);
	// 		if (path_end != std::string::npos)
	// 			locationPath = currentLine.substr(path_start, path_end - path_start);
	// 		else
	// 			locationPath = currentLine.substr(path_start);
	// 	}
	// }

	// if (currentLine.find('{') != std::string::npos)
	// 	found_brace = true;
	// else
	// {
	// 	while (std::getline(file, line))
	// 	{
	// 		if (line.find('{') != std::string::npos)
	// 		{
	// 			found_brace = true;
	// 			break;
	// 		}
	// 		pos = line.find_first_not_of(" \t");
	// 		if (pos != std::string::npos && line[pos] != '#')
	// 		{
	// 			std::cout << "Expected '{' after location, found: " << line << std::endl;
	// 			return (-1);
	// 		}
	// 	}
	// }

	// if (!found_brace)
	// {
	// 	std::cout << "Error: No opening brace found for location block" << std::endl;
	// 	return (-1);
	// }

	// while (std::getline(file, line))
	// {
	// 	pos = line.find_first_not_of(" \t");
	// 	if (pos == std::string::npos || line[pos] == '#')
	// 		continue;
	// 	if (line.find('}') != std::string::npos)
	// 		break;

	// 	std::string value;
	// 	if (pos != std::string::npos)
	// 	{
	// 		size_t space_pos = line.find_first_of(" \t", pos);
	// 		if (space_pos != std::string::npos)
	// 		{
	// 			size_t value_start = line.find_first_not_of(" \t", space_pos);
	// 			if (value_start != std::string::npos)
	// 				value = line.substr(value_start);
	// 		}
	// 	}

	// 	switch (findType(line))
	// 	{
	// 		case(METHODS):
	// 			tempLocation.setMethods(value);
	// 			break;
	// 		case(RETURN):
	// 			tempLocation.setReturn(value);
	// 			break;
	// 		case(ROOT):
	// 			tempLocation.setRoot(value);
	// 			break;
	// 		case(AUTOINDEX):
	// 			tempLocation.setAutoindex(value == "on" || value == "true" || value == "1");
	// 			break;
	// 		case(INDEX):
	// 			tempLocation.setIndex(value);
	// 			break;
	// 		case(MAX_BODY_SIZE):
	// 			tempLocation.setMaxBodySize(parseSize(value));
	// 			break;
	// 		case(STORE):
	// 			tempLocation.setStore(value);
	// 			break;
	// 		case(UNKNOWN):
	// 			std::cout << "Unknown directive in location: " << line << std::endl;
	// 			break;
	// 		default:
	// 			std::cout << "Directive not allowed in location block: " << line << std::endl;
	// 			break;
	// 	}
	// }

	// std::map<std::string, Location> currentMap = buff.getLocationMap();
	// currentMap[locationPath] = tempLocation;
	// buff.setLocationMap(currentMap);

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



	while (std::getline(file, line))
	{
		std::vector<std::string> tkLine = tokenizeLine(line);
		if (tkLine.empty() ||  tkLine.front().empty() || tkLine.front()[0] == '#')
			continue;

		std::vector<std::string>::iterator	it;
		it = std::find(tkLine.begin(), tkLine.end(), "server");
		if (it != tkLine.end())//server found, manage errors and fill it
		{
			if (!isServer(tkLine, it, file))
			{
				std::cerr << "Error: No opening brace found for server block" << std::endl;
				buffer.clear();
				return false;
			}
			try
			{
				serverInstance = configServer(file);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				buffer.clear();
				return false;
			}
			buffer.insert(std::pair<std::string, Server>(serverInstance.getName(), serverInstance));
		}
	}	//BREAK THE PREVIOUS LOOP HERE?
	file.close();
	return (true);
}




			/*==============================*/
			// serverInstance = Server();
			// while (std::getline(file, line))
			// {
			// 	pos = line.find_first_not_of(" \t");
			// 	if (pos == std::string::npos || line[pos] == '#')
			// 		continue;
			// 	if (line.find('}') != std::string::npos)
			// 		break;
			// 	std::string value;
			// 	if (pos != std::string::npos)
			// 	{
			// 		size_t space_pos = line.find_first_of(" \t", pos);
			// 		if (space_pos != std::string::npos)
			// 		{
			// 			size_t value_start = line.find_first_not_of(" \t", space_pos);
			// 			if (value_start != std::string::npos)
			// 				value = line.substr(value_start);
			// 		}
			// 	}
			// 	switch (findType(line))
			// 	{
			// 		case(SERVER_NAME):
			// 			serverInstance.setName(value);
			// 			break;
			// 		case(HOST):
			// 			{
			// 				size_t colonPos = value.find("::");
			// 				if (colonPos != std::string::npos)
			// 				{
			// 					std::string host_part = value.substr(0, colonPos);
			// 					std::string port_part = value.substr(colonPos + 2);
			// 					serverInstance.setHost(host_part);
			// 					serverInstance.setPort(static_cast<unsigned int>(atol(port_part.c_str())));
			// 				}
			// 				else
			// 					serverInstance.setHost(value);// might want to change
			// 			}
			// 			break;
			// 		case(ERROR_PAGE):
			// 			serverInstance.setErrorPage(value);
			// 			break;
			// 		case(BODY_SIZE):
			// 			serverInstance.setBodySize(parseSize(value));
			// 			break;

			// 		// Configuration case
			// 		case(METHODS):
			// 			serverInstance.setMethods(value);
			// 			break;
			// 		case(RETURN):
			// 			serverInstance.setReturn(value);
			// 			break;
			// 		case(ROOT):
			// 			serverInstance.setRoot(value);
			// 			break;
			// 		case(AUTOINDEX):
			// 			serverInstance.setAutoindex(value == "on" || value == "true" || value == "1");
			// 			break;
			// 		case(INDEX):
			// 			serverInstance.setIndex(value);
			// 			break;
			// 		case(MAX_BODY_SIZE):
			// 			serverInstance.setMaxBodySize(parseSize(value));
			// 			break;
			// 		case(STORE):
			// 			serverInstance.setStore(value);
			// 			break;

			// 		// Location case
			// 		case(LOCATION):
			// 			if (parseLocation(file, serverInstance, line) == -1)
			// 			{
			// 				std::cout << "Error parsing location block" << std::endl;
			// 				continue;
			// 			}
			// 			break;
			// 		case(UNKNOWN):
			// 			std::cout << "Unknown directive: " << line << std::endl;
			// 			break;
			// 		default:
			// 			break;
			// 	}
			// }
// 		}
// 	}


// }
