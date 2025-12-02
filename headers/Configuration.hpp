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
	CGIMAP,

	UNKNOWN
}	e_configtype;

struct ReturnDirective
{
	int			code;	// HTTP status code (0 if not set)
	std::string	value;	// URL or text
	bool		isSet;	// Whether return directive was configured

	ReturnDirective() : code(0), value(""), isSet(false) {}
};

class Configuration
{
	protected:
		std::vector<std::string>	_methodsAllowed;
		std::string					_root;
		bool						_autoindex;
		std::string					_index; //not used, keep for future code scalation
		unsigned int				_max_body_size;
		std::string					_store;

	public:
		Configuration();
		Configuration(const Configuration &copy);
		Configuration &operator=(const Configuration &copy);
		virtual ~Configuration();

		//getters
		std::vector<std::string>	getMethods(void) const { return (this->_methodsAllowed); };
		std::string					getRoot(void) const { return (this->_root); };
		std::string					getIndex(void) const { return (this->_index); };
		std::string					getStore(void) const { return (this->_store); };
		unsigned int				getMaxBodySize(void) const { return (this->_max_body_size); };
		bool						getAutoindex(void) const { return (this->_autoindex); };

		//setters simple
		void	setRoot(std::string const& root) { this->_root = root; };
		void	setAutoindex(bool autoindex) { this->_autoindex = autoindex; };
		void	setIndex(std::string const& index) { this->_index = index; };
		void	setMaxBodySize(unsigned int max_body_size) { this->_max_body_size = max_body_size; };

		//setters complex
		void	setMethods(const std::vector<std::string>& methods);
		void	setRoot(const std::vector<std::string>& root);
		void	setIndex(const std::vector<std::string>& index);
		void	setStore(const std::vector<std::string>& store);
};

class Location : public Configuration
{
	private:


		std::string							_path;
		std::string							_cgiPass;
		ReturnDirective						_return;
		std::vector<std::string>			_validCgiExt;
		std::map<std::string, std::string>  _cgiMap;


		int	parseReturnCode(std::string const& strCode);

	public:
		Location();
		Location(const Location &copy);
		Location &operator=(const Location &copy);
		virtual ~Location();

		//setters simple
		void		setLocationPath(std::string const& path) { this->_path = path; };

		//setters complex
		void		setCgiPass(std::vector<std::string> const& value);
		void		setReturn(std::vector<std::string>& return_val);
		void		setCgiExt(std::vector<std::string>& cgiExt);
		void		addCgiExt(std::string const& cgiExt);
		void		setCgiMap(std::map<std::string, std::string>& cgiMap);
		void		addCgiPair(std::vector<std::string>& cgiMapPair);


		//getters
		std::string getLocationPath() const { return this->_path; };
		ReturnDirective	getReturn() const { return this->_return; };

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

		//getters simple
		unsigned int						getPort() const { return (this->_port); };
		unsigned int						getBodySize() const { return (this->_body_size); };
		std::string							getHost() const { return (this->_host); };
		std::string							getName() const { return (this->_name); };
		std::map<int, std::string>const&	getErrorPages() const { return (this->_errorPages); };
		std::map<std::string, Location>		getLocationMap() const { return (this->_location_map); };
		std::map<std::string, Location>		getLocationMap() { return (this->_location_map); };
		std::map<std::string, Location>&	getLocationMapRef() { return (this->_location_map); };

		//getters complex
		Location const*	getSpecificLocation(std::string const& location) const;
		Location const*	getBestMatchLocation(std::string const& location) const;

		//setters simple
		void	setName(std::string const& name) { this->_name = name; };
		void	setHost(const std::string &host) { this->_host = host; };
		void	setPort(unsigned int listen) { this->_port = listen; };
		void	setBodySize(unsigned int body_size) { this->_body_size = body_size; };

		//setters complex
		void	setName(const std::vector<std::string>& name);
		void	setErrorPage(std::vector<std::string>& error_page);
		void	setLocationMap(const std::map<std::string, Location> &location_map) { this->_location_map = location_map; };

		//member functions
		bool	minValidCfg() const;
};
