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
std::vector<std::string>	tokenizeLine(std::string& line);
bool	parse(std::map<std::string, Server> &buffer, char *path);
bool	isServer(std::vector<std::string>::iterator& it, std::ifstream& file);

int		socket_init(std::map<std::string, Server>::iterator current);
