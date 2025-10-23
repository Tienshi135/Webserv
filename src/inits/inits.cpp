#include "header.hpp"

static struct sockaddr_in* init_sockaddr(ServerCfg conf)
{
    struct addrinfo hints, *result;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(conf.getHost().c_str(), NULL, &hints, &result);
    if (status != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return (NULL);
    }
    struct sockaddr_in* addr_in = new struct sockaddr_in;
    std::memcpy(addr_in, result->ai_addr, sizeof(struct sockaddr_in));
    addr_in->sin_port = htons(conf.getPort());
    freeaddrinfo(result);
    return (addr_in);
}

int socket_init(std::map<std::string, ServerCfg>::iterator current)
{
    struct sockaddr_in *addr = init_sockaddr(current->second);
    if (addr == NULL)
    {
        std::cerr << "Failed to initialize socket address" << std::endl;
        return (-1);
    }

    std::cout << "Binding to IP: " << current->second.getHost() << " on port " << current->second.getPort() << std::endl;

    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1)//change to use error handling func
    {
        perror("Socket creation failed");
        std::cout << "Skipping server: " << current->first << std::endl;
        delete (addr);
        return -1;
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)//change to use erro rhandling func
    {
        perror("Setsockopt SO_REUSEADDR failed");
        close(socket_fd);
        std::cout << "Skipping server: " << current->first << std::endl;
        delete (addr);
        return -1;
    }

    if (bind(socket_fd, (struct sockaddr *)addr, sizeof(*addr)) == -1)// change to use error handling func
    {
        perror("Bind error");
        close(socket_fd);
        std::cout << "Skipping server: " << current->first << " (IP not available)" << std::endl;
        delete (addr);
        return -1;
    }

    if (listen(socket_fd, 10) == -1)//change to use error handling func
    {
        perror("Port error");
        close(socket_fd);
        std::cout << "Skipping server: " << current->first << std::endl;
        delete (addr);
        return -1;
    }

    delete (addr);
    return (socket_fd);
}
