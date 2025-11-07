#include "Client.hpp"

#include "Request.hpp"

/*============================= Constructors and destructor =====================================*/
Client::Client() : _client_fd(-1), _bytes_expected(-1), _bytes_read(0)
{
	_read_buffer.reserve(1024);
}

Client::Client(const Client &copy)
{
	this->_client_fd = copy._client_fd;
	this->_read_buffer = copy._read_buffer;
	this->_bytes_expected = copy._bytes_expected;
	this->_bytes_read = copy._bytes_read;
}

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

bool	Client::isCompleteRequest(void)
{
	std::string input = this->concatBuffer();

	if (input.find("\r\n\r\n") == std::string::npos)
		return (false);
	Request	req(input);
	if (!req.isValid())
		return (true);
	if (this->_bytes_read < req.getExpectedReadBytes())
		return (false);
	return (true);
}

void	Client::addToBuffer(const char* data, int size)
{
	if (data == NULL || size <= 0)
		return;
	if (this->_read_buffer.capacity() < this->_read_buffer.size() + size)
		this->_read_buffer.resize(this->_read_buffer.size() + size);
	this->_read_buffer.insert(this->_read_buffer.end(), data, data + size);
}
