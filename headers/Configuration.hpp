#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

typedef enum ConfigType
{
	SERVER_NAME,
	HOST,
	ERROR_PAGE,
	BODY_SIZE,

	METHODS,
	RETURN,
	ROOT,
	AUTOINDEX,
	INDEX,
	MAX_BODY_SIZE,
	STORE,

	LOCATION,

	UNKNOWN
}	e_configtype;

class Configuration
{
	private:
		std::string		_methods;
		std::string		_return;
		std::string		_root;
		bool			_autoindex;
		std::string		_index;
		unsigned int	_max_body_size;
		std::string		_store;

	public:
		Configuration();
		Configuration(const Configuration &copy);
		Configuration &operator=(const Configuration &copy);
		~Configuration();

		std::string		getMethods() const;
		std::string		getReturn() const;
		std::string		getRoot() const;
		bool			getAutoindex() const;
		std::string		getIndex() const;
		unsigned int	getMaxBodySize() const;
		std::string		getStore() const;

		void			setMethods(const std::vector<std::string>& methods);
		void			setReturn(std::vector<std::string>& return_val);
		void			setRoot(const std::vector<std::string>& root);
		void			setAutoindex(bool autoindex);
		void			setIndex(const std::vector<std::string>& index);
		void			setMaxBodySize(unsigned int max_body_size);
		void			setStore(const std::vector<std::string>& store);
};

class Location : public Configuration
{
	public:
		Location();
		Location(const Location &copy);
		Location &operator=(const Location &copy);
		~Location();
};

class Server : public Configuration
{
	private:
		std::string						_name;
		std::string						_host;
		unsigned int					_port;
		std::string						_error_page;
		unsigned int					_body_size;
		std::map<std::string, Location>	_location_map;

	public:
		Server();
		Server(const Server &copy);
		Server &operator=(const Server &copy);
		~Server();

		std::string						getName() const;
		std::string						getHost() const;
		unsigned int					getPort() const;
		std::string						getErrorPage() const;
		unsigned int					getBodySize() const;
		std::map<std::string, Location>	getLocationMap() const;

		void			setName(const std::vector<std::string>& name);
		void			setHost(const std::string &host);
		void			setPort(unsigned int listen);
		void			setErrorPage(std::vector<std::string>& error_page);
		void			setBodySize(unsigned int body_size);
		void			setLocationMap(const std::map<std::string, Location> &location_map);
};
