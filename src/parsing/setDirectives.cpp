#include "header.hpp"

void	setDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value)
{
	switch (directive)
	{
		case(SERVER_NAME):
			server.setName(value);
			break;
		case(HOST):
			{
				if (value.size() > 1)
					throw ERR_PARS("Directive [host] has more than one element");
				std::string		host;
				unsigned int	port;

				if (!parseHostPort(value.front(), host, port))
					throw ERR_PARS("Bad IP:PORT format");
				else
				{
					server.setHost(host);
					server.setPort(port);
				}
			}
			break;
		case(ERROR_PAGE):
			server.setErrorPage(value);
			break;
		case(BODY_SIZE):
			if (value.size() > 1)
				throw ERR_PARS("Directive [body_size] has more than one element");
			server.setBodySize(parseSize(value.front()));
			break;

		// Configuration case
		case(METHODS):
			server.setMethods(value);
			break;
		case(RETURN):
			server.setReturn(value);
			break;
		case(ROOT):
			server.setRoot(value);
			break;
		case(AUTOINDEX):
			if (value.size() > 1)
				throw ERR_PARS("Directive [autoindex] has more than one element");
			server.setAutoindex(value.front() == "on" || value.front() == "true" || value.front() == "1");
			break;
		case(INDEX):
			server.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			if (value.size() > 1)
				throw ERR_PARS("Directive [client_max_body_size] has more than one element");
			server.setMaxBodySize(parseSize(value.front()));
			break;
		case(STORE):
			server.setStore(value);
			break;
		default:
			break;
	}
}


void	setLocationDirective(ServerCfg& server, e_configtype& directive, std::vector<std::string>& value, std::string& locationPath)
{
	std::map<std::string, Location> currentMap = server.getLocationMap();

	Location& tempLocation = currentMap[locationPath];
	switch (directive)
	{
		case(METHODS):
			tempLocation.setMethods(value);
			break;
		case(RETURN):
			tempLocation.setReturn(value);
			break;
		case(ROOT):
			tempLocation.setRoot(value);
			break;
		case(AUTOINDEX):
			if (value.size() > 1)
				throw ERR_PARS("Directive [autoindex] has more than one element");
			tempLocation.setAutoindex(value.front() == "on" || value.front() == "true" || value.front() == "1");
			break;
		case(INDEX):
			tempLocation.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			if (value.size() > 1)
				throw ERR_PARS("Directive [client_max_body_size] has more than one element");
			tempLocation.setMaxBodySize(parseSize(value.front()));
			break;
		case(STORE):
			tempLocation.setStore(value);
			break;
		case(ERROR_PAGE):
			throw ERR_PARS("Directive [error_page] has not been implemented in location!");/*TODO: add setter for error page to base configuration since location can also store it*/
			// tempLocation.setErrorPage(value);
			break;
		default:
			throw ERR_PARS("Directive not allowed in location block");
			break;
	}
	server.setLocationMap(currentMap);
}
