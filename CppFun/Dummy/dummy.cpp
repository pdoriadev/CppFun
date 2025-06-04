#include <iostream>

void dummy()
{
	std::cout << "i dum" << '\n';

}

int func(int a, int b);
int otherFunc(int a, int b, int c, int d);

int main()
{
	dummy();
	void (*func)() = dummy;
	std::cout << "test";
	func();
	return 0;
}


int f(int a, int b, int c, int d)
{
    int result = func(a, b);
    return func(result, c+d);
}

int func(int a, int b)
{
    return otherFunc(a, b, a*2, b*2 );
}

int otherFunc(int e, int  f, int g, int h)
{
    return e * f * g * h;
}
