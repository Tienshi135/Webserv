#include "header.hpp"

void printMap(const std::map<std::string, Server> &buffer)
{
    std::cout << "=== Configuration Map Contents ===" << std::endl;
    std::cout << "Total servers: " << buffer.size() << std::endl;
    std::cout << std::endl;
    
    if (buffer.empty())
    {
        std::cout << "Map is empty!" << std::endl;
        return;
    }
    
    for (std::map<std::string, Server>::const_iterator it = buffer.begin(); 
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
        if (!it->second.getErrorPage().empty())
            std::cout << "  Error Page: " << it->second.getErrorPage() << std::endl;
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
	std::map<int, Server>					sfd;
	std::map<std::string, Server>			buffer;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [*.conf]" << std::endl;
		return (-1);
	}

	//first part - parsing
	parse(buffer, argv[1]);
	printMap(buffer);

	//second part - socket creation and binding
	std::map<std::string, Server>::iterator it = buffer.begin();
	while (it != buffer.end())
	{
		int	socket_fd = socket_init(it);
		if (socket_fd != -1)
		{
			sfd.insert(std::pair<int, Server>(socket_fd, it->second));
			std::cout << "Server " << it->first << " listening successfully!" << std::endl;
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
		fd_set	read_fd, write_fd;
		int		max_fd;
		FD_ZERO(&read_fd);
		FD_ZERO(&write_fd);

		max_fd = 0;
        std::map<int, Server>::iterator sfd_it = sfd.begin();
        while (sfd_it != sfd.end())
        {
            FD_SET(sfd_it->first, &read_fd);
			if (sfd_it->first > max_fd)
				max_fd = sfd_it->first;
            sfd_it++;
        }
		int	activity = select(max_fd + 1, &read_fd, &write_fd, NULL, NULL);
		if (activity == -1)
		{
			if (errno == EBADF)
				std::cerr << "Select error: Bad file descriptor detected" << std::endl;				
			else
				std::cerr << "Select error: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
			perror("Select failed");
			for (std::map<int, Server>::iterator sfd_it = sfd.begin(); sfd_it != sfd.end(); ++sfd_it)
				{
					close(sfd_it->first);
				}
			return (-1);
		}

		//connection request
		sfd_it = sfd.begin();
		while (sfd_it != sfd.end())
        {
            if (FD_ISSET(sfd_it->first, &read_fd))
			{
				Server	conf = sfd_it->second;
				char	input[conf.getMaxBodySize()];

				int	client_fd = accept(sfd_it->first, NULL, NULL);//other args might be usefull idk yet
				if (client_fd == -1)
				{
					std::cerr << "Accept error: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
					continue;
				}

				size_t	bytes_recv = recv(client_fd, &input, conf.getMaxBodySize(), 0);//MSG_OOB
				if (bytes_recv < 0)
				{
					std::cerr << "Recv error: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
					continue;
				}
				else if (bytes_recv == 0)
				{
					std::cout << "Connection closed by client" << std::endl;
					continue;
				}
				else
				{
					std::cout << input << std::endl;
					Request		req(input);
					Response	resp(conf, req);
					std::string	response_str = resp.buildResponse();
					int bytes_sent = send(client_fd, response_str.c_str(), response_str.length(), 0);//MSG_OOB
					if (bytes_sent == -1)
					{
						std::cerr << "Send error: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
						continue;
					}
				}
				close(client_fd);
			}
            sfd_it++;
        }
    }
}