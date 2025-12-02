#include "Configuration.hpp"
#include "ParsingException.hpp"
#include "header.hpp"

struct ReturnDirective;
/*************************************************************/
/*               CONFIGURATION BASE CLASS                    */
/*************************************************************/

/*============== Constructor and destructors ================*/

Configuration::Configuration()
: _methodsAllowed(std::vector<std::string>(1, "GET")),
_root(""),
_autoindex(false),
_index(""),
_max_body_size(0),
_store("")
{}

Configuration::Configuration(const Configuration &copy)
: _methodsAllowed(copy._methodsAllowed),
_root(copy._root),
_autoindex(copy._autoindex),
_index(copy._index),
_max_body_size(copy._max_body_size),
_store(copy._store)
{}

Configuration::~Configuration(){}


/*============== Assing overload operator ================*/
Configuration &Configuration::operator=(const Configuration &copy)
{
	if (this != &copy)
	{
		this->_methodsAllowed = copy._methodsAllowed;
		this->_root = copy._root;
		this->_autoindex = copy._autoindex;
		this->_index = copy._index;
		this->_max_body_size = copy._max_body_size;
		this->_store = copy._store;
	}
	return (*this);
}

/*============== setters ================*/

void Configuration::setMethods(const std::vector<std::string>& methods)
{
	std::vector<std::string>::const_iterator it;

	this->_methodsAllowed.clear();
	for (it = methods.begin(); it != methods.end(); it++)
	{
		if (std::find(this->_methodsAllowed.begin(), this->_methodsAllowed.end(), *it) == this->_methodsAllowed.end())
		{
			this->_methodsAllowed.push_back(*it);
			if (*it != "GET" && *it != "POST" && *it != "DELETE" && *it != "HEAD")
				LOG_WARNING("Unknown method [" + *it + "] allowed on server config");
		}
	}

	if (this->_methodsAllowed.empty())
		this->_methodsAllowed.push_back("GET");
}

void Configuration::setRoot(const std::vector<std::string>& root)
{
	if (root.empty())
		throw ERR_PARS("Directive [root] has no value");
	if (root.size() > 1)
		throw ERR_PARS("Directive [root] has more than one element");
	if (!pathIsDirectory(root.front()))
		LOG_WARNING_LINK("Directive [root] path is not a directory: [" + root.front() + "]");
	this->_root = root.front();
}

void Configuration::setIndex(const std::vector<std::string>& index)
{
	if (index.size() > 1)
	{
		LOG_WARNING("Directive [index] has not implemented yet multiple directives management, only first will be stored");
		//TODO: delete this warning after implementation
	}
	this->_index = index.front();
}

void Configuration::setStore(const std::vector<std::string>& store)
{
	if (store.size() > 1)
		throw ERR_PARS("Directive [store] has more than one element");
	this->_store = store.front();
}

/*************************************************************/
/*                    SERVERCFG CLASS                        */
/*************************************************************/

/*============== Constructor and destructors ================*/

ServerCfg::ServerCfg()
: Configuration(),
_name(""),
_host(""),
_port(0),
_errorPages(),
_body_size(0),
_location_map()
{}

ServerCfg::ServerCfg(const ServerCfg &copy)
: Configuration(copy),
_name(copy._name),
_host(copy._host),
_port(copy._port),
_errorPages(copy._errorPages),
_body_size(copy._body_size),
_location_map(copy._location_map)
{}

ServerCfg::~ServerCfg(){}


/*============== Assing overload operator ================*/

ServerCfg &ServerCfg::operator=(const ServerCfg &copy)
{
	if (this != &copy)
	{
		Configuration::operator=(copy);
		this->_name = copy._name;
		this->_host = copy._host;
		this->_port = copy._port;
		this->_errorPages = copy._errorPages;
		this->_body_size = copy._body_size;
		this->_location_map = copy._location_map;
	}
	return (*this);
}


/*=========================== Getters ===========================*/

Location const*	ServerCfg::getSpecificLocation(std::string const& location) const
{
	std::map<std::string, Location>::const_iterator it;
	it = this->_location_map.find(location);

	if (it != this->_location_map.end())
		return &it->second;

	return 	NULL;
}

Location const* ServerCfg::getBestMatchLocation(std::string const& location) const
{
	Location const* bestMatch = NULL;
	size_t longestMatch = 0;

	std::map<std::string, Location>::const_iterator it;
	for (it = this->_location_map.begin(); it != this->_location_map.end(); ++it)
	{
		std::string const& locationPath = it->first;
		size_t locationLen = locationPath.size();

		if (location.compare(0, locationLen, locationPath) == 0)
		{
			bool isExactMatch = (location.size() == locationLen);
			bool hasSlashAfter = (location.size() > locationLen && location[locationLen] == '/');
			bool endsWithSlash = (locationPath[locationLen - 1] == '/');

			if (isExactMatch || hasSlashAfter || endsWithSlash)
			{
				if (locationLen > longestMatch)
				{
					longestMatch = locationLen;
					bestMatch = &(it->second);
				}
			}
		}
	}

	return bestMatch;
}


/*============================ Setters ===========================*/

void ServerCfg::setName(const std::vector<std::string>& name)
{
	if (name.size() > 1)
	{
		LOG_WARNING("Directive [name] has not implemented yet multiple directives management, only first will be stored");
		//TODO: delete this warning after implementation
	}
	this->_name = name.front();
}

void ServerCfg::setErrorPage(std::vector<std::string>& error_page)
{
	if (error_page.size() % 2 != 0)
		throw ERR_PARS("Directive [error_page] need a pair number of elements");

	std::vector<std::string>::iterator it;
	for (it = error_page.begin(); it != error_page.end(); it++)
	{
		std::stringstream	ss(*it);
		int					code;
		ss >> code;
		if (ss.fail() || !ss.eof())
			throw ERR_PARS("Error code [" + *it + "] not recognizable");
		if (code < 100 || code > 599)
			throw ERR_PARS("Error code [" + *it + "] out of range (100-599)");

		it++;
		std::string	page = *it;
		if (page.empty())
			throw ERR_PARS("missing error page path");
		if (page.find("..") != std::string::npos)
			throw ERR_PARS("Error page path contains '..' which is not allowed");

		this->_errorPages[code] = page;
	}
}

/*============== Member functions ================*/

bool	ServerCfg::minValidCfg(void) const
{
	if (this->_host.empty())
	{
		std::cerr << ORANGE << "WARNING!: " << RESET
				<< "host not set for server "
				<< "[" << this->_name << "]" << std::endl;
		return false;
	}
	if (!this->_port)
	{
		std::cerr << ORANGE << "WARNING!: " << RESET
				<< "port not set for server "
				<< "[" << this->_name << "]" << std::endl;
		return false;
	}
	if (this->_root.empty())
	{
		std::cerr << ORANGE << "WARNING!: " << RESET
				<< "root not set for server"
				<< "[" << this->_name << "]" << std::endl;
		return false;
	}
	return true;
}

/*************************************************************/
/*                     LOCATION CLASS                        */
/*************************************************************/

/*============== Constructor and destructors ================*/

Location::Location() : Configuration(), _path(""), _cgiPass(""), _return(), _validCgiExt(), _cgiMap() {}

Location::Location(const Location &copy)
: Configuration(copy),
_path(copy._path),
_cgiPass(copy._cgiPass),
_return(copy._return),
_validCgiExt(copy._validCgiExt),
_cgiMap(copy._cgiMap)
{}

Location::~Location(){}

/*============== Assing overload operator ================*/

Location &Location::operator=(const Location &copy)
{
	if (this != &copy)
	{
		Configuration::operator=(copy);
		this->_path = copy._path;
		this->_cgiPass = copy._cgiPass;
		this->_return = copy._return;
		this->_validCgiExt = copy._validCgiExt;
		this->_cgiMap = copy._cgiMap;
	}
	return (*this);
}

/*============== setters and getters ================*/

void	Location::setCgiPass(std::vector<std::string> const& value)
{
	 if (value.empty())
		throw ERR_PARS("Location directive [cgi_pass] has no value");
	if (value.size() > 1)
		throw ERR_PARS("Location directive [cgi_pass] has too many values");
	if (!pathIsExecutable(value.front()))
		throw ERR_PARS("Location directive [cgi_pass] path is not executable: [" + value.front() + "]");
	this->_cgiPass = value.front();
}

void Location::setReturn(std::vector<std::string>& return_val)
{
	size_t size = return_val.size();

	if (size == 1)
	{
		std::string value = return_val.front();
		if (isUrl(value))
		{
			this->_return.code = 302;
			this->_return.value = value;
			this->_return.isSet = true;
			return;
		}

		int code = parseReturnCode(return_val.front());
		this->_return.code = code;
		this->_return.value = "";
		this->_return.isSet = true;
		return;
	}

	int code = parseReturnCode(return_val.front());
	this->_return.code = code;

	if (return_val[1].empty())
		throw ERR_PARS("Directive [return] code [" + return_val[0] + "] has empty value");

	if (code >= 300 && code < 400 && !isUrl(return_val[1]))
	throw ERR_PARS("Location directive [return] has code: [" + return_val.front() + "] with no URL");

	std::vector<std::string>::iterator it;
	for (it = return_val.begin() + 1; it != return_val.end(); it++)
	{
		this->_return.value.append(*it);
		if (it + 1 != return_val.end())
			this->_return.value.append(" ");
	}
	this->_return.isSet = true;
}
void	Location::setCgiExt(std::vector<std::string>& cgiExt)
{
	std::vector<std::string>::const_iterator it;

	for (it = cgiExt.begin(); it != cgiExt.end(); it++)
	{
		if (std::find(this->_validCgiExt.begin(), this->_validCgiExt.end(), *it) == this->_validCgiExt.end())
			this->_validCgiExt.push_back(*it);
	}
}

void	Location::addCgiExt(std::string const& cgiExt)
{
	if (std::find(this->_validCgiExt.begin(), this->_validCgiExt.end(), cgiExt) == this->_validCgiExt.end())
		this->_validCgiExt.push_back(cgiExt);
}

void	Location::addCgiPair(std::vector<std::string>& cgiMapPair)
{
	if (cgiMapPair.size() % 2 != 0)
		throw ERR_PARS("Directive [cgi_map] need a pair number of elements");

	std::vector<std::string>::iterator it;
	for (it = cgiMapPair.begin(); it != cgiMapPair.end(); it++)
	{
		std::string	ext = *it;
		if (ext[0] != '.')
			throw ERR_PARS("Bad extension in cgi Location: [" + *it + "]");
		it++;
		std::string	pathToBin = *it;
		if (pathToBin.empty())
			throw ERR_PARS("missing error page path");
		if (!pathIsExecutable(pathToBin))
			throw ERR_PARS("Path to binary file [" + pathToBin + "], to execute [" + ext + "] extensions, is not an executable file" );

		this->_cgiMap[ext] = pathToBin;
		this->addCgiExt(ext);
	}
}

void	Location::setCgiMap(std::map<std::string, std::string>& cgiMap)
{
	this->_cgiMap.clear();
	this->_cgiMap.insert(cgiMap.begin(), cgiMap.end());
}



/*============== Member private functions ================*/

int	Location::parseReturnCode(std::string const& strCode)
{
	std::stringstream ss(strCode);
	int	code;
	ss >> code;

	if (ss.fail() || !ss.eof())
		throw ERR_PARS("Location directive [return] has invalid code: [" + strCode + "]");
	if (code < 100 || code > 599)
		throw ERR_PARS("Location directive [return] code: [" + strCode + "] is out of bounds");

	return code;
}

/*============== Member public functions ================*/

bool	Location::minValidLocation(void) const
{
	bool hasRoot = !this->_root.empty();
	bool hasCGI = !this->_cgiPass.empty();
	bool hasReturn = this->_return.isSet;

	if (!hasRoot && !hasCGI && !hasReturn)
	{
		LOG_WARNING_LINK("Invalid Location: incomplete directives or unknown location type not suported");
		return false;
	}

	if (hasRoot)
	{
		if (!pathIsDirectory(this->_root))
		{
			LOG_WARNING_LINK("Location: root [" + this->_root + "] is not a valid directory. It may be created later");
		}
	}

	if (hasCGI)
	{
		if (!pathIsExecutable(this->_cgiPass))
		{
			LOG_WARNING_LINK("Invalid Location: cgipass [" + this->_cgiPass + "] is not a valid executable");
			return false;
		}
	}

	return true;
}

