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

e_configtype	find_type(std::string line)
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

	std::string directive;
	size_t pos = line.find_first_not_of(" \t");//can change to all whitespace
	if (pos != std::string::npos)
	{
		size_t end = line.find_first_of(" \t:", pos);//can change
		if (end != std::string::npos)
			directive = line.substr(pos, end - pos);
		else
			directive = line.substr(pos);
	}
	std::map<std::string, e_configtype>::iterator it = typeMap.find(directive);
	if (it != typeMap.end())
		return (it->second);

	return (UNKNOWN);
}

int	parseLocation(std::ifstream &file, Server &buff, const std::string &currentLine)
{
	std::string line;
	std::string locationPath;
	Location tempLocation;
	bool found_brace = false;

	size_t pos = currentLine.find("location");
	if (pos != std::string::npos)
	{
		size_t path_start = currentLine.find_first_not_of(" \t", pos + 8);
		if (path_start != std::string::npos)
		{
			size_t path_end = currentLine.find_first_of(" \t{", path_start);
			if (path_end != std::string::npos)
				locationPath = currentLine.substr(path_start, path_end - path_start);
			else
				locationPath = currentLine.substr(path_start);
		}
	}

	if (currentLine.find('{') != std::string::npos)
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
				std::cout << "Expected '{' after location, found: " << line << std::endl;
				return (-1);
			}
		}
	}

	if (!found_brace)
	{
		std::cout << "Error: No opening brace found for location block" << std::endl;
		return (-1);
	}

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
				std::cout << "Unknown directive in location: " << line << std::endl;
				break;
			default:
				std::cout << "Directive not allowed in location block: " << line << std::endl;
				break;
		}
	}

	std::map<std::string, Location> currentMap = buff.getLocationMap();
	currentMap[locationPath] = tempLocation;
	buff.setLocationMap(currentMap);

	std::cout << "Parsed location: " << locationPath << std::endl;
	return (0);

}

bool parse(std::map<std::string, Server> &buffer, char *path)
{
	Server			temp;
	std::ifstream	file;
	std::string		line;

	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Error could'nt open file" << std::endl;
		buffer.clear();
		return (false);
	}

	std::vector<std::string>			tkLine;
	std::vector<std::string>::iterator	it;

	while (std::getline(file, line))
	{
		// size_t pos = line.find_first_not_of(" \t");
		// if (pos == std::string::npos || line[pos] == '#')
		// 	continue;
		// if (line.find("server") != std::string::npos)
		// {

		tkLine = tokenizeLine(line);

		if (line.find("server{"))
		{
			it = std::find(tkLine.begin(), tkLine.end(), "server{");
			if (!getServerParameters(it, file))
				return false;
			continue;
		}

		it = std::find(tkLine.begin(), tkLine.end(), "server");
		if (tkLine.empty() || (!tkLine.empty() && tkLine.front()[0] == '#'))
			continue;
		if (it != tkLine.end())
		{
			if (!isServer(it, file))
			{
				std::cerr << "Error: No opening brace found for server block" << std::endl;
				break;
			}
			// bool found_brace = false;
			// if (line.find('{') != std::string::npos)
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
			// 			std::cout << "Expected '{' after server, found: " << line << std::endl;
			// 			break;
			// 		}
			// 	}
			// }
			// if (!found_brace)
			// {
			// 	std::cout << "Error: No opening brace found for server block" << std::endl;
			// 	continue;
			// }
			if (!getConfigItemParams(it, file))
				return false;
			//BREAK THE PREVIOUS LOOP HERE?
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
					case(ERROR_PAGE):
						temp.setErrorPage(value);
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
			buffer.insert(std::pair<std::string, Server>(temp.getName(), temp));
		}
	}

	file.close();
	return (true);
}
