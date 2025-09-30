#pragma once

#include <cstdlib>
#include <map>
#include "Configuration.hpp"

bool parse(std::map<std::string, Configuration> &buffer, char *path);