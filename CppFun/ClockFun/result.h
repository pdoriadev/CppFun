#pragma once
#include <string>
struct result
{
	bool passed;
	std::string message;
	result(bool _passed, std::string _message)
	{
		passed = _passed;
		message = _message;
	}
};