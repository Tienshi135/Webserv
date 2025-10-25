#include "ParsingException.hpp"

ParsingException::ParsingException(std::string file, int line, std::string errormsg){
	std::ostringstream oss;
	oss << RED << "Error at: " << RESET
		<< file  << ":" << line << "\n"
		<< RED << errormsg << RESET;
	this->_msg = oss.str();
}

ParsingException::ParsingException(std::string file, int line, std::string errormsg, size_t cfgLine){
	std::ostringstream oss;
	oss << RED << "Error at: " << RESET
		<< file  << ":" << line << "\n"
		<< RED << errormsg << ", around config line " RESET
		<< "[" << cfgLine << "]";
	this->_msg = oss.str();
}

const char* ParsingException::what() const throw()
{
	return this->_msg.c_str();
}

ParsingException::~ParsingException(void) throw() {}
