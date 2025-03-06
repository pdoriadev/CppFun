#pragma once
#include "SpellSystem.h"

static const spell spells[]
{
	spell
	(
		"Absorb Knowledge",
		"Wizard only. This spell allows a wizard to absorb the knowledge from a written		\
		 without having to read it.A wizard immediately gains 40 experience points			\
		for casting this spell to represent the speed with which they can gain knowledge.	\
		This experience does not count against the maximum that can be earned in one		\
		game.This spell may only be cast after a game in which the wizard was not			\
		reduced to 0 Health.",
		Schools::type::SIGILIST,
		12,
		{SpellCategories::type::OUT_OF_GAME_A},
		true
	),
	spell
	(
		"Animal Companion",
		"The spellcaster summons an animal companion of their choice from the				\
		following options to become a permanent member of their warband : bear				\
		(page 179), ice toad(page 180), snow leopard(page 182), or wolf(page 182).			\
		All Animal Companions count as standard soldiers.Animal companions are				\
		more strong - willed than wild examples of their species and receive a permanent	\
		+ 3 Will.A spellcaster may only have one animal companion at any time.",
		Schools::type::WITCH,
		10,
		{ SpellCategories::type::OUT_OF_GAME_B },
		false
	),
	spell
	(
		"Animate Construct",
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
		{ SpellCategories::type::OUT_OF_GAME_B },
		false
	),
	spell
	(
		"Animate Skull",
		"The spellcaster fills a skull with magic malice and throws it at an opponent. Place	\
		one animated skull(page 190) within 6” of the spellcaster.It can be placed				\
		directly into combat.This skull is an uncontrolled creature.The spellcaster may			\
		not cast this spell again until this creature is removed from the table, but may		\
		spend an action to cancel the spell, in which case the animated skull is				\
		immediately removed from the table.",
		Schools::type::NECROMANCER,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Awareness",
		"If this spellcaster is on the table, its warband may add +2 to its Initiative Rolls for	\
		the purposes of determining the primary player only.This bonus stacks so, if both			\
		the wizardand the apprentice have cast this spelland are both on the table, the				\
		player may add +4 to their Initiative Rolls.The maximum possible bonus is +4.				\
		This spell counts as active on the spellcaster during the game and may be					\
		cancelled by anything that cancels spells.",
		Schools::type::SOOTHSAYER,
		12,
		{ SpellCategories::type::OUT_OF_GAME_B },
		false
	),
	spell
	(
		"Banish",
		"All demons within line of sight of the spellcaster must pass an immediate Will			\
		Roll with a Target Number equal to the Casting Roll.If a demon fails the roll and		\
		its current Will is +4 or less, it is immediately reduced to 0 Health and removed		\
		from the table.If its current Will is +5 or higher, it suffers damage equal to three	\
		times the amount by which it failed the Will Roll.",
		Schools::type::THAUMATURGE,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Beauty",
		"",
		Schools::type::ILLUSIONIST,
		10,
		{ SpellCategories::type::SELF_ONLY },
		false
	),
	spell
	(
		"Blinding Light",
		"",
		Schools::type::THAUMATURGE,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Blink",
		"",
		Schools::type::ILLUSIONIST,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Bone Dart",
		"",
		Schools::type::NECROMANCER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Bones of the Earth",
		"",
		Schools::type::NECROMANCER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Brew Potion",
		"",
		Schools::type::WITCH,
		12,
		{ SpellCategories::type::OUT_OF_GAME_B },
		false
	),
	spell
	(
		"Bridge",
		"",
		Schools::SIGILIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Call Storm",
		"",
		Schools::ELEMENTALIST,
		12,
		{ SpellCategories::type::AREA_EFFECT },
		false
	),
	spell
	(
		"Circle of Protection",
		"",
		Schools::THAUMATURGE,
		12,
		{ SpellCategories::type::TOUCH },
		false
	),
	spell
	(
		"Combat Awareness",
		"",
		Schools::SOOTHSAYER,
		12,
		{ SpellCategories::type::TOUCH },
		false
	),
	spell
	(
		"Control Animal",
		"",
		Schools::WITCH,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Control Construct",
		"",
		Schools::ENCHANTER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Control Demon",
		"",
		Schools::SUMMONER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Control Undead",
		"",
		Schools::NECROMANCER,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Crumble",
		"",
		Schools::CHRONOMANCER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Curse",
		"",
		Schools::WITCH,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Decay",
		"",
		Schools::CHRONOMANCER,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Destroy Undead",
		"",
		Schools::THAUMATURGE,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Destructive Sphere",
		"",
		Schools::ELEMENTALIST,
		12,
		{ SpellCategories::type::AREA_EFFECT },
		false
	),
	spell
	(
		"Dispel",
		"",
		Schools::THAUMATURGE,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Draining Word",
		"",
		Schools::SIGILIST,
		14,
		{ SpellCategories::type::AREA_EFFECT },
		false
	),
	spell
	(
		"Elemental Ball",
		"",
		Schools::ELEMENTALIST,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Elemental Bolt",
		"",
		Schools::ELEMENTALIST,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Elemental Hammer",
		"",
		Schools::ELEMENTALIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Elemental Shield",
		"",
		Schools::ELEMENTALIST,
		10,
		{ SpellCategories::type::SELF_ONLY },
		false
	),
	spell
	(
		"Embed Enchantment",
		"",
		Schools::ENCHANTER,
		14,
		{ SpellCategories::type::OUT_OF_GAME_A },
		false
	),
	spell
	(
		"Enchant Armour",
		"",
		Schools::ENCHANTER,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Explosive Rune",
		"",
		Schools::SIGILIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Control Undead",
		"",
		Schools::WITCH,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Familiar",
		"",
		Schools::WITCH,
		10,
		{ SpellCategories::type::OUT_OF_GAME_B },
		false
	),
	spell
	(
		"Fast Act",
		"",
		Schools::CHRONOMANCER,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Fleet Feet",
		"",
		Schools::CHRONOMANCER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Fog",
		"",
		Schools::WITCH,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Fool's Gold",
		"",
		Schools::ILLUSIONIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Furious Quill",
		"",
		Schools::SIGILIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Glow",
		"",
		Schools::ILLUSIONIST,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Grenade",
		"",
		Schools::ENCHANTER,
		10,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Heal",
		"",
		Schools::THAUMATURGE,
		8,
		{ SpellCategories::type::LINE_OF_SIGHT },
		false
	),
	spell
	(
		"Illusionary Soldier",
		"",
		Schools::ILLUSIONIST,
		12,
		{ SpellCategories::type::LINE_OF_SIGHT },
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
		{ SpellCategories::type::SELF_ONLY },
		false
		),


};