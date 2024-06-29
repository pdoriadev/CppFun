#pragma once
#include "Spells.h"
#include "Attributes.h"




// Should include:
	// list of spells
	// starting stats (going to assume all levels created here are level 0 )
	// school
// Later:
	// apprentice
	// warband
namespace Spellcasters
{
	class wizardInstance
	{

	};

	class wizard
	{
	protected:
		std::vector<spell> spells;
		Schools::type school = Schools::type::UNASSIGNED;
		Attributes attributes;

	public:
		wizard(Schools::type _school, std::vector<spell> _spells, Att)
		{
			assert(_school != Schools::type::UNASSIGNED && "Must provide valid school");
			school = _school;
		}



	};
}