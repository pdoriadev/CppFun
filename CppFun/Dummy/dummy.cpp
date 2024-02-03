#include <iostream>

void dummy()
{
	std::cout << "i dum" << '\n';

}

int main()
{
	dummy();
	void (*func)() = dummy;
	std::cout << "test";
	func();
	return 0;
}
