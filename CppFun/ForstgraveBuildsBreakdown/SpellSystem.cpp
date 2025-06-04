#include "SpellSystem.h"
#include <vector>
#include <assert.h>


namespace Schools
{
	static const std::string getTypeString(const type _type)
	{
		switch (_type)
		{
		case CHRONOMANCER:
			return "CHRONOMANCER";
		case ELEMENTALIST:
			return "ELEMENTALIST";
		case ENCHANTER:
			return  "ENCHANTER";
		case ILLUSIONIST:
			return "ILLUSIONIST";
		case NECROMANCER:
			return "NECROMANCER";
		case SIGILIST:
			return "SIGILIST";
		case SOOTHSAYER:
			return "SOOTHSAYER";
		case SUMMONER:
			return "SUMMONER";
		case THAUMATURGE:
			return "THAUMATURGE";
		case WITCH:
			return "WITCH";
		default:
			assert("No matching school");
		}
	}

	static const std::string getTypeDescriptions(const type _type)
	{
		switch (_type)
		{
		case CHRONOMANCER:	
			return "Time Wizard. Gamblers and risk-takers.";
		case ELEMENTALIST:	
			return "Avatar-y. Control of four elements. Damage dealer.";
		case ENCHANTER:		
			return "Artifer-y.";
		case ILLUSIONIST:	
			return "Conjurer. Trickster.";
		case NECROMANCER:	
			return "Death Mage";
		case SIGILIST:		
			return "Rune Reader. Scribe. Power from runes.";
		case SOOTHSAYER:	
			return "Seer. Sage of History. Fortune Teller.";
		case SUMMONER:		
			return "Summoner. Summons demons (any creature from another plane)";
		case THAUMATURGE:	
			return "Healer. Support";
		case WITCH:			
			return "Alchemist. Goth friend of forest.";
		default:
			assert("No matching school");
		}
	}


	static const Schools::SchoolAlignment getAlignment(const Schools::type _chosenSchool) 
	{	
		SchoolAlignment alignments[10]
		{
			SchoolAlignment(ELEMENTALIST, NECROMANCER, SOOTHSAYER, ILLUSIONIST, SIGILIST, SUMMONER, THAUMATURGE, WITCH, ENCHANTER),
			SchoolAlignment(CHRONOMANCER, ENCHANTER, SUMMONER, NECROMANCER, SIGILIST, SOOTHSAYER, THAUMATURGE, WITCH, ILLUSIONIST),
			SchoolAlignment(ELEMENTALIST, SIGILIST, WITCH, ILLUSIONIST, NECROMANCER, SOOTHSAYER, SUMMONER, THAUMATURGE, CHRONOMANCER),
			SchoolAlignment(SIGILIST, SOOTHSAYER, THAUMATURGE, CHRONOMANCER, ENCHANTER, NECROMANCER, SUMMONER, WITCH, ELEMENTALIST),
			SchoolAlignment(CHRONOMANCER, SUMMONER, WITCH, ELEMENTALIST, ENCHANTER, ILLUSIONIST, SIGILIST, SOOTHSAYER, THAUMATURGE),
			SchoolAlignment(ENCHANTER, ILLUSIONIST, THAUMATURGE, CHRONOMANCER, ELEMENTALIST, NECROMANCER, SOOTHSAYER, WITCH, SUMMONER),
			SchoolAlignment(CHRONOMANCER, ILLUSIONIST, THAUMATURGE, ELEMENTALIST, ENCHANTER, NECROMANCER, SIGILIST, SUMMONER, WITCH),
			SchoolAlignment(ELEMENTALIST, NECROMANCER, WITCH, CHRONOMANCER, ENCHANTER, ILLUSIONIST, SOOTHSAYER, THAUMATURGE, SIGILIST),
			SchoolAlignment(ILLUSIONIST, SIGILIST, SOOTHSAYER, CHRONOMANCER, ELEMENTALIST, ENCHANTER, SUMMONER, WITCH, NECROMANCER),
			SchoolAlignment(ENCHANTER, NECROMANCER, SUMMONER, CHRONOMANCER, ELEMENTALIST, ILLUSIONIST, SIGILIST, THAUMATURGE, SOOTHSAYER)
		};

		assert(_chosenSchool == UNASSIGNED && "School does not exist.");
		assert((int)_chosenSchool < 1 || (int)_chosenSchool > 10 && "School does not exist");
		
		return alignments[(int)_chosenSchool - 1];
	}
	
}

namespace SpellCategories
{
	static const std::string getTypeString(const SpellCategories::type _chosenCategory)
	{
		switch (_chosenCategory)
		{
		case AREA_EFFECT:
			return "AREA_EFFECT";
		case LINE_OF_SIGHT:
			return "LINE_OF_SIGHT";
		case OUT_OF_GAME_A:
			return "OUT_OF_GAME_A";
		case OUT_OF_GAME_B:
			return "OUT_OF_GAME_B";
		case SELF_ONLY:
			return "SELF_ONLY";
		case TOUCH:
			return "TOUCH";
		default:
			assert("No matching category");
		}
	}
	static const std::string getTypeDescription(const SpellCategories::type _chosenCategory)
	{
		switch (_chosenCategory)
		{
		case AREA_EFFECT:
			return "Spell affects a given area.May(1) affect area around target point \
				OR(2) affect whole table.If generates attack make separate attack \
				roll against each target.";
		case LINE_OF_SIGHT:
			return "Spell can be cast on any target that is within line of sight of \
				spellcaster.Includes figure casting spell.Max range : 24.";
		case OUT_OF_GAME_A:
			return "Cannot be cast during game.Can only cast after a game.Wizard and \
				apprentice may each attempt to cast each Out of Game spell once \
				after each game.Cannot be empowered.Damage not taken from failing \
				to cast.No experience earned from casting.";
		case OUT_OF_GAME_B:
			return "Cannot be cast during game.Can only cast before a game.Wizard and \
				apprentice may each attempt to cast each Out of Game spell once \
				after each game.Cannot be empowered.Damage not taken from failing \
				to cast.No experience earned from casting.";
		case SELF_ONLY:
			return "Can only affect the figure that cast the spell. Cannot be cast on anyone else.";
		case TOUCH:
			return "Target must be within 1\" of spellcaster. Spellcasters may cast this on themselves.";
		default:
			assert("No matching category");
		}
	};
	
}

namespace Spells
{
	std::vector<std::string> names(64);
	std::vector<std::string> descriptions(64);
	std::vector<int> baseCastingNumber(64);
	std::vector<int> baseRollChance(64);
	// Enums initialized at namespace scope are initialized to zero.
	//		https://stackoverflow.com/questions/6842799/enum-variable-default-value
	std::vector<Schools::type> school(64);
	std::vector<SpellCategories::type> categories(64);
	std::vector<bool> wizardOnly(64);
}

