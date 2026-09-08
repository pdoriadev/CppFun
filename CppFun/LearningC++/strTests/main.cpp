#include <iostream>

int main()
{
	const char* worldStr = "World";
	std::string fullStr = "Hello ".c_str() + worldStr;
	std::cout << fullStr << std::endl;

	return 0;
}
