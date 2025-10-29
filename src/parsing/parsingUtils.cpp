#include "header.hpp"

/**
 * @brief Removes leading and trailing quotes from a string
 *
 * @param quoted String to trim (modified in place)
 */
void	trimQuotes(std::string& quoted)
{
	if (quoted.empty())
		return;
	if (quoted[0] == '\"' || quoted[0] == '\'')
		quoted.erase(0, 1);

	if (quoted.empty())
		return;

	if (quoted[quoted.size() -1] == '\"' || quoted[quoted.size() -1] ==  '\'')
		quoted.erase(quoted.size() - 1, 1);
}

/**
 * @brief Parses HTTP header parameters (semicolon-separated key=value pairs)
 *
 * Handles common HTTP header formats like:
 *   - Content-Disposition: form-data; name="file"; filename="photo.jpg"
 *   - Content-Type: multipart/form-data; boundary=----WebKitBoundary
 *
 * @param strElements String containing parameters (without header name)
 *                    Example: "form-data; name=\"file\"; filename=\"photo.jpg\""
 *
 * @return Map of key-value pairs
 *         - Quoted values have quotes removed
 *         - Trailing semicolons are removed
 *         - Keys without values have empty string as value
 *
 * @note Input should NOT include the header name (e.g., "Content-Disposition:")
 */
std::map<std::string, std::string>	parseHeaderParameters(std::string& strElements)
{
	std::map<std::string, std::string>	elementsMap;
	std::vector<std::string> elementsVec = tokenizeLine(strElements);

	std::vector<std::string>::iterator it;
	for (it = elementsVec.begin(); it != elementsVec.end(); it++)
	{
		std::string key;
		std::string value;
		size_t assingPos = it->find("=");
		if (assingPos != std::string::npos)
		{
			key = it->substr(0, assingPos);
			value = it->substr(assingPos + 1);
			trimQuotes(value);
			if (!value.empty() && value[value.size() - 1] == ';')
				value.erase(value.size() -1, 1);
		}
		else
		{
			key = *it;
			if (!key.empty() && key[key.size() - 1] == ';')
				key.erase(key.size() -1, 1);
			value = "";
		}
		elementsMap.insert(std::make_pair(key, value));
	}
	return elementsMap;
}

/**
 * @brief Converts an integer to string
 *
 * @param num Integer to convert
 * @return String representation of the number
 */
std::string numToString(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

/**
 * @brief Converts a size_t to string (overload)
 *
 * @param num Size value to convert
 * @return String representation of the number
 */
std::string numToString(size_t num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

/**
 * @brief Converts a string to lowercase
 *
 * @param string Input string to convert
 * @return New string with all characters in lowercase
 */
std::string	strToLower(std::string const& string)
{
	std::string	lowered;
	for (size_t i = 0; i < string.size(); i++)
		lowered[i] = std::tolower(string[i]);

	return lowered;
}

/**
 * @brief Parses an IPv4 address into its four octets
 *
 * Validates format and returns empty vector on any error:
 *   - Leading/trailing dots
 *   - Empty segments
 *   - Non-digit characters
 *   - Leading zeros (e.g., "192.01.0.1")
 *
 * @param ip IPv4 address as string (e.g., "192.168.1.1")
 * @return Vector of 4 integers (0-255), or empty vector if invalid
 */
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

/**
 * @brief Returns list of valid size unit suffixes
 *
 * @return Vector containing: "K", "KB", "M", "MB", "G", "GB"
 */
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

/**
 * @brief Checks if a unit string is in the valid unit list
 *
 * @param unitList Vector of valid units to check against
 * @param unit Unit string to validate
 * @return true if unit is found in the list, false otherwise
 */
bool	isValidUnit(std::vector<std::string>& unitList, std::string const& unit)
{
	std::vector<std::string>::iterator it;
	for (it = unitList.begin(); it != unitList.end(); it++)
		if (!it->compare(unit))
			return true;
	return false;
}

/**
 * @brief Parses size strings with optional unit suffixes (K/KB, M/MB, G/GB)
 *
 * Validates and converts sizes like:
 *   - "1024" → 1024 bytes
 *   - "1K" or "1KB" → 1024 bytes
 *   - "5M" or "5MB" → 5242880 bytes
 *   - "1G" or "1GB" → 1073741824 bytes (maximum allowed)
 *
 * @param value Size string to parse (case-insensitive for units)
 * @return Size in bytes as unsigned int
 * @throws ERR_PARS if format is invalid or size exceeds 1GB limit
 */
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

/**
 * @brief Maps directive names to their corresponding configuration type enum
 *
 * Uses static map for efficiency (initialized once).
 * Supported directives:
 *   - Server: server_name, host, error_page, body_size
 *   - Location: methods/allow_methods, return/redirect, root, autoindex,
 *               index, client_max_body_size/max_body_size, store, location
 *
 * @param directive Directive name as string
 * @return e_configtype enum value, or UNKNOWN if not found
 */
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

/**
 * @brief Tokenizes configuration file lines into directive components
 *
 * Handles:
 *   - Whitespace-delimited tokens
 *   - Quoted strings (preserves spaces within quotes)
 *   - Comments (text after '#' is ignored)
 *   - Semicolon removal from last token (required syntax)
 *   - Special tokens: '{', '}'
 *
 * @param line Configuration line to parse (modified to remove '\r')
 * @param nLine Line number for error reporting
 * @return Vector of tokens (empty if line is empty/comment)
 * @throws ParsingException for unclosed quotes or missing semicolons
 */
std::vector<std::string>	tokenizeLine(std::string& line, size_t nLine)
{
	std::string					token;
	std::vector<std::string>	tokenized;
	std::stringstream			ss(line);

	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

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

/**
 * @brief Tokenizes HTTP request lines (simplified version without strict validation)
 *
 * Similar to config tokenizer but:
 *   - No semicolon requirements
 *   - No comment handling
 *   - Returns empty vector on parse errors instead of throwing
 *   - Handles quoted strings with whitespace
 *
 * @param line Request line to parse (modified to remove '\r')
 * @return Vector of tokens (empty on error or empty line)
 */
std::vector<std::string>	tokenizeLine(std::string& line)
{
	std::string					token;
	std::vector<std::string>	tokenized;

	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	std::stringstream			ss(line);


	while (ss >> std::ws && ss.peek() != EOF)
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
			tokenized.push_back(token);
		}
	}
	return tokenized;
}

/**
 * @brief Sets default values for server configuration if not explicitly defined
 *
 * Applied defaults:
 *   - server_name: "default_server" (or "default_server_N" for multiple servers)
 *   - index: "index.html"
 *   - client_max_body_size: 1MB
 *   - location root: inherits from server root if not set
 *
 * Prints informational messages for each default applied.
 *
 * @param server ServerCfg object to apply defaults to (modified in place)
 */
void	setDefaults(ServerCfg& server)
{
	static int nbServers;

	if (server.getName().empty())
	{
		if (nbServers > 0)
			server.setName("default_server_" + numToString(nbServers));
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
