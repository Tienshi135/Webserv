#include "header.hpp"
#include "Request.hpp"
#include "Response_headers/Response.hpp"
#include "Response_headers/ResponseFactory.hpp"
#include "Response_headers/ResponseError.hpp"
#include "Response_headers/ResponseGet.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <signal.h>


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
        {
            std::vector<std::string> methods = it->getMethods();
            std::vector<std::string>::const_iterator it_m;
            std::cout << "  Allowed Methods: ";
            for (it_m = methods.begin(); it_m != methods.end(); it_m++)
                std::cout << *it_m << " ";
            std::cout << std::endl;
        }
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
                std::cout << "        Path: " << loc_it->second.getLocationPath() << std::endl;
                if (!loc_it->second.getMethods().empty())
                {
                    std::vector<std::string> methods = loc_it->second.getMethods();
                    std::vector<std::string>::const_iterator it;
                    std::cout << "      Allowed Methods: ";
                    for (it = methods.begin(); it != methods.end(); it++)
                        std::cout << *it << " ";
                    std::cout << std::endl;
                }
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

void signalHandler(int signum)
{
    std::cout << RED << "\n🛑 Signal: " << strsignal(signum) << ". Cleaning up..." << RESET << std::endl;
    exit(0);
}

int main(int argc, char **argv)
{
	std::map<int, ServerCfg>			sfd;//map of server fds to their configurations couldchange to a class
	std::map<int, std::vector<Client*> >	cfd;//map of server fds to their clients
	std::vector<ServerCfg>				buffer;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [*.conf]" << std::endl;
		return (-1);
	}

    signal(SIGINT, signalHandler);
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
			cfd[socket_fd] = std::vector<Client*>();
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

			for (size_t i = 0; i < cfd[sfd_it->first].size(); i++)
            {
                int client_fd = cfd[sfd_it->first][i]->getClientFd();
                FD_SET(client_fd, &read_fd);
                if (client_fd > max_fd)
                    max_fd = client_fd;
            }
            sfd_it++;
		}
		struct timeval timeout;
		timeout.tv_sec = 0;
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
                Client  *new_client = new Client(sfd_it);
                if (new_client->getClientFd() < 0)
                    delete (new_client);
                else
                    cfd[sfd_it->first].push_back(new_client);
            }

            for (int i = cfd[sfd_it->first].size() - 1; i >= 0; i--)
            {
                Client &client = *cfd[sfd_it->first][i];
                if (FD_ISSET(client.getClientFd(), &read_fd))
                {
                    if (client.readBuffer() >= 0)
                    {
                        if (!client.isCompleteRequest())
                            continue;

                        client.getRequest().printRequest();
                        client.getRequest().printRecepAnalisis(client.getTotalBytesReceived());

                        client.sendResponse();

                        /*if we stopped the reading because payload to large and the socked still have data
                            accept and select will try to read again from the same socket, so we need to properly close it before deleting
                            the reference.
                        */
                    }
                    client.closeConnection(read_fd);
                    delete (cfd[sfd_it->first][i]);
                    cfd[sfd_it->first].erase(cfd[sfd_it->first].begin() + i);
                }
            }
            sfd_it++;
        }
    }
    return (0);
}
