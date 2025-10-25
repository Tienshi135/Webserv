#pragma once

#include <iostream>
#include <stdexcept>
#include <sstream>
#include "colors.hpp"

#define ERR_PARS(msg) ParsingException(__FILE__, __LINE__, msg)
#define ERR_PARS_CFGLN(msg, cfgLine) ParsingException(__FILE__, __LINE__, msg, cfgLine)

class ParsingException : public std::exception
{
	private:
		std::string	_msg;
	public:
		ParsingException(std::string file, int line, std::string errormsg);
		ParsingException(std::string file, int line, std::string errormsg, size_t cfgLine);
		virtual ~ParsingException() throw();
		virtual const char *what() const throw();
};

