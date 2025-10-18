#include "header.hpp"

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

bool	isServer(std::vector<std::string>& tknLine, std::vector<std::string>::iterator& it, File& file)
{
	std::string	line;

	it++;
	if (it != tknLine.end() && *it == "{")
		return true;
	if (it != tknLine.end())
		return false;
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
		return false;
	}
	return false;
}

bool	isLocation(std::vector<std::string>& tknLine, std::vector<std::string>::iterator& it, File& file, std::string& locationPath)
{

	std::string	line;

	it++;
	if (it == tknLine.end() || !isValidLocationPath(*it))
		return false;
	locationPath = *it;
	it++;
	if (it != tknLine.end() && *it == "{")
		return true;
	if (it != tknLine.end())
		return false;
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
		return false;
	}
	return false;
}

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
