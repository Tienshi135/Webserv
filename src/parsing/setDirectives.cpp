#include "header.hpp"

/*TODO: review this function */
void	setDirective(Server& server, e_configtype& directive, std::string& value)//review this function logic later to look for optimisation
{
	switch (directive)
	{
		case(SERVER_NAME):
			server.setName(value);
			break;
		case(HOST):
			{
				std::string		host;
				unsigned int	port;

				if (!parseHostPort(value, host, port))
					throw ERR_PARS("Bad IP:PORT format");
				else
				{
					server.setHost(host);
					server.setPort(port);
				}
			}
			break;
		case(ERROR_PAGE):
			server.setErrorPage(value);//TODO: error page should be a map with all the error codes and it's correspondent pages
			break;
		case(BODY_SIZE):
			server.setBodySize(parseSize(value));
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
			server.setAutoindex(value == "on" || value == "true" || value == "1");
			break;
		case(INDEX):
			server.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			server.setMaxBodySize(parseSize(value));
			break;
		case(STORE):
			server.setStore(value);
			break;
		case(UNKNOWN):
			std::cout << "Unknown directive: " << std::endl;//TODO: alredy checkd? should we check this here or before?
			break;
		default:
			break;
	}
}

/*TODO: review this function */
void	setLocationDirective(Server& server, e_configtype& directive, std::string& value, std::string& locationPath)
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
			tempLocation.setAutoindex(value == "on" || value == "true" || value == "1");
			break;
		case(INDEX):
			tempLocation.setIndex(value);
			break;
		case(MAX_BODY_SIZE):
			tempLocation.setMaxBodySize(parseSize(value));
			break;
		case(STORE):
			tempLocation.setStore(value);
			break;
			/*TODO: add setter for error page to base configuration since location can also store it*/
		// case(ERROR_PAGE):
		// 	tempLocation.setErrorPage(value);
		// 	break;
		case(UNKNOWN):
			std::cout << "Unknown directive in location: " << std::endl;
			break;
		default:
			std::cout << "Directive not allowed in location block: " << std::endl;
			break;
	}
	server.setLocationMap(currentMap);
}
