#pragma once
#include <string>
#include <vector>
#include <assert.h>

/*
TO-DO:
	function that gets school info.
	function thtat gets category info. 

*/


namespace Schools
{
	enum type
	{
		UNASSIGNED,		// 0
		CHRONOMANCER,	// 1
		ELEMENTALIST,	// 2
		ENCHANTER,		// 3
		ILLUSIONIST,	// 4
		NECROMANCER,	// 5
		SIGILIST,		// 6
		SOOTHSAYER,		// 7
		SUMMONER,		// 8
		THAUMATURGE,	// 9
		WITCH			// 10
	};

	static const std::string typeStrings[] =
	{
		"UNASSIGNED",	// 0
		"CHRONOMANCER",	// 1
		"ELEMENTALIST",	// 2
		"ENCHANTER",	// 3
		"ILLUSIONIST",	// 4
		"NECROMANCER",	// 5
		"SIGILIST",		// 6
		"SOOTHSAYER",	// 7
		"SUMMONER",		// 8
		"THAUMATURGE",	// 9
		"WITCH"			// 10 
	};

	static const std::string descriptions[] =
	{
		"N/A",															// 0
		"Time Wizard. Gamblers and risk-takers.",						// 1
		"Avatar-y. Control of four elements. Damage dealer.",			// 2
		"Artifer-y.",													// 3
		"Conjuerer. Trickster.",										// 4
		"Scribe. Power from runes.",									// 5
		"Seer. Sage of History. Fortune Teller.",						// 6
		"Summoner. Summons demons (any creature from another plane)",	// 7
		"Healer. Support",												// 8
		"Alchemist. Goth friend of forest."								// 9
	};
}

class School
{
	Schools::type type = Schools::type::UNASSIGNED;
	Schools::type alignedTypes[3] =
	{
		Schools::type::UNASSIGNED,
		Schools::type::UNASSIGNED,
		Schools::type::UNASSIGNED
	};
	Schools::type neutralTypes[3] =
	{
		Schools::type::UNASSIGNED,
		Schools::type::UNASSIGNED,
		Schools::type::UNASSIGNED
	};
	Schools::type opposedType = Schools::type::UNASSIGNED;
};

namespace SpellCategories
{
	enum type
	{
		UNASSIGNED,		// 0
		AREA_EFFECT,	// 1
		LINE_OF_SIGHT,	// 2
		OUT_OF_GAME_A,	// 3
		OUT_OF_GAME_B,	// 4
		SELF_ONLY,		// 5
		TOUCH			// 6
	};

	static const std::string typeStrings[] =
	{
		"UNASSIGNED",	// 0
		"AREA_EFFECT",	// 1
		"LINE_OF_SIGHT",// 2
		"OUT_OF_GAME_A",// 3
		"OUT_OF_GAME_B",// 4
		"SELF_ONLY",	// 5
		"TOUCH"			// 6
	};

	static const std::string descriptions[] =
	{
		"N/A",	// 0
		"Spell affects a given area. May (1) affect area around target point \
			OR(2) affect whole table. If generates attack make separate attack \
			roll against each target.",												// 1
		"Spell can be cast on any target that is within line of sight of \
			spellcaster. Includes figure casting spell. Max range: 24\".",			// 2
		"Cannot be cast during game. Can only cast after a game. Wizard and \
			apprentice may each attempt to cast each Out of Game spell once \
			after each game. Cannot be empowered. Damage not taken from failing \
			to cast. No experience earned from casting.",							// 3
		"Cannot be cast during game. Can only cast before a game. Wizard and \
			apprentice may each attempt to cast each Out of Game spell once \
			after each game. Cannot be empowered. Damage not taken from failing \
			to cast. No experience earned from casting.",							// 4
		"Can only affect the igure that cast the spell. Cannot be cast on anyone \
			else.",																	// 5
		"Target must be within 1\" of spellcaster. Spellcasters may cast this on \
			themselves."															// 6			
	};
}



class spell
{
private:
	std::string name = "";
	std::string description = "";
	int baseCastingNumber = -1;
	double baseRollChance = -1;
	SpellCategories::type category = SpellCategories::type::UNASSIGNED;
	Schools::type school = Schools::type::UNASSIGNED;
	bool wizardOnly = false;

public:
	spell(std::string _name, std::string _desc, Schools::type _school, 
		int _roll, SpellCategories::type _category,  bool _wizardOnly)
	{
		assert(_name != "" && "name string is empty");
		name = _name;

		assert(_desc != "" && "description string is empty");
		description = _desc;

		assert(_school != Schools::type::UNASSIGNED && "School is unassigned");
		school = _school;

		assert(_roll > 0 && _roll < 21 && "invalid roll value");
		baseCastingNumber = _roll;
		baseRollChance = (20 - static_cast<double>(baseCastingNumber)) / 20;

		assert(_category != SpellCategories::type::UNASSIGNED && "Category is unassigned");
		category = _category;

		wizardOnly = _wizardOnly;
	}

	std::string getName() { return name; }
	std::string getDescription() { return description; }
	int getRoll() { return baseCastingNumber; }
	Schools::type getSchool() { return school; }
	double getRollChance() { return baseRollChance; }
	SpellCategories::type getCategory() { return category; }
};

static const spell spells =
{
	spell(
		"ABSORB KNOWLEDGE", 
		"Wizard only. This spell allows a wizard to absorb the knowledge from a written		\
		 without having to read it.A wizard immediately gains 40 experience points			\
		for casting this spell to represent the speed with which they can gain knowledge.	\
		This experience does not count against the maximum that can be earned in one		\
		game.This spell may only be cast after a game in which the wizard was not			\
		reduced to 0 Health.",
		Schools::type::SIGILIST,
		12,
		SpellCategories::type::OUT_OF_GAME_A,
		true)

};