#include <stdlib.h>
#include <vector>
#include <string>
#include "Player.h"
using namespace std;
#pragma once

class Region
{
private:
	int num_borders;
	vector<string> borders;
	Player* commander;

public:
	Region(int, vector<string>, Player*);

	
};

