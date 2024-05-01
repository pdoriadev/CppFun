#pragma once


class ItemInterface
{
protected:
	virtual bool UseItem() = 0;
	// empty placeholder class to write soldiers stub
};

class HealthPotion : public::ItemInterface
{
	bool full;

public:
	HealthPotion(bool _full)
	{
		full = _full;
	}

	bool UseItem() 
	{

	}
};