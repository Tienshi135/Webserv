#include "Configuration.hpp"
#include "ParsingException.hpp"

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

void Configuration::setMethods(const std::vector<std::string>& methods)
{
	if (methods.size() > 1)//TODO: check real limit for methods
		throw ERR_PARS("Directive [methods] has more than one element");
	this->_methods = methods.front();
}
/* TODO: return should be a map up to 1 or two code URL, that stores the path as string value and the code as key*/
void Configuration::setReturn(std::vector<std::string>& return_val)
{
	std::cerr << YELLOW << "WARNING! " << RESET
		<< "Directive [return_val] implementation is unfinished, check < TODO: > comments"
		<< std::endl;//TODO: delete this warning after implementation

	std::vector<std::string>::iterator it;
	for (it = return_val.begin(); it != return_val.end(); it++)
	{
		this->_return.append(*it);
		if (it != return_val.end())
			this->_return.append(" ");
	}
}

void Configuration::setRoot(const std::vector<std::string>& root)
{
	if (root.size() > 1)
		throw ERR_PARS("Directive [root] has more than one element");
	this->_root = root.front();
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
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Server::Server() : Configuration(), _name(""), _host(""), _port(0), _error_page(""), _body_size(0), _location_map()
{
}

Server::Server(const Server &copy) : Configuration(copy), _name(copy._name), _host(copy._host), _port(copy._port), _error_page(copy._error_page), _body_size(copy._body_size), _location_map(copy._location_map)
{
}

Server &Server::operator=(const Server &copy)
{
	if (this != &copy)
	{
		Configuration::operator=(copy);
		this->_name = copy._name;
		this->_host = copy._host;
		this->_port = copy._port;
		this->_error_page = copy._error_page;
		this->_body_size = copy._body_size;
		this->_location_map = copy._location_map;
	}
	return (*this);
}

Server::~Server()
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Server::getName() const
{
	return (this->_name);
}

std::string Server::getHost() const
{
	return (this->_host);
}

unsigned int Server::getPort() const
{
	return (this->_port);
}

std::string Server::getErrorPage() const
{
	return (this->_error_page);
}

unsigned int Server::getBodySize() const
{
	return (this->_body_size);
}

std::map<std::string, Location> Server::getLocationMap() const
{
	return (this->_location_map);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Server::setName(const std::vector<std::string>& name)
{
	if (name.size() > 1)
	{
		std::cerr << YELLOW << "WARNING! " << RESET
				<< "Directive [name] has not implemented yet multiple directives management, only first will be stored"
				<< std::endl;//TODO: delete this warning after implementation
	}
	this->_name = name.front();
}

void Server::setHost(const std::string &host)
{
	this->_host = host;
}

void Server::setPort(unsigned int listen)
{
	this->_port = listen;
}
/* TODO: error page should be a map with all the error codes as keys and it's correspondent pages paths as string values*/
void Server::setErrorPage(std::vector<std::string>& error_page)
{
	std::cerr << YELLOW << "WARNING! " << RESET
			<< "Directive [error_page] implementation is unfinished, check < TODO: > comments"
			<< std::endl;//TODO: delete this warning after implementation

	std::vector<std::string>::iterator it;
	for (it = error_page.begin(); it != error_page.end(); it++)
	{
		this->_error_page.append(*it);
		if (it != error_page.end())
			this->_error_page.append(" ");
	}
}

void Server::setBodySize(unsigned int body_size)
{
	this->_body_size = body_size;
}

void Server::setLocationMap(const std::map<std::string, Location> &location_map)
{
	this->_location_map = location_map;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
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
