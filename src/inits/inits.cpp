#include "header.hpp"

static struct sockaddr_in* init_sockaddr(ServerCfg conf)
{
    struct sockaddr_in  *addr_in = new struct sockaddr_in;
    struct addrinfo     hints, *result;
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    int status = getaddrinfo(conf.getHost().c_str(), NULL, &hints, &result);
    if (status != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        delete (addr_in);
        return (NULL);
    }
    *addr_in = *reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
    addr_in->sin_port = htons(conf.getPort());
    freeaddrinfo(result);
    return (addr_in);
}

<<<<<<< HEAD
int socket_init(std::vector<Server>::iterator current)
=======
int socket_init(std::map<std::string, ServerCfg>::iterator current)
>>>>>>> main
{
    struct sockaddr_in *addr = init_sockaddr(*current);
    if (addr == NULL)
    {
        std::cerr << "Failed to initialize socket address" << std::endl;
        return (-1);
    }
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1)
    {
        perror("Socket creation failed");
        std::cout << "Skipping server: " << current->getName() << std::endl;
        delete (addr);
        return (-1);
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("Setsockopt SO_REUSEADDR failed");
        close(socket_fd);
        std::cout << "Skipping server: " << current->getName() << std::endl;
        delete (addr);
        return (-1);
    }

<<<<<<< HEAD
    if (bind(socket_fd, (struct sockaddr *)addr, sizeof(*addr)) == -1)
=======
    if (bind(socket_fd, (struct sockaddr *)addr, sizeof(*addr)) == -1)// change to use error handling func
>>>>>>> main
    {
        perror("Bind error");
        close(socket_fd);
        std::cout << "Skipping server: " << current->getName() << " (IP not available)" << std::endl;
        delete (addr);
        return (-1);
    }

<<<<<<< HEAD
    if (listen(socket_fd, 10) == -1)
=======
    if (listen(socket_fd, 10) == -1)//change to use error handling func
>>>>>>> main
    {
        perror("listen error");
        close(socket_fd);
        std::cout << "Skipping server: " << current->getName() << std::endl;
        delete (addr);
        return (-1);
    }
<<<<<<< HEAD
=======

>>>>>>> main
    delete (addr);
    return (socket_fd);
}
