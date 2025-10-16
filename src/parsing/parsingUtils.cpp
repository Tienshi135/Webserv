#include "header.hpp"
#include <fstream>
#include <algorithm>
#include <map>


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
				std::runtime_error("Error: unclosed quotes in config file");//better erro msg? keep track of getline number of lines?
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


bool	getConfigItemParams(std::vector<std::string>::iterator& it, std::ifstream& file)
{
	std::vector<std::string>			tkLine;
	std::string							line;
	std::map<std::string, std::string>	serverParams;



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
