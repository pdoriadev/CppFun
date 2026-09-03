#include <iostream>

/*
 * CONCLUSIONS ON WRITING THIS
 *
 * You cannot pass a ptr to a function for it to be assigned.
 * The memory the ptr is pointing to will (a) be deallocated and/or (b) be outside
 * 	its local memory, leading to a Segmentation Fault.
 * Can attempt to replicate C#'s out keyword by passing references, HOWEVER,
 * 	this requires the object be initialized. Dunno if this will work for all ttypes I want reliable. Maybe?
 * 	It also does not *enforce* being assigned in the called function. It's another thing for the programmer to track, which, can work. I'd rather be told when I've forgotten to assign something than have to debug it or setup error checks.
 */

int* generateInt()
{
	int* x;
	*x = 1;
	std::cout << "generated int value: " << *x << std::endl;
	return x;
}

// passing by reference.
bool updatePtr(int& outInt)
{
	// assigning reference's address to the address of an int ptr returned from another function
	outInt = *generateInt();
	if (&outInt == NULL)
	{
		std::cout << "int ptr is null in local function." << std::endl;
		return false;	
	}

	return true; 
}

int main()
{
	bool result = false;
	int z; // default initialization
	result = updatePtr(z);
	if(&z == NULL)
	{
		return -1;
		std::cout << "int ptr is null in main()" << std::endl;
		return -1;
	}

	std::cout << "int ptr's value in main(): " << z << std::endl;
	std::cout << "bool result value: " << result << std::endl;

	return 0;
}
