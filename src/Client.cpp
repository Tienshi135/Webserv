#include "Client.hpp"
#include "Configuration.hpp"

/*============================= Constructors and destructor =====================================*/
Client::Client(std::map<int, ServerCfg>::iterator const& fd_and_cfg)
: _client_fd(-1), _bytes_expected(-1), _bytes_read(0), _request(), _config(fd_and_cfg->second)
{
	this->_read_buffer.reserve(1024);

	int client_fd = accept(fd_and_cfg->first, NULL, NULL);
	if (client_fd == -1)
	{
		perror("Accept error");
		LOG_WARNING_LINK("Accept failed to open fd [" + numToString(client_fd) + \
		"] for server config: " + this->_config.getName());
		return;
	}
	int op = fcntl(client_fd, F_GETFL);
	if (op == -1)
	{
		perror("Fcntl F_GETFL error");
		LOG_HIGH_WARNING_LINK("Fd [" + numToString(client_fd) + "] for server: [" + this->_config.getName() + \
		"] is not a valid fd or already closed (posible datar race)");
		close(client_fd);
		return;
	}
	if (fcntl(client_fd, F_SETFL, op | O_NONBLOCK) == -1)
	{
		perror("Fcntl F_SETFL error");
		LOG_HIGH_WARNING_LINK("Fd [" + numToString(client_fd) + "] is not a valid fd");
		close(client_fd);
		return;
	}
	this->_client_fd = client_fd;
	LOG_INFO("Client connected on server : " + this->_config.getName() + " (fd: " + numToString(client_fd) + ")");
}

Client::Client(const Client &copy)
: _client_fd(copy._client_fd),
  _read_buffer(copy._read_buffer),
  _bytes_expected(copy._bytes_expected),
  _bytes_read(copy._bytes_read),
  _request(copy._request),
  _config(copy._config) {}

Client &Client::operator=(const Client &copy)
{
	if (this != &copy)
	{
		this->_client_fd = copy._client_fd;
		this->_read_buffer = copy._read_buffer;
		this->_bytes_expected = copy._bytes_expected;
		this->_bytes_read = copy._bytes_read;
	}
	return *this;
}

Client::~Client() {};

/*============================= Public member functions =====================================*/

bool	Client::isCompleteRequest(void)//TODO refactor completely
{
	return this->_request.getRequestCompleted();
}

void	Client::addToBuffer(const char* data, int size)
{
	if (data == NULL || size <= 0)
		return;
	if (this->_read_buffer.capacity() < this->_read_buffer.size() + size)
		this->_read_buffer.resize(this->_read_buffer.size() + size);
	this->_read_buffer.insert(this->_read_buffer.end(), data, data + size);
}

int	Client::readBuffer()
{
	int ret = 0;
	while (!this->isCompleteRequest())
	{
		char buffer[1024];//TODO find a bigger reading size, low buffer increases loops exponentialy on larger files so is less efficient
		this->_bytes_read = recv(this->_client_fd, buffer, sizeof(buffer), 0);
		if (this->_bytes_read == 0)
		{
			//TODO client is disconected, handle this?
			// LOG_INFO("Client disconnected : " + numToString(this->_client_fd));
			// close(this->_client_fd);
			// this->_client_fd = -1;
		}
		else if (this->_bytes_read < 0)
		{
			//TODO handle read error
			//if body is expected but body size is not equal expected size launch error?
			this->_request.setRequestCompleted(true);
			ret = -1;
			break;
		}
		else
		{
			this->addToBuffer(buffer, this->_bytes_read);//TODO a bit inneficient, maybe we have to rethink this logic
			ret =  this->_request.parseInput(this->_read_buffer);
			if (ret < 0)
				break;
		}
	}
	return ret;
}

void	Client::sendResponse(void)
{
	Response* response = ResponseFactory::createResponse(this->_config, this->_request);
	if (!response)
	{
		//TODO handle error
	}

	response->buildResponse();
	std::string response_str = response->getRawResponse();
	response->printResponse();

	ssize_t bytes_sent = send(this->_client_fd, response_str.c_str(), response_str.length(), 0);
	if (bytes_sent == -1)
		perror("Send error");

	delete (response);
}

void	Client::closeConnection(void)
{
	LOG_INFO("Connection closed with client : " + numToString(this->_client_fd) + ", awaiting next client");
	close(this->_client_fd);
}
