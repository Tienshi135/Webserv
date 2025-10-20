#include "header.hpp"

int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "fcntl F_GETFL error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "fcntl F_SETFL error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    return 0;
}

int main(int argc, char **argv)
{
	std::map<int, Server>				sfd;
	std::map<int, std::vector<int> >	cfd;  // server_fd -> vector of client_fds
	std::vector<Server>					buffer;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [*.conf]" << std::endl;
		return (-1);
	}

	//first part - parsing
	parse(buffer, argv[1]);

	//second part - socket creation and binding
	std::vector<Server>::iterator it = buffer.begin();
	while (it != buffer.end())
	{
		int	socket_fd = socket_init(it);
		if (socket_fd != -1)
		{
			sfd.insert(std::pair<int, Server>(socket_fd, *it));
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
		std::map<int, Server>::iterator sfd_it = sfd.begin();
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
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(sfd_it->first, (struct sockaddr *)&client_addr, &client_len);
				if (client_fd < 0)
					std::cerr << "accept() error: " << strerror(errno) << std::endl;
				if (setNonBlocking(client_fd) == -1)
					close(client_fd);
				cfd[sfd_it->first].push_back(client_fd);
			}
			else if (FD_ISSET(sfd_it->first, &read_fd) && !cfd[sfd_it->first].empty())
			{
				// Handle data from connected clients (not implemented here)
			}
			sfd_it++;
		}
    }
}