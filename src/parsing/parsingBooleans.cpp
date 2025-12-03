#include "header.hpp"

/**
 * @brief Checks if two string vectors share any common element
 *
 * @param v1 First vector to compare
 * @param v2 Second vector to compare
 * @return true if at least one element exists in both vectors, false otherwise
 */
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

/**
 * @brief Checks if a path exists in the filesystem
 *
 * @param path Path to check
 * @return true if path exists, false otherwise
 */
bool pathExists(const std::string &path)
{
	struct stat info;
	return (stat(path.c_str(), &info) == 0);
}

/**
 * @brief Validates if path is a readable directory
 *
 * Checks that path:
 *   - Exists
 *   - Is a directory (not a file)
 *   - Has read permissions
 *
 * @param path Directory path to validate
 * @return true if path is a readable directory, false otherwise
 */
bool	pathIsDirectory(std::string const& path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	if (!S_ISDIR(st.st_mode))
		return false;
	if (access(path.c_str(), R_OK) != 0)
		return false;
	return true;
}

/**
 * @brief Validates if path is a readable regular file
 *
 * Checks that path:
 *   - Exists
 *   - Is a regular file (not a directory or special file)
 *   - Has read permissions
 *
 * @param path File path to validate
 * @return true if path is a readable regular file, false otherwise
 */
bool	pathIsRegFile(std::string const& path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	if (!S_ISREG(st.st_mode))
		return false;
	if (access(path.c_str(), R_OK) != 0)
		return false;
	return true;
}

/**
 * @brief Validates if path is an executable
 *
 * Checks that path:
 *   - Exists
 *   - Is not a directory
 *   - Has execute permissions
 *
 * @param path Directory path to validate
 * @return true if path is an executable, false otherwise
 */
bool	pathIsExecutable(std::string const& path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	if (!S_ISREG(st.st_mode))
		return false;
	if (access(path.c_str(), X_OK) != 0)
		return false;
	return true;
}

/**
 * @brief Validates location path format for configuration (URI)
 *
 * Valid paths must:
 *   - Start with '/'
 *   - Not contain whitespace characters (space, tab, newline)
 *
 * @param path Location path to validate
 * @return true if path format is valid, false otherwise
 */
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

/**
 * @brief Detects and validates server block opening in configuration file
 *
 * Valid formats:
 *   - "server {" on same line
 *   - "server" on one line, "{" on next line (with max 1 empty line between)
 *
 * @param tknLine Tokenized current line
 * @param file File object that contains the ifstream file and line counter
 * (modified: advances line counter if reading ahead)
 * @return true if valid server block opening found, false otherwise
 * @throws ParsingException if server block syntax is invalid
 */
bool	foundServer(std::vector<std::string>& tknLine, File& file)
{
	std::string	line;

	std::vector<std::string>::iterator	it;
	it = std::find(tknLine.begin(), tknLine.end(), "server");
	if (it != tknLine.end())
	{
		if (tknLine.front() == "server" && tknLine.back() == "{" && tknLine.size() == 2)
			return true;

		int	linesChecked = 0;
		while (std::getline(file.file, line) && linesChecked <= 1)
		{
			file.nLines++;
			tknLine = tokenizeLine(line, file.nLines);
			if (tknLine.empty())
			{
				linesChecked++;
				continue;
			}
			if (tknLine.front() == "{")
				return true;
			throw ERR_PARS_CFGLN("Wrong server block opening syntax", file.nLines);
		}
	}
	return false;
}

/**
 * @brief Detects and validates location block opening in configuration file
 *
 * Valid formats:
 *   - "location /path {" on same line (3 tokens)
 *   - "location /path" on one line, "{" on next line (with max 1 empty line between)
 *
 * @param tknLine Tokenized current line
 * @param file File object that contains the ifstream file and line counter
 * (modified: advances line counter if reading ahead)
 * @return true if valid location block opening found, false otherwise
 * @throws ParsingException if location block syntax is invalid
 */
bool	foundLocation(std::vector<std::string>& tknLine, File& file)
{

	std::string	line;

	std::vector<std::string>::iterator	it;
	it = std::find(tknLine.begin(), tknLine.end(), "location");
	if (it != tknLine.end())
	{
		if (tknLine.front() == "location" && tknLine.back() == "{" && tknLine.size() == 3)
			return true;
		int	linesChecked = 0;
		while (std::getline(file.file, line) && linesChecked <= 1)
		{
			file.nLines++;
			tknLine = tokenizeLine(line, file.nLines);
			if (tknLine.empty())
			{
				linesChecked++;
				continue;
			}
			if (tknLine.front() == "{")
				return true;
			throw ERR_PARS_CFGLN("Wrong location block opening syntax", file.nLines);
		}
	}
	return false;
}

/**
 * @brief Parses and validates "host:port" string format
 *
 * Validates:
 *   - Format is "host:port" (colon-separated)
 *   - Port is numeric and in range 1-65535
 *   - Host is valid IPv4 address (4 octets, each 0-255)
 *
 * @param value String to parse (e.g., "127.0.0.1:8080")
 * @param host Output parameter for validated host/IP
 * @param port Output parameter for validated port number
 * @return true if format is valid and values are set, false otherwise
 */
bool parseHostPort(const std::string& value, std::string& host, unsigned int& port)
{
	if (value.empty())
		return false;

	size_t colonPos = value.find_last_of(':');
	if (colonPos == std::string::npos)//we can implement default port here. Now with no port set aborts launching
		return false;

	//validate host and store in variable
	std::string strPort = value.substr(colonPos + 1);

	if (strPort.empty())
		return false;
	for (size_t i = 0; i < strPort.length(); i++)
	{
		if (!std::isdigit(strPort[i]))
			return false;
	}
	long portLong = std::atol(strPort.c_str());
	if (portLong < 1 || portLong > 65535)
		return false;
	port = static_cast<unsigned int>(portLong);

	//store and validate IP octets
	host = value.substr(0, colonPos);
	std::vector<int> octets = parseIPOctets(host);

	if (octets.size() != 4)
		return false;

	for (size_t i = 0; i < octets.size(); i++)
	{
		if (octets[i] < 0 || octets[i] > 255)
			return false;
	}

	return true;
}

/**
 * @brief Validates URL or relative path format for redirect directives
 *
 * Valid formats:
 *   - Relative path: /page, /old/page.html
 *   - Absolute URL: http://example.com, http://example.com/page
 *
 * @param url String to validate
 * @return true if string is a valid URL or path, false otherwise
 */
bool	isUrl(std::string const& url)
{
	if (url.empty())
		return false;

	if (url[0] == '/')
		return true;

	if (url.size() >= 7)
	{
		std::string http = strToLower(url.substr(0, 7));
		if (http == "http://")
			return true;
	}

	return false;
}
