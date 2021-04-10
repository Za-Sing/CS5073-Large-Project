#include "Region.h"

Region::Region(int id, string name, vector<int> border_ids)
{
	this->id = id;
	this->name = name;
	this->border_ids = border_ids;
	troops = 0;
	commander_id = -1;

}

//getters and setters

// ID's, names, and borders will not be changed after initialization
// so they do not have setters (they should also probably be constants).
int Region::getID() const
{
	return this->id;
}

string Region::getName() const
{
	return name;
}

vector<int> Region::getBorder_ids() const
{
	return border_ids;
}

void Region::updateTroops(int numTroops)
{
	this->troops = numTroops;
}

void Region::addTroops(int numTroops)
{
	this->troops += numTroops;
}

int Region::getTroops() const
{
	return troops;
}

void Region::updateCommander_id(int commander_id)
{
	this->commander_id = commander_id;
}

int Region::getCommander_id() const
{
	return commander_id;
}
