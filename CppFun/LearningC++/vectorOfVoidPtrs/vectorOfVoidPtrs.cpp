#include <vector>
#include <iostream>

/*
 * Using an array of void pointers to get data to whatever type I want
 * **as long as I know the type it is pointing to**
 */
bool intVectorToVoidPtrVector() {
	std::vector<int> intVec = {100, 200, 300};
	std::vector<void*> voidVec;  
	
	for (int i = 0; i < intVec.size(); ++i) {
		voidVec.push_back((void*)&intVec[i]); // casts memory location for int to void*. Caches it. 
		std::cout << intVec.at(i) << std::endl; // prints int from int vector.
		std::cout << *(int*)voidVec.at(i) << std::endl; // prints int at same memory location from void* vector. 
	}

	return true;
}


/*
 * PROBLEM 
 * each type vec is a void* vector BUT the void* has to actually be pointing to something, ya know. So, where's it pointing to? Itself? Don't make much sense. A vector of voids don't make much sense either. Have to actually point to the vector to add, remove, etc. How do I have a pointer to a vector whose type is determined at runtime??
 * */

/* Solution. 
 * C-style dynamic array of void*. 
 * These have the added benefit of being able to point to each other. 
 *
 */

int main() {
	
  void* voidPtrArr[]; // create voidPtrArr. 
	// allocate memory? 
	// copy values into allocated memory. 
	// size of element
	// length of array. 
  std::cout << *voidPtrArr[0] << std::endl;
	return 0;
}
