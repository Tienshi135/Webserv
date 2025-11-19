#include "Client.hpp"
#include "Configuration.hpp"

/*============================= Constructors and destructor =====================================*/
Client::Client(std::map<int, ServerCfg>::iterator const& fd_and_cfg)
: _client_fd(-1), _bytes_expected(-1), _bytes_read(0), _total_bytes_Received(0), _request(), _config(fd_and_cfg->second)
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

	gettimeofday(&this->_creation_time, NULL);
	LOG_INFO("Client connected on server : " + this->_config.getName() + " (fd: " + numToString(client_fd) + ")");
}

Client::Client(const Client &copy)
: _client_fd(copy._client_fd),
  _read_buffer(copy._read_buffer),
  _bytes_expected(copy._bytes_expected),
  _bytes_read(copy._bytes_read),
//   _request(copy._request),
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

void	Client::printPerformanceStats() const
{
	// Calculate time differences in milliseconds
	size_t connection_to_first_byte =
		((this->_request_start_time.tv_sec - this->_creation_time.tv_sec) * 1000) +
		((this->_request_start_time.tv_usec - this->_creation_time.tv_usec) / 1000);

	size_t request_duration =
		((this->_request_end_time.tv_sec - this->_request_start_time.tv_sec) * 1000) +
		((this->_request_end_time.tv_usec - this->_request_start_time.tv_usec) / 1000);

	size_t response_processing =
		((this->_response_sent_time.tv_sec - this->_request_end_time.tv_sec) * 1000) +
		((this->_response_sent_time.tv_usec - this->_request_end_time.tv_usec) / 1000);

	size_t total_time =
		((this->_response_sent_time.tv_sec - this->_creation_time.tv_sec) * 1000) +
		((this->_response_sent_time.tv_usec - this->_creation_time.tv_usec) / 1000);

	// Calculate throughput (bytes per second)
	size_t throughput = 0;
	if (request_duration > 0)
		throughput = (this->_total_bytes_Received * 1000.0) / request_duration;  // bytes/sec


    std::string title = "PERFORMANCE STATISTICS (fd: " + numToString(this->_client_fd) + ")";
    int width = 60;
    int inner_width = width - 2;

    int padding = (inner_width - title.length()) / 2;
    int right_padding = inner_width - title.length() - padding;

	std::cout << CYAN << "\n╔══════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║" << std::string(padding, ' ') << title << std::string(right_padding, ' ') << "║" << RESET << std::endl;
	std::cout << CYAN << "╠══════════════════════════════════════════════════════════╣" << RESET << std::endl;

	title = " Connection established → First byte: ";
	std::string value = numToString(connection_to_first_byte) + " ms ";
	std::cout << CYAN << "║" << RESET << title << std::setw(width - title.size()) << value << CYAN << "║" << RESET << std::endl;

	title = " Request reception (reading): ";
	value = numToString(request_duration) + " ms ";
	std::cout << CYAN << "║" << RESET << title << std::setw(width - title.size() - 2) << value << CYAN << "║" << RESET << std::endl;

	title = " Response processing + sending: ";
	value = numToString(response_processing) + " ms ";
	std::cout << CYAN << "║" << RESET << title << std::setw(width - title.size() - 2) << value << CYAN << "║" << RESET << std::endl;

	std::cout << CYAN << "╟──────────────────────────────────────────────────────────╢" << RESET << std::endl;

	title = " TOTAL TIME: ";
	value = numToString(total_time) + " ms ";
	std::cout << CYAN << "║" << GREEN << title << std::setw(width - title.size() - 2) << value << CYAN << "║" << RESET << std::endl;

	std::cout << CYAN << "╠══════════════════════════════════════════════════════════╣" << RESET << std::endl;

	title = " Bytes received: ";
	value = numToString(this->_total_bytes_Received) + " bytes ";
	std::cout << CYAN << "║"  << RESET << title << std::setw(width - title.size() - 2) << value << CYAN << "║" << RESET << std::endl;

	title = " Throughput: ";
	value = numToString((throughput / 1024)) + " KB/s  ";
	std::cout << CYAN << "║"  << RESET << title << std::setw(width - title.size() - 2) << value << CYAN << "║" << RESET << std::endl;

	std::cout << CYAN << "╚══════════════════════════════════════════════════════════╝" << RESET << std::endl;
}

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
	char buffer[4096];//TODO find a bigger reading size, low buffer increases loops exponentialy on larger files so is less efficient
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
		LOG_HIGH_WARNING_LINK("recv returned error unexpectedly: [" +
			(errno != 0? std::string(strerror(errno)) : "nothing to read") + "]");
		gettimeofday(&this->_request_end_time, NULL);
		this->_request.setRequestCompleted(true);
		return -1;
	}
	else
	{
		if (this->_total_bytes_Received == 0)
			gettimeofday(&this->_request_start_time, NULL);

		int ret = 0;
		this->setTotalBytesReceived(this->getTotalBytesReceived() + this->_bytes_read);
		ret = this->_request.parseInput(buffer, this->_bytes_read, this->_total_bytes_Received);
		if (ret < 0)
		{
			LOG_WARNING("Request parsing failed");
			this->_request.setRequestCompleted(true);
			return ret;
		}

		if (this->_total_bytes_Received >= static_cast<size_t>(this->_request.getExpectedReadBytes()))
		{
			gettimeofday(&this->_request_end_time, NULL);
			this->_request.setRequestCompleted(true);
			return ret;
		}
	}

	return (0);
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

	gettimeofday(&this->_response_sent_time, NULL);

	this->printPerformanceStats();
	delete (response);
}

void	Client::closeConnection(void)
{
	LOG_INFO("Connection closed with client : " + numToString(this->_client_fd) + ", awaiting next client");
	close(this->_client_fd);
}
