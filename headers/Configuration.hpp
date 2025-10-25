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
	CGIPASS,

	UNKNOWN
}	e_configtype;

class Configuration
{
	protected:
		std::string		_methods;
		std::string		_root;
		bool			_autoindex;
		std::string		_index;
		unsigned int	_max_body_size;
		std::string		_store;

	public:
		Configuration();
		Configuration(const Configuration &copy);
		Configuration &operator=(const Configuration &copy);
		virtual ~Configuration();

		std::string		getMethods() const;
		std::string		getRoot() const;
		bool			getAutoindex() const;
		std::string		getIndex() const;
		unsigned int	getMaxBodySize() const;
		std::string		getStore() const;

		void			setMethods(const std::vector<std::string>& methods);
		void			setRoot(const std::vector<std::string>& root);
		void			setRoot(std::string const& root);
		void			setAutoindex(bool autoindex);
		void			setIndex(const std::vector<std::string>& index);
		void			setIndex(std::string const& index);
		void			setMaxBodySize(unsigned int max_body_size);
		void			setStore(const std::vector<std::string>& store);
};

class Location : public Configuration
{
	private:
		struct ReturnDirective
	{
		int			code;	// HTTP status code (0 if not set)
		std::string	value;	// URL or text
		bool		isSet;	// Whether return directive was configured

		ReturnDirective() : code(0), value(""), isSet(false) {}
	};

		ReturnDirective	_return;
		std::string		_locationPath;
		std::string		_cgiPass;

		int	parseReturnCode(std::string const& strCode);

	public:
		Location();
		Location(const Location &copy);
		Location &operator=(const Location &copy);
		virtual ~Location();

		void	setLocationPath(std::string const& locationPath);
		void	setCgiPass(std::vector<std::string> const& value);
		void	setReturn(std::vector<std::string>& return_val);

		ReturnDirective	getReturn() const;

		bool	minValidLocation() const;
	};

class ServerCfg : public Configuration
{
	private:
		std::string						_name;
		std::string						_host;
		unsigned int					_port;
		std::map<int, std::string>		_errorPages;
		unsigned int					_body_size;
		std::map<std::string, Location>	_location_map;

	public:
		ServerCfg();
		ServerCfg(const ServerCfg &copy);
		ServerCfg &operator=(const ServerCfg &copy);
		virtual ~ServerCfg();

		std::string							getName() const;
		std::string							getHost() const;
		unsigned int						getPort() const;
		std::map<int, std::string>const&	getErrorPages() const;
		unsigned int						getBodySize() const;
		std::map<std::string, Location>		getLocationMap() const;
		std::map<std::string, Location>		getLocationMap();

		void			setName(const std::vector<std::string>& name);
		void			setName(std::string const& name);
		void			setHost(const std::string &host);
		void			setPort(unsigned int listen);
		void			setErrorPage(std::vector<std::string>& error_page);
		void			setBodySize(unsigned int body_size);
		void			setLocationMap(const std::map<std::string, Location> &location_map);

		bool			minValidCfg() const;
};
