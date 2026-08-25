#include <cstdint>
	// uint types
#include <iostream>
	// std::cout
#include <print>
	// std::print
#include <climits>
	// uint_max
bool shiftValue(uint64_t num = 1, uint16_t shifts = 8)
{
	std::print(std::cout, "\n=== Shift uint {} {} times ===\n", num, shifts);
	std::print(std::cout, "Num's starting value: {}\n", num);
	for (uint8_t i = 0; i < shifts; ++i)
	{	
		num = num << 1;
		std::print(std::cout, "Num shifted {1:} times {0:}\n", num, i);
	}

	return true;
}

bool assignEveryBitTo1(uint64_t num = 0, uint8_t bitLength = 16)
{	
	std::print(std::cout, "\n==== Assign Every Bit to 1 ====\n");
	std::print(std::cout, "Num's starting value: {}\n", num);
	for (uint8_t i = 0; i < bitLength; ++i)
	{
		if (((num >> i) % 2) == 1)
		{
			continue;
		}	
		
		std::print(std::cout, "Num's {0:>} bit assigned to 1. Num + bitAddedValue = {1:>} + {2:>} \n", i, num, (1 << i));	
		num += (1 << i);
	}
	
	std::print(std::cout, "\nNum's Final Value: {}\n", num);
	return true;
}	

bool addAbsurdlyLargeInteger()
{
	uint64_t num = 0;
	for ( ; num < UINT_MAX; ++num);
        std::print(std::cout, "{}", num);	
	return true;
}

int main (void)
{
	shiftValue();
	assignEveryBitTo1(0);
	assignEveryBitTo1(3000);
	addAbsurdlyLargeInteger();
}
