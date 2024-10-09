#include "BinarySearches.h"
#include "SortedArrays.h"

int main()
{
	const std::vector<int>& arr = SortedArrays::getSortedIntPos(0);
	Searches::BinarySearchEx1(arr, arr[arr.size() - 5], true);

	return 0;
}