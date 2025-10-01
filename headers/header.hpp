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

// System includes
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>

#include "Configuration.hpp"

bool    parse(std::map<std::string, Configuration> &buffer, char *path);
int     socket_init(std::map<std::string, Configuration>::iterator current);