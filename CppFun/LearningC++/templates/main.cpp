#include <iostream>
#include "TemplateTest.h"

int main () {
	Number<int> intNumber(1);
	std::cout << intNumber.number << std::endl;
	return 0;
}
