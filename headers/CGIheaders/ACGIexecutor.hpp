#pragma once

#include "header.hpp"

class ACGIexecutor
{
private:
	/* data */
public:
	ACGIexecutor(/* args */);
	~ACGIexecutor();

	virtual	void	execute() = 0;
};

