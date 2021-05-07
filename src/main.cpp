#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Brisk.h"
#include "Player.h"

using namespace std;

// Helper function to write to a .CSV file for data collection
void writeCSVDouble(string filename, vector<double> vals) {

	// Create an output filestream object
	ofstream file(filename);

	// write to csv
	for (int i = 0; i < vals.size(); i -= -1) {
		file << vals.at(i) << "\n";
	}

	// Close the file
	file.close();
}

int main()
{
	srand(time(NULL));
	
	// Game setup
	Brisk game = Brisk();

	// GA TEST
	GeneticAlgorithm GA = GeneticAlgorithm();
	GA.preEvolveAttack(100, 100, 0.2);
	GA.preEvolvePlacement(100, 100, 0.2);

	// Graph making
	vector<double> player0;
	vector<double> player1;
	vector<double> player2;
	vector<double> player3;
	vector<double> player4;

	// Initial game setup
	string input;
	printf("Enter a number (3-5) of players.\n");
	getline(cin, input);
	int numPlayers = stoi(input);
	printf("Indicate how many will be bots.\n");
	getline(cin, input);
	int numBots = stoi(input);
	printf("Indicate how many bots will be pretrained.\n");
	getline(cin, input);
	int numPretrainedBots = stoi(input);
	// Make sure the range is correct
	while (numPlayers < 3 || numPlayers > 5) {
		printf("Please enter a number between 3 & 5.\n");
		getline(cin, input);
		numPlayers = stoi(input);
	}

	// Initialize Players vector
	vector<Player*>* players = new vector<Player*>();
	for (int i = 0; i < numPlayers; i++) {
		// Initialize GeneticAlgorithm object if there are bots to be created
		if (numBots > 0) {
			Player* newPlayer = new Player(numPlayers, true);
			--numBots;
			// Pre-evolve bots if appropriate
			if (numPretrainedBots > 0) {
				newPlayer->getGA()->preEvolveAttack(100, 100, 0.20);
				newPlayer->getGA()->preEvolvePlacement(100, 100, 0.20);
				--numPretrainedBots;
			}
			players->push_back(newPlayer);
		}
		else {
			Player* newPlayer = new Player(numPlayers, false);
			players->push_back(newPlayer);
		}
	}

	
	game.beginningClaim(players);

    // Main game loop
	bool inPlay = true;
	int roundIndex = 0;
	while (inPlay && roundIndex < 200)
	{
		for (int currentPlayer = 0; currentPlayer < numPlayers; currentPlayer++)
		{
			// Gather data for region number ownership

			switch (currentPlayer) {

			case 0:
				player0.push_back(players->at(currentPlayer)->getOwnedRegions().size());
				writeCSVDouble("Player0.csv", player0);
				printf("CURRENT REGIONS PLAYER 0: %d\n", players->at(currentPlayer)->getOwnedRegions().size());
				break;

			case 1:
				player1.push_back(players->at(currentPlayer)->getOwnedRegions().size());
				writeCSVDouble("Player1.csv", player1);
				printf("CURRENT REGIONS PLAYER 1: %d\n", players->at(currentPlayer)->getOwnedRegions().size());
				break;

			case 2:
				player2.push_back(players->at(currentPlayer)->getOwnedRegions().size());
				writeCSVDouble("Player2.csv", player2);
				printf("CURRENT REGIONS PLAYER 2: %d\n", players->at(currentPlayer)->getOwnedRegions().size());
				break;

			case 3:
				player3.push_back(players->at(currentPlayer)->getOwnedRegions().size());
				writeCSVDouble("Player3.csv", player3);
				printf("CURRENT REGIONS PLAYER 3: %d\n", players->at(currentPlayer)->getOwnedRegions().size());
				break;

			case 4:
				player4.push_back(players->at(currentPlayer)->getOwnedRegions().size());
				writeCSVDouble("Player4.csv", player4);
				printf("CURRENT REGIONS PLAYER 4: %d\n", players->at(currentPlayer)->getOwnedRegions().size());
				break;
			}

			if (!players->at(currentPlayer)->getDefeated()) {

				game.placeTroops(currentPlayer, players);
		  		bool gainedARegion = false;
			

				// Attack regions
				printf("Make an attack? y/n\n");
				string attackResponse;
				if (players->at(currentPlayer)->getGA() != NULL) {
					attackResponse = players->at(currentPlayer)->getGA()->gaPlay(3, currentPlayer, -1, &(game.board));
				}
				else {
					cin >> attackResponse;

				}
				while (attackResponse == "y")
				{

					game.attackSequence(players, currentPlayer, &gainedARegion);
					printf("Make another attack? y/n\n");
					if (players->at(currentPlayer)->getGA() != NULL) {
						attackResponse = players->at(currentPlayer)->getGA()->gaPlay(8, currentPlayer, -1, &(game.board));
					}
					else {
						cin >> attackResponse;

					}
				}

				// Move troops
				bool inputing = true;
				bool chainExists = false;
				while (inputing)
				{
					string troopMovement;
					printf("Move your troops, Player %i. Enter in the form <Origin Region ID> <Target Region ID> <# of Troops>.\n", currentPlayer);
					
					if (players->at(currentPlayer)->getGA() != NULL) 
					{
						troopMovement = players->at(currentPlayer)->getGA()->gaPlay(9, currentPlayer, -1, &(game.board));
					}
					else 
					{
						getline(cin, troopMovement);
					}
					
					vector<int> splitInt;
					stringstream ss(troopMovement);
					string element;
					while (getline(ss, element, ' '))
					{
						splitInt.push_back(stoi(element));
					}
					if (splitInt.at(0) != -1)
					{

						if (game.board.at(splitInt.at(0)).getCommander_id() != currentPlayer || game.board.at(splitInt.at(1)).getCommander_id() != currentPlayer)
						{
							printf("You don't own at least one of those regions.\n");
							continue;
						}
						else
						{

							vector<bool> falses(game.board.size(), false);
							if (game.isChain(splitInt.at(0), splitInt.at(1), currentPlayer, falses))	// check for chain
							{
								if (game.board.at(splitInt.at(0)).getTroops() > splitInt.at(2))
								{
									game.board.at(splitInt.at(0)).addTroops(-splitInt.at(2));
									game.board.at(splitInt.at(1)).addTroops(splitInt.at(2));
									inputing = false;
								}
								else
								{
									printf("You don't have that many troops to move.\n");
									continue;
								}


							}
							else
							{
								printf("You don't have a chain of regions along which to move troops.\n");
								continue;
							}
						}
					}
					else
					{
						inputing = false;
					}
				}
			}
		}


		// How to intialize Card
		// Card testy{ TERRITORY::China, TROOP::Artillery };

		++roundIndex; // End of round
	}
	// De-allocate memory
	for (int i = 0; i < players->size(); ++i) {
		delete players->at(i)->getGA();
		delete players->at(i);
	}
	delete players;
}