#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>

typedef struct item
{
	std::string name;
	uint16_t weight;
	uint16_t calories;	
} item;

bool greedyAlgo(const uint16_t maxW, const std::vector<item> itemList, &std::vector<item> greedyList) 
{	
	/* Sort the item list by calorie/weight
	 * 	selection sort
	 * Iterate throught the item list, adding each item as long as its weight
	 * 	does not make the total weight exceed maxWeight.
	 * max(n) = 64 items
	 * */
	for (uint16_t i = 0; i < items.count() - 1; ++i)
	{

		for (uint16_t j = i + 1; j < items.count(); ++j)
		{
			
		}	
	}

	

	return true;
}

bool exhaustiveOptimized(uint16_t maxW, std::vector<item> items) 
{
	
	return true;
}

int main(void)
{
	

	return 0;
}

