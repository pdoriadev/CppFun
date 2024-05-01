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

static const spell spells []
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
		true),
	spell(
		"ANIMAL COMPANION",
		"The spellcaster summons an animal companion of their choice from the				\
		following options to become a permanent member of their warband : bear				\
		(page 179), ice toad(page 180), snow leopard(page 182), or wolf(page 182).			\
		All Animal Companions count as standard soldiers.Animal companions are				\
		more strong - willed than wild examples of their species and receive a permanent	\
		+ 3 Will.A spellcaster may only have one animal companion at any time.",
		Schools::type::WITCH,
		10,
		SpellCategories::type::OUT_OF_GAME_B,
		false
		),
	spell(
		"ANIMATE CONSTRUCT",
		"It is assumed that the spellcaster has built a construct prior to using this spell to	\
		animate it.If the spell is successfully cast, the construct immediately becomes a		\
		permanent member of the warband, taking the place of a soldier.A spellcaster			\
		must declare the size of construct they are attempting to animate(small, medium			\
		or large – page 183) before rolling to cast the spell.The larger the construct, the		\
		harder it is to animate, so the following modifiers are applied to the Casting Roll :	\
		Small - 0, Medium - 3, Large - 6. There is no limit to the number of constructs in		\
		a warband other than the normal limits for soldiers.Large constructs count as			\
		specialist soldiers, the others as standard soldiers.",
		Schools::type::ENCHANTER,
		10,
		SpellCategories::type::OUT_OF_GAME_B,
		false
		),
	spell(
		"ANIMATE SKULL",
		"The spellcaster fills a skull with magic malice and throws it at an opponent. Place	\
		one animated skull(page 190) within 6” of the spellcaster.It can be placed				\
		directly into combat.This skull is an uncontrolled creature.The spellcaster may			\
		not cast this spell again until this creature is removed from the table, but may		\
		spend an action to cancel the spell, in which case the animated skull is				\
		immediately removed from the table.",
		Schools::type::NECROMANCER,
		8,
		SpellCategories::LINE_OF_SIGHT,
		false
		),
	spell(
		"AWARENESS",
		"If this spellcaster is on the table, its warband may add +2 to its Initiative Rolls for	\
		the purposes of determining the primary player only.This bonus stacks so, if both			\
		the wizardand the apprentice have cast this spelland are both on the table, the				\
		player may add + 4 to their Initiative Rolls.The maximum possible bonus is + 4.				\
		This spell counts as active on the spellcaster during the game and may be					\
		cancelled by anything that cancels spells.",
		Schools::type::SOOTHSAYER,
		12,
		SpellCategories::OUT_OF_GAME_B,
		false
		),
	spell(
		"BANISH",
		"All demons within line of sight of the spellcaster must pass an immediate Will			\
		Roll with a Target Number equal to the Casting Roll.If a demon fails the roll and		\
		its current Will is + 4 or less, it is immediately reduced to 0 Health and removed		\
		from the table.If its current Will is + 5 or higher, it suffers damage equal to three	\
		times the amount by which it failed the Will Roll.",
		Schools::type::THAUMATURGE,
		10,
		SpellCategories::LINE_OF_SIGHT,
		false
		),
	spell(
		"ILLUSIONIST",
		"This spell causes anyone who looks on the spellcaster to see a paragon of beauty.		\
		Any member of an opposing warband must make a Will Roll with a Target					\
		Number equal to the Casting Roll if they wish to do any of the following : move			\
		combat with the spellcaster, make a shooting attack that could potentially				\
		hit the spellcaster(including shooting attacks generated by spells), or cast any		\
		spell that targets the spellcaster.Spellcasters may empower this Will Roll in the		\
		same way they would to resist a spell.A figure may only attempt such a Will Roll		\
		once per turn.This spell has no effect on creatures(anything found in Chapter			\
		Six : Bestiary, page 176) or war hounds.",
		Schools::type::ILLUSIONIST,
		10,
		SpellCategories::type::SELF_ONLY,
		false
		)

};