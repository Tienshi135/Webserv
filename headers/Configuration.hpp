#pragma once

#include <iostream>
#include <string>

typedef enum ConfigType
{
	SERVER_NAME,
	HOST,
	LISTEN,
	ROOT,
	INDEX,
	ERROR_PAGE,
	MAX_BODY_SIZE,
	UNKNOWN
}	e_configtype;

class Configuration
{
	private:
		std::string		_name;
		std::string		_host;
		unsigned int	_listen;
		std::string		_root;
		std::string		_index;
		std::string		_error_page;
		unsigned int	_max_body_size;
	public:
	Configuration();
	Configuration(const Configuration &copy);
	Configuration &operator=(const Configuration &copy);
	~Configuration();

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	std::string		getName() const;
	std::string		getHost() const;
	unsigned int	getListen() const;
	std::string		getRoot() const;
	std::string		getIndex() const;
	std::string		getErrorPage() const;
	unsigned int	getMaxBodySize() const;

	void			setName(const std::string &name);
	void			setHost(const std::string &host);
	void			setListen(unsigned int listen);
	void			setRoot(const std::string &root);
	void			setIndex(const std::string &index);
	void			setErrorPage(const std::string &error_page);
	void			setMaxBodySize(unsigned int max_body_size);
};