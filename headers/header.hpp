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
#include <fcntl.h>

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

bool    parse(std::vector<Server> &buffer, char *path);
int     socket_init(std::vector<Server>::iterator current);