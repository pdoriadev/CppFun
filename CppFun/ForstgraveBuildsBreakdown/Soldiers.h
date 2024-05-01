#pragma once
#include "Attributes.h"
#include "Items.h"
#include <vector>
#include <string>

// this is where all the soldier data goes
class Soldier
{
	std::string name = "";
	Attributes attributes;
	std::vector<ItemInterface> items;
	int maxItems = 1;
	// weapons
	// cost

public:
	Soldier(std::string _name, Attributes _attributes, int _maxItems)
	{
		name = _name;
		attributes = _attributes;
		maxItems = _maxItems;
	}

	Soldier(std::string _name, Attributes _attributes, ItemInterface item, int _maxItems)
	{
		name = _name;
		attributes = _attributes;
		items.push_back(item);
		maxItems = _maxItems;
	}
};


static const Soldier soldiers[]
{
	Soldier("Thug", Attributes(6, 2, 0, 10, -1, 10), 1),
	Soldier("Apothecary", Attributes(6, 1, 0, 10, 3, 12), HealthPotion(0), 1)
}