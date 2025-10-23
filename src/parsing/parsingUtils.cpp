#include "header.hpp"

std::string intToString(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::vector<int> parseIPOctets(const std::string& ip)
{
	std::vector<int> octets;
	std::stringstream ss(ip);
	std::string segment;

	if (!ip.empty() && (ip[0] == '.' || ip[ip.length() - 1] == '.'))
		return octets;

	while (std::getline(ss, segment, '.'))
	{
		if (segment.empty())
		{
			octets.clear();
			return octets;
		}
		for (size_t i = 0; i < segment.size(); i++)
		{
			if (!std::isdigit(segment[i]))
			{
				octets.clear();
				return octets;
			}
		}
		if (segment.length() > 1 && segment[0] == '0')
		{
			octets.clear();
			return octets;
		}
		int octet = std::atoi(segment.c_str());
		octets.push_back(octet);
	}
	if (!ss.eof())
	{
		octets.clear();
		return octets;
	}
	return octets;
}

std::vector<std::string>	validUnitList(void)
{
	std::vector<std::string> units;
	units.push_back("K");
	units.push_back("KB");
	units.push_back("M");
	units.push_back("MB");
	units.push_back("G");
	units.push_back("GB");

	return units;
}
bool	isValidUnit(std::vector<std::string>& unitList, std::string const& unit)
{
	std::vector<std::string>::iterator it;
	for (it = unitList.begin(); it != unitList.end(); it++)
		if (!it->compare(unit))
			return true;
	return false;
}
unsigned int parseSize(const std::string &value)
{
	if (value.empty())
		return 0;

	std::vector<std::string> validUnits = validUnitList();
	std::string numStr;
	std::string suffix;

	for (size_t i = 0; i < value.length(); ++i)
	{
		if (std::isdigit(value[i]) || value[i] == '.')
			numStr += value[i];
		else
		{
			suffix = value.substr(i);
			break;
		}
	}

	if (numStr.empty())
		throw ERR_PARS("Invalid size format: no number found");

	if (suffix.empty())
	{
		unsigned long long int size = atoll(numStr.c_str());
		if (size > (1024 * 1024 * 1024) || size > std::numeric_limits<unsigned int>::max())
			throw ERR_PARS("size is too big. Max size allowed is 1G");
		return static_cast<unsigned int>(size);
	}
	for (size_t i = 0; i < suffix.length(); ++i)
		suffix[i] = std::toupper(suffix[i]);
	if (!isValidUnit(validUnits, suffix))
		throw ERR_PARS("Invalid size format: unknown unit identifier");

	unsigned long long int size = atoll(numStr.c_str());


	switch (suffix[0])
	{
		case 'K':
			if (size > (1024 * 1024))
				throw ERR_PARS("Size is too big. Max size allowed is 1G");
			size *= 1024;
			break;
		case 'M':
			if (size > 1024)
				throw ERR_PARS("Size is too big. Max size allowed is 1G");
			size *= (1024 * 1024);
			break;
		case 'G':
			if (size > 1)
				throw ERR_PARS("Size is too big. Max size allowed is 1G");
			size *= (1024 * 1024 * 1024);
			break;
		default:
			break;
	}
	if (size > std::numeric_limits<unsigned int>::max())
		throw ERR_PARS("size is too big. Max size allowed is 1G");
	return static_cast<unsigned int>(size);
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
	return (UNKNOWN);
}

std::vector<std::string>	tokenizeLine(std::string& line, size_t nLine)
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
				throw ERR_PARS_CFGLN("Unclosed quotes in config file", nLine);//TODO: this does not work properly, review this logic
			tokenized.push_back(token);
		}
		else
		{
			ss >> token;
			if (token[0] == '#')// comment mark, stop reading the line
				break;
			tokenized.push_back(token);
		}
	}
	if (tokenized.empty() || tokenized.front().empty())
		return tokenized;
	if (!tokenized.empty() && tokenized.back() != "{" && tokenized.back() != "}" && tokenized.front() != "#")
	{
		if (tokenized.back()[tokenized.back().length() - 1] == ';')
		{
			tokenized.back().erase(tokenized.back().length() - 1);// Remove semicolon from last token
			if (tokenized.back().empty())
				tokenized.pop_back();
		}
		else
		{
			throw ERR_PARS_CFGLN("Directive must end with semicolon", nLine);
		}
	}
	return tokenized;
}

std::vector<std::string>	tokenizeLine(std::string& line)
{
	std::string					token;
	std::vector<std::string>	tokenized;
	std::stringstream			ss(line);

	while (ss >> std::ws)
	{
		char quote = ss.peek();
		if (quote == '"' || quote == '\'')
		{
			ss.get();
			if (!getline(ss, token, quote))
			{
				tokenized.clear();
				return tokenized;
			}
			tokenized.push_back(token);
		}
		else
		{
			ss >> token;
			if (token[0] == '#')// comment mark, stop reading the line
				break;
			tokenized.push_back(token);
		}
	}
	return tokenized;
}

void	setDefaults(Server& server)
{
	static int nbServers;

	if (server.getName().empty())
	{
		if (nbServers > 0)
			server.setName("default_server_" + intToString(nbServers));
		else
			server.setName("default_server");

		std::cout << CYAN << "info: " << RESET
		<< "server " << "[" << "" << "]"
		<< " did not define [name], setting default value: "
		<< BOLD << server.getName() << RESET << std::endl;
	}

	if (server.getIndex().empty())
	{
		server.setIndex("index.html");

		std::cout << CYAN << "info: " << RESET
		<< "server " << "[" << server.getName() << "]"
		<< " did not define [index], setting default value: "
		<< BOLD << "index.html" << RESET << std::endl;
	}

	if (server.getMaxBodySize() == 0)
	{
		server.setMaxBodySize(1024 * 1024);

		std::cout << CYAN << "info: " << RESET
		<< "server " << "[" << server.getName() << "]"
		<< " did not define [client_max_body_size], setting default value: "
		<< BOLD << "1MB" << RESET << std::endl;
	}

	std::map<std::string, Location> tmpLocation = server.getLocationMap();
	if (!tmpLocation.empty())
	{
		std::map<std::string, Location>::iterator it;
		for (it = tmpLocation.begin(); it != tmpLocation.end(); it++)
		{
			if (it->second.getRoot().empty())
			{
				it->second.setRoot(server.getRoot());

				std::cout << CYAN << "info: " << RESET
				<< "location with path " << "[" << it->first << "]"
				<< " did not define [root], setting inherited value from server: "
				<< BOLD << server.getRoot() << RESET << std::endl;
			}
		}
		server.setLocationMap(tmpLocation);
	}

	nbServers += 1;
}
