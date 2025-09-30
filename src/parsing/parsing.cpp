#include "header.hpp"
#include <fstream>

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
		typeMap["listen"] = LISTEN;
		typeMap["root"] = ROOT;
		typeMap["index"] = INDEX;
		typeMap["error_page"] = ERROR_PAGE;
		typeMap["client_max_body_size"] = MAX_BODY_SIZE;
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

bool parse(std::map<std::string, Configuration> &buffer, char *path)
{
	Configuration	temp;
	std::ifstream   file;
	std::string		line;

	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Error could'nt open file" << std::endl;
		buffer.clear();
		return (false);
	}
	
	while (std::getline(file, line))
	{
		size_t pos = line.find_first_not_of(" \t");
		if (pos == std::string::npos || line[pos] == '#')
			continue;
		if (line.find("server") != std::string::npos)
		{
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
			temp = Configuration();
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
						temp.setHost(value);
						break;
					case(LISTEN):
						temp.setListen(static_cast<unsigned int>(atol(value.c_str())));
						break;
					case(ROOT):
						temp.setRoot(value);
						break;
					case(INDEX):
						temp.setIndex(value);
						break;
					case(ERROR_PAGE):
						temp.setErrorPage(value);
						break;
					case(MAX_BODY_SIZE):
						temp.setMaxBodySize(parseSize(value));
						break;
					case(UNKNOWN):
						std::cout << "Unknown directive: " << line << std::endl;
						break;
					default:
						break;
				}
			}
			std::string serverKey = temp.getName();
			buffer.insert(std::pair<std::string, Configuration>(serverKey, temp));
		}
	}
	
	file.close();
	return (true);
}