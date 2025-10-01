#include "header.hpp"

void printMap(const std::map<std::string, Configuration> &buffer)
{
    std::cout << "=== Configuration Map Contents ===" << std::endl;
    std::cout << "Total configurations: " << buffer.size() << std::endl;
    std::cout << std::endl;
    
    if (buffer.empty())
    {
        std::cout << "Map is empty!" << std::endl;
        return;
    }
    
    for (std::map<std::string, Configuration>::const_iterator it = buffer.begin(); 
         it != buffer.end(); ++it)
    {
        std::cout << "Key: " << it->first << std::endl;
        std::cout << "  Name: " << it->second.getName() << std::endl;
        std::cout << "  IP: " << it->second.getHost() << std::endl;
        std::cout << "  Port: " << it->second.getListen() << std::endl;
        std::cout << "  Root: " << it->second.getRoot() << std::endl;
        std::cout << "  Index: " << it->second.getIndex() << std::endl;
        std::cout << "  Error Page: " << it->second.getErrorPage() << std::endl;
        std::cout << "  Max Body Size: " << it->second.getMaxBodySize() << std::endl;
        std::cout << "---" << std::endl;
    }
}

int main(int argc, char **argv)
{
	std::vector<int>						sfd;
	std::map<std::string, Configuration>	buffer;

	if (argc != 2)
	{
		std::cout << "usage: ./webserv [*.conf]" << std::endl;
		return (-1);
	}

	//first part - parsing
	parse(buffer, argv[1]);
	printMap(buffer);

	//second part - socket creation and binding
	std::map<std::string, Configuration>::iterator it = buffer.begin();
	while (it != buffer.end())
	{
		int	socket_fd = socket_init(it);
		if (socket_fd != -1)
		{
			sfd.push_back(socket_fd);
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
		for (size_t i = 0; i < sfd.size(); i++)
		{
			FD_SET(sfd[i], &readfds);
			if (sfd[i] > max_fd)
				max_fd = sfd[i];
		}
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		int activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
		if (activity < 0)
		{
			perror("Select error");
			close(sfd.back());
			return (-1);
		}
		else if (activity == 0)
		{
			std::cout << '.' << std::endl;
			continue;
		}
		for (size_t i = 0; i < sfd.size(); i++)
		{
			if (FD_ISSET(sfd[i], &readfds))
			{
				int client_fd = accept(sfd[i], NULL, NULL);
				if (client_fd == -1)
				{
					perror("Accept error");
					continue;
				}
			
			char buffer[1024];
			ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);//add check for received bigger than buffer size
			if (bytes_read > 0)
			{
				buffer[bytes_read] = '\0';
				std::cout << "Received: " << buffer << std::endl;
				
				const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nHello";
				ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
				if (bytes_sent == -1)
					perror("Send error");
			}
			if (bytes_read == -1)
				perror("Read error");			
			close(client_fd);
			}
		}
	}
}