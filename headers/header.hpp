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


/*parsing config*/
std::vector<std::string>			tokenizeLine(std::string& line);
Server			configServer(std::ifstream& file);
e_configtype	findType(std::string line);
bool	parse(std::map<std::string, Server> &buffer, char *path);
bool	isServer(std::vector<std::string>::iterator& it, std::ifstream& file);
bool	isLocation(std::vector<std::string>::iterator& it, std::ifstream& file, std::string& locationPath);
bool	hasCommonElement(std::vector<std::string>& v1, std::vector<std::string>& v2);
void	setDirective(Server& server, e_configtype& directive, std::string& value);
void	setLocationDirective(Server& server, e_configtype& directive, std::string& value, std::string& locationPath);
// int		parseLocation(std::ifstream &file, Server &buff, const std::string &currentLine);
int		parseLocation(Server& server, std::vector<std::string>& currentLinestd, std::ifstream &file);
unsigned int parseSize(const std::string &value);

int		socket_init(std::map<std::string, Server>::iterator current);
