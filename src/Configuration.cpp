#include "Configuration.hpp"

Configuration::Configuration()
{

}

Configuration::Configuration(const Configuration &copy) : _name(copy._name), _host(copy._host), _listen(copy._listen), _root(copy._root), _index(copy._index), _error_page(copy._error_page), _max_body_size(copy._max_body_size)
{

}

Configuration   &Configuration::operator=(const Configuration &copy)
{
    if (this != &copy)
    {
        this->_name = copy._name;
        this->_host = copy._host;
        this->_listen = copy._listen;
        this->_root = copy._root;
        this->_index = copy._index;
        this->_error_page = copy._error_page;
        this->_max_body_size = copy._max_body_size;
    }
    return (*this);
}

Configuration::~Configuration()
{

}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

std::string Configuration::getName() const
{
	return this->_name;
}

std::string Configuration::getHost() const
{
	return this->_host;
}

unsigned int Configuration::getListen() const
{
	return this->_listen;
}

std::string Configuration::getRoot() const
{
    return this->_root;
}
std::string Configuration::getIndex() const
{
    return this->_index;
}
std::string Configuration::getErrorPage() const
{
    return this->_error_page;
}
unsigned int Configuration::getMaxBodySize() const
{
    return this->_max_body_size;
}

void Configuration::setName(const std::string &name)
{
	this->_name = name;
}

void Configuration::setHost(const std::string &host)
{
	this->_host = host;
}

void Configuration::setListen(unsigned int listen)
{
	this->_listen = listen;
}

void Configuration::setRoot(const std::string &root)
{
	this->_root = root;
}

void Configuration::setIndex(const std::string &index)
{
	this->_index = index;
}

void Configuration::setErrorPage(const std::string &error_page)
{
	this->_error_page = error_page;
}

void Configuration::setMaxBodySize(unsigned int max_body_size)
{
	this->_max_body_size = max_body_size;
}
