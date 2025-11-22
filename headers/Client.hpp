#pragma once

#include "header.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseFactory.hpp"
#include "signal.h"
#include <unistd.h>


class Client {
	private:

		struct timeval	_creation_time;
		struct timeval	_request_start_time;
		struct timeval	_request_end_time;
		struct timeval	_response_sent_time;

		int					_client_fd;
		int					_bytes_expected;
		int					_bytes_read;
		size_t				_total_bytes_Received;
		Request				_request;
		ServerCfg const&	_config;

	//private member functions
		bool    _checkSizeLimits();
	public:
		Client(std::map<int, ServerCfg>::iterator const& fd_and_cfg);
		Client(const Client &copy);
		Client &operator=(const Client &copy);
		~Client();

		Request&	getRequest() {return this->_request; }
		int			getClientFd() const { return this->_client_fd; }
		void		setClientFd(int fd) { this->_client_fd = fd; }
		int			getBytesRead() const { return this->_bytes_read; }
		void		setBytesRead(int bytes) { this->_bytes_read = bytes; }
		int			getBytesExpected() const { return this->_bytes_expected; }
		void		setBytesExpected(int bytes) { this->_bytes_expected = bytes; }
		size_t		getTotalBytesReceived() const { return this->_total_bytes_Received; }
		void		setTotalBytesReceived(size_t total_bytes_received) {this->_total_bytes_Received = total_bytes_received; }

		void		printPerformanceStats(void) const;
		bool		isCompleteRequest(void);
		int			readBuffer(void);
		void		sendResponse();
		void		closeConnection();
};
