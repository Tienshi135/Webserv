#include "header.hpp"
#include <fstream>
#include <algorithm>


std::vector<std::string>	tokenizeLine(std::string& line)
{
	std::string					token;
	std::vector<std::string>	tokenized;
	std::stringstream			ss(line);
	while (ss >> token)
		tokenized.push_back(token);
	return tokenized;
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
