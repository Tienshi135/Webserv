#pragma once

#include "header.hpp"

class ACGIexecutor;
class Request;
class Configuration;

class CGIfactory
{
	private:
		CGIfactory();
		~CGIfactory();
	public:

		static bool	isValidCGI(Request const& req, Configuration const& cfg);
		static ACGIexecutor	*create(Request const& req, Configuration const& cfg);
};

