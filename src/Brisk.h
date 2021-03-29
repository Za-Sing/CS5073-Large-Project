#ifndef BRISK_H
#define BRISK_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include "Player.h"
#include "Card.h"

class Brisk
{
private:
	const int NUM_REGIONS = 42;
	int numTurns, numPlayers, currentPlayer, regionsLeft, regionChoice, attackFrom, attackTo;
	bool succPlace;
	vector<Card>* deck;
	void initDeck(string filename);
public:
	Brisk();
	void beginningClaim(vector<Player> players);
	bool isChain(int startID, int endID, int currentPlayer, vector<bool> visited);
	void attackSequence(vector<Player> players);
	vector<Region> board;
};

#endif // !BRISK_H