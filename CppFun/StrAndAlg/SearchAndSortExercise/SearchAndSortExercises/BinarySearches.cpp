#include "BinarySearches.h"
#include <iostream>
namespace Searches
{
	int BinarySearchEx1(const std::vector<int>& sortedArr, int target, bool outputStats)
	{
		int low = 0;
		int high = sortedArr.size() - 1;

		int projectedIterations = std::log2(sortedArr.size());
		if (outputStats)
		{
			std::cout << "Sorted Arr: ";
			for (int i = 0; i < sortedArr.size(); i++)
			{
				std::cout << sortedArr[i] << ' ';
			}

			std::cout << std::log2(sortedArr.size()) << " projected iterations" << std::endl;
		}

		int iterations = 0;
		while (low != high)
		{
			iterations++;
			if (iterations > projectedIterations)
			{
				std::cout << "Iterations exeeded projected iterations. Something wrong with algorithm?" << std::endl;
				return -1;
			}
			int mid = (high - low) / 2;
			
			if (target == sortedArr.at(mid))
			{
				if (outputStats)
				{
					std::cout << "Found target " << target << " at index " << mid << ".'\n";
					std::cout << iterations << " actual iterations" << std::endl;
				}
				return mid;
			}

			if (target > sortedArr.at(mid))
			{
				low = mid + 1;
				continue;
			}

			high = mid - 1;

		}
	}
}