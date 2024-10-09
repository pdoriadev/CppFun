#include "SortedArrays.h"

namespace SortedArrays
{
	static std::vector<int> counting = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
	static std::vector<int> evens = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50 };
	
	std::vector<int>& getSortedIntPos(int selection)
	{
		switch (selection)
		{
		case 0 :
			return counting;
		case 1 : 
			return evens;

		}
	}

	static std::vector<int> countingNeg = { 0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16, -17, -18, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, -30 };
	std::vector<int>& getSortedIntNeg(int selection)
	{
		return countingNeg;
	}
}