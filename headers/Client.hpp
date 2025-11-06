#pragma once

#include "header.hpp"

class Client {
	private:
		int					_client_fd;
		std::vector<char>	_read_buffer;
		int					_bytes_expected;
		int					_bytes_read;

	public:
	Client();
	Client(const Client &copy);
	Client &operator=(const Client &copy);
	~Client();

	int		getClientFd() const { return this->_client_fd; }
	void	setClientFd(int fd) { this->_client_fd = fd; }
	int		getBytesRead() const { return this->_bytes_read; }
	void	setBytesRead(int bytes) { this->_bytes_read = bytes; }
	int		getBytesExpected() const { return this->_bytes_expected; }
	void	setBytesExpected(int bytes) { this->_bytes_expected = bytes; }
	void	addToBuffer(const char* data, int size);

	bool	isCompleteRequest(void);
	std::string concatBuffer() const { return std::string(this->_read_buffer.begin(), this->_read_buffer.end()); }
};
