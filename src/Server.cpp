#include "Server.hpp"

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

void Configuration::setMethods(const std::string &methods)
{
	this->_methods = methods;
}

void Configuration::setReturn(const std::string &return_val)
{
	this->_return = return_val;
}

void Configuration::setRoot(const std::string &root)
{
	this->_root = root;
}

void Configuration::setAutoindex(bool autoindex)
{
	this->_autoindex = autoindex;
}

void Configuration::setIndex(const std::string &index)
{
	this->_index = index;
}

void Configuration::setMaxBodySize(unsigned int max_body_size)
{
	this->_max_body_size = max_body_size;
}

void Configuration::setStore(const std::string &store)
{
	this->_store = store;
}

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

void Server::setName(const std::string &name)
{
	this->_name = name;
}

void Server::setHost(const std::string &host)
{
	this->_host = host;
}

void Server::setPort(unsigned int listen)
{
	this->_port = listen;
}

void Server::setErrorPage(const std::string &error_page)
{
	this->_error_page = error_page;
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
