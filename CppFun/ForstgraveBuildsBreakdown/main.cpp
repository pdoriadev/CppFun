/*
Be able to select spells via number input.
Receive percentage breakdown and use cases for each spell

*/

#include <iostream>
#include <fstream>
#include "Spells.h"
#include "Attributes.h"

// Should include:
	// list of spells
	// starting stats (going to assume all levels created here are level 0 )
	// school
// Later:
	// apprentice
	// warband
class wizard
{
private:
	std::vector<spell> spells;
	Schools::type school = Schools::type::UNASSIGNED;
	AttributesInstance attributes;

public:
	wizard(Schools::type school)
	{
		assert(school != Schools::type::UNASSIGNED && "Must provide valid school");
		school = school;
	}

	

};


// print spells to console (organized in different ways)
	// alphabetical
	// by schools
	// by relationship to a school (the school, aligned, neutral, opposed)
	// 
// print spells to console for selection
// 
// output selected spells 
// 
// output spells + spell descriptions to file

// spell selection + confirmation
	// wizard creation requires 8 spells

int main()
{

};


