#pragma once

class Attributes
{
	int move = 0;
	int fight = 0;
	int shoot = 0;
	int armor = 0;
	int will = 0;
	int health = 0;
public: 
	Attributes() {}

	Attributes(int M, int F, int S, int A, int W, int H)
	{
		move = M;
		fight = F;
		shoot = S;
		armor = A;
		will = W;
		health = H;
	}
};



class AttributesInstance
{
private:
	Attributes baseAttributes;
	Attributes currentAttributes;

public: 
	AttributesInstance(Attributes _baseAttributes)
	{
		baseAttributes = _baseAttributes;
		currentAttributes = _baseAttributes;
	}

	// put in functions to change current attributes.
};
