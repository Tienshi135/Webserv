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
#include "Request.hpp"
#include "Response.hpp"

// Parsing includes
#include <fstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <sys/stat.h>


//parsing
bool    parse(std::map<std::string, Server> &buffer, char *path);
bool hasCommonElement(std::vector<std::string>& v1, std::vector<std::string>& v2);
bool pathExists(const std::string &path);
bool isValidLocationPath(const std::string &path);
std::vector<std::string>	tokenizeLine(std::string& line);
void	setLocationDirective(Server& server, e_configtype& directive, std::string& value, std::string& locationPath);
void	setDirective(Server& server, e_configtype& directive, std::string& value);
Server	configServer(std::ifstream& file);
bool	isServer(std::vector<std::string>& tknLine, std::vector<std::string>::iterator& it, std::ifstream& file);
bool	isLocation(std::vector<std::string>& tknLine, std::vector<std::string>::iterator& it, std::ifstream& file, std::string& locationPath);
int	parseLocation(Server& server, std::vector<std::string>& locationLine, std::ifstream &file);
e_configtype	findType(std::string directive);
unsigned int parseSize(const std::string &value);
std::vector<int> parseIPOctets(const std::string& ip);
bool parseHostPort(const std::string& value, std::string& host, unsigned int& port);

int     socket_init(std::map<std::string, Server>::iterator current);
