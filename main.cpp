#include "header.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseFactory.hpp"
#include "ResponseError.hpp"
#include "ResponseGet.hpp"


void printVector(const std::vector<ServerCfg> &buffer)
{
    std::cout << "=== Configuration Vector Contents ===" << std::endl;
    std::cout << "Total servers: " << buffer.size() << std::endl;
    std::cout << std::endl;

    if (buffer.empty())
    {
        std::cout << "Vector is empty!" << std::endl;
        return;
    }

    for (std::vector<ServerCfg>::const_iterator it = buffer.begin();
         it != buffer.end(); ++it)
    {
        std::cout << "=== Server: " << it->getName() << " ===" << std::endl;

        // Server-specific fields
        if (!it->getName().empty())
            std::cout << "  Server Name: " << it->getName() << std::endl;
        if (!it->getHost().empty())
            std::cout << "  Host/IP: " << it->getHost() << std::endl;
        if (it->getPort() != 0)
            std::cout << "  Port: " << it->getPort() << std::endl;
        if (!it->getErrorPages().empty())
		{
			std::cout << "  Error Pages: " << std::endl;
			std::map<int, std::string>::const_iterator it_err;
			std::map<int, std::string> const& errorPages = it->getErrorPages();
			for (it_err = errorPages.begin(); it_err != errorPages.end(); it_err++)
				std::cout << "    " << it_err->first << " " << it_err->second << std::endl;
		}
        if (it->getBodySize() != 0)
            std::cout << "  Body Size Limit: " << it->getBodySize() << std::endl;

        // Configuration fields (inherited) - only show non-empty
        if (!it->getMethods().empty())
            std::cout << "  Methods: " << it->getMethods() << std::endl;
        if (!it->getReturn().empty())
            std::cout << "  Return: " << it->getReturn() << std::endl;
        if (!it->getRoot().empty())
            std::cout << "  Root: " << it->getRoot() << std::endl;
        if (it->getAutoindex())
            std::cout << "  Autoindex: on" << std::endl;
        if (!it->getIndex().empty())
            std::cout << "  Index: " << it->getIndex() << std::endl;
        if (it->getMaxBodySize() != 0)
            std::cout << "  Max Body Size: " << it->getMaxBodySize() << std::endl;
        if (!it->getStore().empty())
            std::cout << "  Store: " << it->getStore() << std::endl;

        // Locations
        if (!it->getLocationMap().empty())
        {
            std::cout << "  Locations:" << std::endl;
            std::map<std::string, Location> const& locations = it->getLocationMap();
            std::map<std::string, Location>::const_iterator loc_it;
            for (loc_it = locations.begin(); loc_it != locations.end(); ++loc_it)
            {
                std::cout << "    Location: " << loc_it->first << std::endl;
                if (!loc_it->second.getMethods().empty())
                    std::cout << "      Methods: " << loc_it->second.getMethods() << std::endl;
                if (!loc_it->second.getReturn().empty())
                    std::cout << "      Return: " << loc_it->second.getReturn() << std::endl;
                if (!loc_it->second.getRoot().empty())
                    std::cout << "      Root: " << loc_it->second.getRoot() << std::endl;
                if (loc_it->second.getAutoindex())
                    std::cout << "      Autoindex: on" << std::endl;
                if (!loc_it->second.getIndex().empty())
                    std::cout << "      Index: " << loc_it->second.getIndex() << std::endl;
                if (loc_it->second.getMaxBodySize() != 0)
                    std::cout << "      Max Body Size: " << loc_it->second.getMaxBodySize() << std::endl;
                if (!loc_it->second.getStore().empty())
                    std::cout << "      Store: " << loc_it->second.getStore() << std::endl;
            }
        }

        std::cout << "================================" << std::endl;
        std::cout << std::endl;
    }
}

void printMap(const std::map<std::string, ServerCfg> &buffer)
{
    std::cout << "=== Configuration Map Contents ===" << std::endl;
    std::cout << "Total servers: " << buffer.size() << std::endl;
    std::cout << std::endl;

    if (buffer.empty())
    {
        std::cout << "Map is empty!" << std::endl;
        return;
    }

    for (std::map<std::string, ServerCfg>::const_iterator it = buffer.begin();
         it != buffer.end(); ++it)
    {
        std::cout << "=== Server: " << it->first << " ===" << std::endl;

        // Server-specific fields
        if (!it->second.getName().empty())
            std::cout << "  Server Name: " << it->second.getName() << std::endl;
        if (!it->second.getHost().empty())
            std::cout << "  Host/IP: " << it->second.getHost() << std::endl;
        if (it->second.getPort() != 0)
            std::cout << "  Port: " << it->second.getPort() << std::endl;
        if (!it->second.getErrorPages().empty())
		{
			std::cout << "  Error Pages: " << std::endl;
			std::map<int, std::string>::const_iterator it_err;
			std::map<int, std::string> const& errorPages = it->second.getErrorPages();
			for (it_err = errorPages.begin(); it_err != errorPages.end(); it_err++)
				std::cout << "    " << it_err->first << " " << it_err->second << std::endl;
		}
        if (it->second.getBodySize() != 0)
            std::cout << "  Body Size Limit: " << it->second.getBodySize() << std::endl;

        // Configuration fields (inherited) - only show non-empty
        if (!it->second.getMethods().empty())
            std::cout << "  Methods: " << it->second.getMethods() << std::endl;
        if (!it->second.getReturn().empty())
            std::cout << "  Return: " << it->second.getReturn() << std::endl;
        if (!it->second.getRoot().empty())
            std::cout << "  Root: " << it->second.getRoot() << std::endl;
        if (it->second.getAutoindex())
            std::cout << "  Autoindex: on" << std::endl;
        if (!it->second.getIndex().empty())
            std::cout << "  Index: " << it->second.getIndex() << std::endl;
        if (it->second.getMaxBodySize() != 0)
            std::cout << "  Max Body Size: " << it->second.getMaxBodySize() << std::endl;
        if (!it->second.getStore().empty())
            std::cout << "  Store: " << it->second.getStore() << std::endl;

        // Location map
        std::map<std::string, Location> locationMap = it->second.getLocationMap();
        if (!locationMap.empty())
        {
            std::cout << "  Locations (" << locationMap.size() << "):" << std::endl;
            for (std::map<std::string, Location>::const_iterator loc_it = locationMap.begin();
                 loc_it != locationMap.end(); ++loc_it)
            {
                std::cout << "    Location: " << loc_it->first << std::endl;
                if (!loc_it->second.getMethods().empty())
                    std::cout << "      Methods: " << loc_it->second.getMethods() << std::endl;
                if (!loc_it->second.getReturn().empty())
                    std::cout << "      Return: " << loc_it->second.getReturn() << std::endl;
                if (!loc_it->second.getRoot().empty())
                    std::cout << "      Root: " << loc_it->second.getRoot() << std::endl;
                if (loc_it->second.getAutoindex())
                    std::cout << "      Autoindex: on" << std::endl;
                if (!loc_it->second.getIndex().empty())
                    std::cout << "      Index: " << loc_it->second.getIndex() << std::endl;
                if (loc_it->second.getMaxBodySize() != 0)
                    std::cout << "      Max Body Size: " << loc_it->second.getMaxBodySize() << std::endl;
                if (!loc_it->second.getStore().empty())
                    std::cout << "      Store: " << loc_it->second.getStore() << std::endl;
            }
        }

        std::cout << "================================" << std::endl;
        std::cout << std::endl;
    }
}

int main(int argc, char **argv)
{
	std::map<int, ServerCfg>			sfd;
	std::map<int, std::vector<int> >	cfd;
	std::vector<ServerCfg>				buffer;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [*.conf]" << std::endl;
		return (-1);
	}

	//first part - parsing
	try
	{
		parse(buffer, argv[1]);
	}
	catch(const std::exception& e)
	{

		std::cerr << e.what() << '\n';
		buffer.clear();
		return (-1);
	}
	printVector(buffer);

	//second part - socket creation and binding
	std::vector<ServerCfg>::iterator it = buffer.begin();
	while (it != buffer.end())
	{
		int	socket_fd = socket_init(it);
		if (socket_fd != -1)
		{
			sfd.insert(std::pair<int, ServerCfg>(socket_fd, *it));
			cfd[socket_fd] = std::vector<int>();  // Initialize empty client vector for this server
			std::cout << "Server " << it->getName() << " listening successfully!" << std::endl;
		}
		it++;
	}
	if (sfd.empty())
	{
		std::cout << "No servers could be started! Exiting." << std::endl;
		return (-1);
	}

	//third part - main loop with select
	while (true)
	{
		fd_set	read_fd;
		int		max_fd = 0;

		FD_ZERO(&read_fd);
		std::map<int, ServerCfg>::iterator sfd_it = sfd.begin();
		while (sfd_it != sfd.end())
		{
			FD_SET(sfd_it->first, &read_fd);
			if (sfd_it->first > max_fd)
				max_fd = sfd_it->first;
			sfd_it++;
		}
		
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		
		int activity = select(max_fd + 1, &read_fd, NULL, NULL, &timeout);
		if (activity < 0)
		{
			std::cerr << "select() error: " << strerror(errno) << std::endl;
			break;
		}
		
		sfd_it = sfd.begin();
		while (sfd_it != sfd.end())
		{
			if (FD_ISSET(sfd_it->first, &read_fd))
			{
				int client_fd = accept(sfd_it->first, NULL, NULL);
				if (client_fd == -1)
				{
					perror("Accept error");
					continue;
				}
			
			char buffer[1024];
			std::cout << "Client connected, reading request..." << std::endl;
			ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);//add check for received bigger than buffer size
			if (bytes_read > 0)
			{
				buffer[bytes_read] = '\0';
				Request	req(buffer);
					
				const ServerCfg &server_config = sfd_it->second;
					
				Response* response = ResponseFactory::createResponse(server_config, req);
				if (response)
				{
					response->buildResponse();
					std::string response_str = response->getRawResponse();
					ssize_t bytes_sent = send(client_fd, response_str.c_str(), response_str.length(), 0);
					if (bytes_sent == -1)
						perror("Send error");
					delete response;
				}
					
				// std::string error_response = "HTTP/1.0 500 Internal Server Error\r\n";
				// error_response += "Content-Type: text/html\r\n";
				// error_response += "Content-Length: 52\r\n";
				// error_response += "Connection: close\r\n\r\n";
				// error_response += "<html><body><h1>500 Internal Server Error</h1></body></html>";
			}
			if (bytes_read == -1)
				perror("Read error");			
			close(client_fd);
			}
			LOG_INFO("Conection closed, awaiting next client");
		}
    }
}
