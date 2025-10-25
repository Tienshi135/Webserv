#pragma once

// Standard C++ includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sstream>
#include <stdexcept>

// System includes
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include "Configuration.hpp"

// Parsing includes
#include <fstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <sys/stat.h>
#include "ParsingException.hpp"
#include <limits>

// LOG defines
#define LOG_INFO(string) std::cout << BLUE << "Info: " << RESET << string << std::endl
#define LOG_INFO_LINK(string) std::cout << BLUE << "Info: " << RESET << BOLD << string << RESET << " at: " << __FILE__ << ":" << __LINE__ << std::endl

#define LOG_WARNING(string) std::cout << YELLOW << "Warning : " << RESET << string << std::endl
#define LOG_WARNING_LINK(string) std::cout << YELLOW << "Warning : " << RESET << BOLD << string << RESET << " at: " << __FILE__ << ":" << __LINE__ << std::endl

#define LOG_HIGH_WARNING(string) std::cout << ORANGE << "Warning: " << RESET << string << std::endl
#define LOG_HIGH_WARNING_LINK(string) std::cout << ORANGE << "Warning: " << RESET << BOLD << string << RESET << " at: " << __FILE__ << ":" << __LINE__ << std::endl

#define LOG_ERROR(string) std::cout << BLUE << "Error: " << RESET << string << std::endl
#define LOG_ERROR_LINK(string) std::cout << BLUE << "Error: " << RESET << BOLD << string << string << RESET << " at: " << __FILE__ << ":" << __LINE__ << std::endl

class Request;
class ServerCfg;
class Response;

struct File
{
	std::ifstream	file;
	size_t			nLines;
};

/*======= PARSING =====*/
//parsing
void				parse(std::vector<ServerCfg> &buffer, char *path);
void				parseLocation(ServerCfg& server, std::vector<std::string>& locationLine, File &file);
ServerCfg			parseServer(File& file);

//parsing utils
std::vector<std::string>	tokenizeLine(std::string& line, size_t nbLine);
std::vector<std::string>	tokenizeLine(std::string& line);
std::vector<int>			parseIPOctets(const std::string& ip);
std::string					numToString(int num);
std::string					numToString(size_t num);
e_configtype				findType(std::string directive);
unsigned int 				parseSize(const std::string &value);
void						setDefaults(ServerCfg& server);

//parsing booleans
bool	foundServer(std::vector<std::string>& tknLine, File& file);
bool	foundLocation(std::vector<std::string>& tknLine, File& file);
bool	pathExists(const std::string &path);
bool	isValidLocationPath(const std::string &path);
bool	hasCommonElement(std::vector<std::string>& v1, std::vector<std::string>& v2);
bool	parseHostPort(const std::string& value, std::string& host, unsigned int& port);

//set directives
void	setLocationDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value, std::string& locationPath);
void	setDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value);

/*=========== INIT ============*/
int     socket_init(std::vector<ServerCfg>::iterator current);
