#pragma once
#include <string>


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

	class SchoolAlignment
	{
	public:
		type alignedTypes[3] =
		{
			UNASSIGNED,
			UNASSIGNED,
			UNASSIGNED
		};
		type neutralTypes[5] =
		{
			UNASSIGNED,
			UNASSIGNED,
			UNASSIGNED,
			UNASSIGNED,
			UNASSIGNED
		};
		type opposedType = UNASSIGNED;

		SchoolAlignment(type a1, type a2, type a3, type n1, type n2, type n3, type n4, type n5, type opp) 
		{
			alignedTypes[0] = a1;
			alignedTypes[1] = a2;
			alignedTypes[2] = a3;
			neutralTypes[0] = n1;
			neutralTypes[1] = n2;
			neutralTypes[2] = n3;
			neutralTypes[3] = n4;
			neutralTypes[4] = n5;
			opposedType = opp;
		}
	};

	static const std::string getTypeString(const type _type);
	static const std::string getTypeDescriptions(const type _type);
	static const SchoolAlignment getAlignment(const Schools::type _chosenSchool);
}


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

	static const std::string getTypeString(const SpellCategories::type _chosenCategory);
	static const std::string getTypeDescription(const SpellCategories::type _chosenCategory);
}



