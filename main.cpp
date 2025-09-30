#include <fstream>
#include <iostream>
#include <cctype>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

#include "header.hpp"

unsigned int stringToIP(const std::string &ip_str)//might change implementation to use result from getaddrinfo
{
    struct addrinfo hints, *result;
    std::memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int status = getaddrinfo(ip_str.c_str(), NULL, &hints, &result);
    if (status != 0)
	{
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return INADDR_ANY;
    }
    
    struct sockaddr_in* addr_in = (struct sockaddr_in*)result->ai_addr;
    unsigned int ip_addr = addr_in->sin_addr.s_addr;
    freeaddrinfo(result);
    
    return (ip_addr);
}

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
	struct sockaddr_in						addr;
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
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(buffer[it->first].getListen());
		addr.sin_addr.s_addr = stringToIP(buffer[it->first].getHost());
		std::cout << "Binding to IP: " << buffer[it->first].getHost() << " on port " << buffer[it->first].getListen() << std::endl;
		
		int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (socket_fd == -1)
		{
			perror("Socket creation failed");
			std::cout << "Skipping server: " << it->first << std::endl;
			it++;
			continue;
		}
		
		int opt = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			perror("Setsockopt SO_REUSEADDR failed");
			close(socket_fd);
			std::cout << "Skipping server: " << it->first << std::endl;
			it++;
			continue;
		}
		
		if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		{
			perror("Bind error");
			close(socket_fd);
			std::cout << "Skipping server: " << it->first << " (IP not available)" << std::endl;
			it++;
			continue;
		}
		
		if (listen(socket_fd, 10) == -1)
		{
			perror("Listen error");
			close(socket_fd);
			std::cout << "Skipping server: " << it->first << std::endl;
			it++;
			continue;
		}
		
		sfd.push_back(socket_fd);
		std::cout << "Server " << it->first << " listening successfully!" << std::endl;
		it++;
	}
	if (sfd.empty())
	{
		std::cout << "No servers could be started! Exiting." << std::endl;
		return (-1);
	}

	//third part - main loop with select
	fd_set readfds;
	struct timeval timeout;
	while (true)
	{
		int	max_fd = 0;
		FD_ZERO(&readfds);
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