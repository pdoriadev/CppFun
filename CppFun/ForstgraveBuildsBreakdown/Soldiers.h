#pragma once
#include "Attributes.h"
#include "Items.h"
#include <vector>
#include <string>
#include <assert.h>

namespace Soldiers
{

	// used to represent a specific soldier
	class SoldierInstance
	{
	protected:
		Soldier currentSoldierData;
		Soldier baseSoldierData;

	public:

		SoldierInstance(int baseSoldierIndex)
		{
			baseSoldierData = soldiers[baseSoldierIndex];
		}

		const Soldier* getCurrentSoldierData() { return &currentSoldierData; }
		const Soldier* getBaseSoldierData() { return &baseSoldierData; }


	};


	// used to represent base static soldier data for given soldierTypes
	class Soldier
	{
		std::string name = "";
		Attributes attributes;
		std::vector<ItemInterface> items;
		int maxItems = 1;
		// weapons
		// cost

		bool hasSet = false;

	public:

		Soldier()
		{
			assert(hasSet && "Failed to set all necessary data.");
		}

		Soldier(std::string _name, Attributes _attributes, int _maxItems)
		{
			name = _name;
			attributes = _attributes;
			maxItems = _maxItems;

			hasSet = true;
		}

		Soldier(std::string _name, Attributes _attributes, ItemInterface item, int _maxItems)
		{
			name = _name;
			attributes = _attributes;
			items.push_back(item);
			maxItems = _maxItems;

			hasSet = true;
		}
	};


	static const Soldier soldiers[]
	{
		Soldier("Thug", Attributes(6, 2, 0, 10, -1, 10), 1),
		Soldier("Apothecary", Attributes(6, 1, 0, 10, 3, 12), HealthPotion(0), 1)
	};

}
