#include "header.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseFactory.hpp"
#include "ResponseError.hpp"
#include "ResponseGet.hpp"


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
                if (!loc_it->second.getReturn().isSet)
                    std::cout << "      Return: " << loc_it->second.getReturn().code << " "
							<< loc_it->second.getReturn().value << std::endl;
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
	std::map<int, ServerCfg>					sfd;
	std::map<std::string, ServerCfg>		buffer;//might be overkill and can change

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
	printMap(buffer);

	//second part - socket creation and binding
	std::map<std::string, ServerCfg>::iterator it = buffer.begin();
	while (it != buffer.end())
	{
		int	socket_fd = socket_init(it);
		if (socket_fd != -1)
		{
			sfd.insert(std::pair<int, ServerCfg>(socket_fd, it->second));
			std::cout << "Server " << it->first << " listening successfully!" << std::endl;
		}
		it++;
	}
	if (sfd.empty())
	{
		std::cout << "No servers could be started! Exiting." << std::endl;
		return (-1);
	}

	//third part - main loop with select ( need to change write to use select as well)
	fd_set readfds, writefds;
	struct timeval timeout;
	while (true)
	{
		int	max_fd = 0;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);// not used for now

		for (std::map<int, ServerCfg>::iterator sfd_it = sfd.begin(); sfd_it != sfd.end(); ++sfd_it)
		{
			FD_SET(sfd_it->first, &readfds);
			if (sfd_it->first > max_fd)
				max_fd = sfd_it->first;
		}

		timeout.tv_sec = 60;
		timeout.tv_usec = 0;
		int activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
		if (activity < 0)
		{
			perror("Select error");
			for (std::map<int, ServerCfg>::iterator sfd_it = sfd.begin(); sfd_it != sfd.end(); ++sfd_it)
			{
				close(sfd_it->first);
			}
			return (-1);
		}
		else if (activity == 0)
		{
			return (1);
		}
		for (std::map<int, ServerCfg>::iterator sfd_it = sfd.begin(); sfd_it != sfd.end(); ++sfd_it)
		{
			if (FD_ISSET(sfd_it->first, &readfds))
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

					std::string 		received(buffer);
					Request				req(received);
					ServerCfg const&	cfg = sfd_it->second;

					Response* response = ResponseFactory::createResponse(cfg, req);
					response->buildResponse();
					std::string builtResponse = response->getRawResponse();

					response->printResponse();
					ssize_t bytes_sent = send(client_fd, builtResponse.c_str(), builtResponse.length(), 0);
					if (bytes_sent == -1)
						perror("Send error");
					delete response;
				}
				if (bytes_read == -1)
					perror("Read error");
				close(client_fd);
			}
			LOG_INFO("Conection closed, awaiting next client");
		}
	}
}
