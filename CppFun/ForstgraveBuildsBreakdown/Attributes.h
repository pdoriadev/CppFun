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


