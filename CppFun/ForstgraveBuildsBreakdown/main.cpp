/*
Program Flow (console version OR make this in QT as learning exercise for views??):
1) Create a wizard

	Wizard Creation
		- School selection
			- ability to browse schools and spells
				- how are they shown
			- after selecting a school, spells are listed in terms of alignment
				(i.e. school's spells first, then aligned, then neutral, then opposed.)
			- ability to go back and "undo any selection"
		- Be able to select spells via number input.
	Receive percentage breakdown and use cases for each spell.


// min/max
// compare stats and values
// web app

	
2) Create a party (wizard + warband)





	Warband creation
*/

#include <iostream>
#include <fstream>
#include "json.h"
#include "SpellSystem.h"

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
	std::ifstream file("Spells.json");
	nlohmann::json jsonData;
	file >> jsonData;  // Read JSON file

	// Iterate over JSON array
	//for (const auto& entry : jsonData) {
	//	std::cout << "Name: " << entry["name"] << "\n";
	//	std::cout << "Age: " << entry["age"] << "\n";
	//	std::cout << "Student: " << (entry["is_student"] ? "Yes" : "No") << "\n";
	//	std::cout << "-----------------\n";
	//}

	const Schools::SchoolAlignment alignment = Schools::getAlignment(Schools::CHRONOMANCER);
	for (int i = 0; i < 3; i++)
	{

	}




	return 0;

};


