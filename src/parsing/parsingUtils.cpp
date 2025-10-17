#include "header.hpp"

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
	return (UNKNOWN);
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
				std::runtime_error("Error: unclosed quotes in config file");//TODO: better erro msg? keep track of getline number of lines?
			tokenized.push_back(token);
		}
		else
		{
			ss >> token;
			tokenized.push_back(token);
		}
	}
	if (tokenized.front().empty())
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
			throw std::runtime_error("Error: directive must end with semicolon");
		}
	}
	return tokenized;
}


