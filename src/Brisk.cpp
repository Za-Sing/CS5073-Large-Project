/*
* Brisk is for encapsulating the main game loop. In it, there is the initial pre-game setup for territory claiming
* and reinforcing. It is also used for accessing game data at will, and for updating the status of the game.
*/
#include "Brisk.h"

using namespace std;

// Initialize the card deck from a file
void Brisk::initDeck(string filename) {
	ifstream file;
	file.open(filename);
	string line;
	deck = new vector<Card>();
	// Read through the file
	while (getline(file, line)) {
		Card card;
		// Read in the enum property for territory
		card.territory = static_cast<TERRITORY>(stoi(line));
		// Read next line
		if (!getline(file, line))
			break;
		// Assign troop type
		card.troop = static_cast<TROOP>(stoi(line));
		// Add card to deck
		deck->push_back(card);
	}
	// Shuffle the deck now that it is initialized
	random_shuffle(deck->begin(), deck->end());
	file.close();
}

Brisk::Brisk()
{
	vector<int> test;
	//initialize board (vector of regions)
	board.push_back(Region(0, "Alaska", vector<int>{1, 3}));
	board.push_back(Region(1, "Northwest_Territory", vector<int>{0, 3, 4, 2}));
	board.push_back(Region(2, "Greenland", vector<int>{1, 4, 5, 13}));
	board.push_back(Region(3, "Alberta", vector<int>{0, 1, 4, 6}));
	board.push_back(Region(4, "Ontario", vector<int>{1, 2, 5, 7, 6, 3}));
	board.push_back(Region(5, "Eastern_Canada", vector<int>{4, 2, 7}));
	board.push_back(Region(6, "Western_United_States", vector<int>{3, 4, 7, 8}));
	board.push_back(Region(7, "Eastern_United_States", vector<int>{6, 4, 5, 8}));
	board.push_back(Region(8, "Central_America", vector<int>{6, 7}));
	board.push_back(Region(9, "Venezuela", vector<int>{})); 
	board.push_back(Region(10, "Brazil", vector<int>{})); 
	board.push_back(Region(11, "Peru", vector<int>{}));
	board.push_back(Region(12, "Argentina", vector<int>{})); 
	board.push_back(Region(13, "Iceland", vector<int>{})); 
	board.push_back(Region(14, "Scandinavia", vector<int>{})); 
	board.push_back(Region(15, "Russia", vector<int>{})); 
	board.push_back(Region(16, "Great_Britain", vector<int>{})); 
	board.push_back(Region(17, "Northern_Europe", vector<int>{}));
	board.push_back(Region(18, "Western_Europe", vector<int>{})); 
	board.push_back(Region(19, "Southern_Europe", vector<int>{})); 
	board.push_back(Region(20, "Ural", vector<int>{})); 
	board.push_back(Region(21, "Siberia", vector<int>{})); 
	board.push_back(Region(22, "Yakutsk", vector<int>{})); 
	board.push_back(Region(23, "Irkutsk", vector<int>{})); 
	board.push_back(Region(24, "Kamchatka", vector<int>{}));
	board.push_back(Region(25, "Afghanistan", vector<int>{})); 
	board.push_back(Region(26, "China", vector<int>{})); 
	board.push_back(Region(27, "Mongolia", vector<int>{})); 
	board.push_back(Region(28, "Japan", vector<int>{})); 
	board.push_back(Region(29, "Middle_East", vector<int>{})); 
	board.push_back(Region(30, "India", vector<int>{})); 
	board.push_back(Region(31, "Southeast_Asia", vector<int>{}));
	board.push_back(Region(32, "North_Africa", vector<int>{})); 
	board.push_back(Region(33, "Egypt", vector<int>{})); 
	board.push_back(Region(34, "East_Africa", vector<int>{})); 
	board.push_back(Region(35, "Central_Africa", vector<int>{})); 
	board.push_back(Region(36, "South_Africa", vector<int>{})); 
	board.push_back(Region(37, "Madagascar", vector<int>{}));
	board.push_back(Region(38, "Indonesia", vector<int>{})); 
	board.push_back(Region(39, "New_Guinea", vector<int>{})); 
	board.push_back(Region(40, "Western_Australia", vector<int>{})); 
	board.push_back(Region(41, "Eastern_Australia", vector<int>{}));



}

void Brisk::beginningClaim(vector<Player> players) {

	numPlayers = players.size();

	//find number of turns total after regions are chosen
	switch (numPlayers)
	{
	case 2: numTurns = 40; break;
	case 3: numTurns = 35; break;
	case 4: numTurns = 30; break;
	case 5: numTurns = 25; break;
	}

	regionsLeft = 42;

	//claim new territories until territories gone

	//go for the number of regions left to choose from
	for (int i = 0; i < numTurns; i-=-1) {
		//cycle through players
		currentPlayer = (i % numPlayers);

		//if empty regions exist
		if (regionsLeft != 0) {
			//print and take player's region choice
			printf("Player %s, please choose next region ID for region to occupy.\n", to_string(currentPlayer + 1));
			cin >> regionChoice;

			// TODONE: once region is implemented, update each region to be owned by the player that chooses it
			// make sure the region is removed from the selectable pool of regions

			vector<Region> currentRegions = players[currentPlayer].getOwnedRegions();
			currentRegions.push_back(board[regionChoice]);
			board[regionChoice].addTroops(1);
			board[regionChoice].updateCommander_id(currentPlayer);
			players[currentPlayer].updateOwnedRegions(currentRegions);

			//remove troop from player's troop count
			players[currentPlayer].updateArmySize(players[currentPlayer].getTotalArmySize() - 1);

			//remove region left
			regionsLeft -= 1;
		}
		//else if there are no regions left
		else {
			//if the player has troops left to place
			if (players[currentPlayer].getTotalArmySize() > 0) {

				printf("Player %s, please add a troop to one of your owned regions.\n", to_string(currentPlayer + 1));

				// TODONE: once region is implemented, update each region the player puts a troop on to.

				succPlace = false;
				while (succPlace == false) {

					cin >> regionChoice;

					// check that they own the region
					if (board[regionChoice].getCommander_id() == currentPlayer) {
						board[regionChoice].addTroops(1);
						succPlace = true;
					}
					else {
						cout << "Invalid choice! Please choose a region you own.";
					}
				}

				//remove troop from player's troop count
				players[currentPlayer].updateArmySize(players[currentPlayer].getTotalArmySize() - 1);
			}
		}
	}
}

// TODO: implement game data access methods

// TODO: implement game status update methods