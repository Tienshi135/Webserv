<<<<<<< HEAD:src/Server.cpp
#include "Server.hpp"
=======
#include "Configuration.hpp"
#include "ParsingException.hpp"
>>>>>>> main:src/Configuration.cpp

Configuration::Configuration() : _methods(""), _return(""), _root(""), _autoindex(false), _index(""), _max_body_size(0), _store("")
{
}

Configuration::Configuration(const Configuration &copy) : _methods(copy._methods), _return(copy._return), _root(copy._root), _autoindex(copy._autoindex), _index(copy._index), _max_body_size(copy._max_body_size), _store(copy._store)
{
}

Configuration &Configuration::operator=(const Configuration &copy)
{
    if (this != &copy)
    {
        this->_methods = copy._methods;
        this->_return = copy._return;
        this->_root = copy._root;
        this->_autoindex = copy._autoindex;
        this->_index = copy._index;
        this->_max_body_size = copy._max_body_size;
        this->_store = copy._store;
    }
    return (*this);
}

Configuration::~Configuration()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Configuration::getMethods() const
{
	return (this->_methods);
}
std::string Configuration::getReturn() const
{
	return (this->_return);
}

std::string Configuration::getRoot() const
{
    return (this->_root);
}

bool Configuration::getAutoindex() const
{
	return (this->_autoindex);
}

std::string Configuration::getIndex() const
{
    return (this->_index);
}

unsigned int Configuration::getMaxBodySize() const
{
    return (this->_max_body_size);
}

std::string Configuration::getStore() const
{
	return (this->_store);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*TODO: methods should be a vector of strings*/
void Configuration::setMethods(const std::vector<std::string>& methods)
{
	for (size_t i = 0; i < methods.size(); i++)
	{
		this->_methods.append(methods[i]);
		if (i + 1 < methods.size())
			this->_methods.append("");
	}
}
/* TODO: return should be a map up to 1 or two code URL, that stores the path as string value and the code as key*/
void Configuration::setReturn(std::vector<std::string>& return_val)
{
	std::cerr << YELLOW << "WARNING! " << RESET
		<< "Directive [return] implementation is unfinished, check < TODO: > comments"
		<< std::endl;//TODO: delete this warning after implementation

	std::vector<std::string>::iterator it;
	for (it = return_val.begin(); it != return_val.end(); it++)
	{
		this->_return.append(*it);
		if ((it + 1) != return_val.end())
			this->_return.append(" ");
	}
}

void Configuration::setRoot(const std::vector<std::string>& root)
{
	if (root.size() > 1)
		throw ERR_PARS("Directive [root] has more than one element");
	this->_root = root.front();
}

void Configuration::setRoot(std::string const& root)
{
	this->_root = root;
}

void Configuration::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

void Configuration::setIndex(const std::vector<std::string>& index)
{
	if (index.size() > 1)
	{
		std::cerr << YELLOW << "WARNING! " << RESET
				<< "Directive [index] has not implemented yet multiple directives management, only first will be stored"
				<< std::endl;//TODO: delete this warning after implementation
	}
	this->_index = index.front();
}

void Configuration::setIndex(std::string const& index)
{
	this->_index = index;
}

void Configuration::setMaxBodySize(unsigned int max_body_size)
{
	this->_max_body_size = max_body_size;
}

void Configuration::setStore(const std::vector<std::string>& store)
{
	if (store.size() > 1)
		throw ERR_PARS("Directive [store] has more than one element");
	this->_store = store.front();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

ServerCfg::ServerCfg() : Configuration(), _name(""), _host(""), _port(0), _body_size(0), _location_map()
{
}

ServerCfg::ServerCfg(const ServerCfg &copy) : Configuration(copy), _name(copy._name), _host(copy._host), _port(copy._port), _errorPages(copy._errorPages), _body_size(copy._body_size), _location_map(copy._location_map)
{
}

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

ServerCfg::~ServerCfg()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string ServerCfg::getName() const
{
	return (this->_name);
}

std::string ServerCfg::getHost() const
{
	return (this->_host);
}

unsigned int ServerCfg::getPort() const
{
	return (this->_port);
}

std::map<int, std::string> const& ServerCfg::getErrorPages() const
{
	return (this->_errorPages);
}

unsigned int ServerCfg::getBodySize() const
{
	return (this->_body_size);
}

std::map<std::string, Location> ServerCfg::getLocationMap() const
{
	return (this->_location_map);
}

std::map<std::string, Location> ServerCfg::getLocationMap()
{
	return (this->_location_map);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void ServerCfg::setName(const std::vector<std::string>& name)
{
	if (name.size() > 1)
	{
		std::cerr << YELLOW << "WARNING! " << RESET
				<< "Directive [name] has not implemented yet multiple directives management, only first will be stored"
				<< std::endl;//TODO: delete this warning after implementation
	}
	this->_name = name.front();
}

void ServerCfg::setName(std::string const& name)
{
	this->_name = name;
}

void ServerCfg::setHost(const std::string &host)
{
	this->_host = host;
}

void ServerCfg::setPort(unsigned int listen)
{
	this->_port = listen;
}
/* TODO: error page should be a map with all the error codes as keys and it's correspondent pages paths as string values*/
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

void ServerCfg::setBodySize(unsigned int body_size)
{
	this->_body_size = body_size;
}

void ServerCfg::setLocationMap(const std::map<std::string, Location> &location_map)
{
	this->_location_map = location_map;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Location::Location() : Configuration()
{
}

Location::Location(const Location &copy) : Configuration(copy)
{
}

Location &Location::operator=(const Location &copy)
{
	if (this != &copy)
	{
		Configuration::operator=(copy);
	}
	return (*this);
}

Location::~Location()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


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

