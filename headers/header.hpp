#pragma once

// Standard C++ includes
#include <iostream>
#include <iomanip>
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
#include <sys/time.h>
#include <dirent.h>
#include "Configuration.hpp"

// Parsing includes
#include <fstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include "ParsingException.hpp"
#include <limits>


#define ARRAY_SIZE(array)((int)(sizeof(array) / sizeof(array[0])))

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
class Client;

struct File
{
	std::ifstream	file;
	size_t			nLines;
};

enum e_errorcode
{
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,

    // 2xx Success
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,

    // 3xx Redirection
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    TEMPORARY_REDIRECT = 307,

    // 4xx Client Errors
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,

    // 5xx Server Errors
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

/*======= PARSING =====*/
//parsing
void				parse(std::vector<ServerCfg> &buffer, char *path);
void				parseLocation(ServerCfg& server, std::vector<std::string>& locationLine, File &file);
ServerCfg			parseServer(File& file);

//parsing utils
std::map<std::string, std::string>	parseHeaderParameters(std::string& strElements);
std::vector<std::string>	tokenizeLine(std::string& line, size_t nbLine);
std::vector<std::string>	tokenizeLine(std::string& line);
std::vector<std::string>	tokenizeHeaderParams(std::string& line);
std::vector<int>			parseIPOctets(const std::string& ip);
std::string					numToString(int num);
std::string					numToString(size_t num);
std::string					strToLower(std::string const& string);
e_configtype				findType(std::string directive);
unsigned int 				parseSize(const std::string &value);
ssize_t						getFileSize(const std::string& filepath);
ssize_t						getSafeSize(std::string const& sizeString);
void						setDefaults(ServerCfg& server);
void						trimQuotes(std::string& quoted);
std::string					urlDecode(std::string const& encoded);
std::string					urlEncode(const std::string& s);
std::string					htmlEscape(const std::string& s);

//parsing booleans
bool	foundServer(std::vector<std::string>& tknLine, File& file);
bool	foundLocation(std::vector<std::string>& tknLine, File& file);
bool	pathExists(const std::string &path);
bool	isValidLocationPath(const std::string &path);
bool	isUrl(std::string const& url);
bool	hasCommonElement(std::vector<std::string>& v1, std::vector<std::string>& v2);
bool	parseHostPort(const std::string& value, std::string& host, unsigned int& port);
bool	pathIsDirectory(std::string const& path);
bool	pathIsRegFile(std::string const& path);
bool	pathIsExecutable(std::string const& path);


//set directives
void	setLocationDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value, std::string& locationPath);
void	setDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value);

/*=========== INIT ============*/
int     socket_init(std::vector<ServerCfg>::iterator current);
