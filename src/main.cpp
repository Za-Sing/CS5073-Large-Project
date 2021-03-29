#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Brisk.h"
#include "Player.h"

using namespace std;



void placeTroops(int currentPlayer, vector<Player> *players, Brisk* game)
{
	//calculate player's new troops
	vector<Region> ownedRegions = players->at(currentPlayer).getOwnedRegions();
	int numRegions = ownedRegions.size();
	int newTroops = numRegions / 3;

	// place troops
	printf("It's player %i's turn. You have %i new troops to place.\nInput your troop placement in the form: <Region ID> <# of new troops>.\n", currentPlayer, newTroops);
	string troopPlacement;
	getline(cin, troopPlacement);
	while (getline(cin, troopPlacement) && newTroops != 0) {


		vector<int> splitInt;
		stringstream ss(troopPlacement);
		string element;
		while (getline(ss, element, ' '))
		{
			splitInt.push_back(stoi(element));
		}
		if (game->board.at(splitInt.at(0)).getCommander_id() != currentPlayer)
		{
			printf("You don't control that region.");
			continue;
		}
		if (splitInt.at(1) <= newTroops)
		{
			game->board.at(splitInt.at(0)).addTroops(splitInt.at(1));
			newTroops -= splitInt.at(1);
			printf("%i new troops left to place.\n", newTroops);
		}
		else
		{
			printf("You don't have that many troops to place.\n");
		}

	}
}

int main()
{
	//game setup
	Brisk game = Brisk();
	
	int numPlayers;
	string input;
	printf("Enter a number (3-5) of players.\n");
	getline(cin, input);
	numPlayers = stoi(input);
	// Make sure the range is correct
	while (numPlayers < 3 || numPlayers > 5) {
		printf("Please enter a number between 3 & 5.\n");
		getline(cin, input);
		numPlayers = stoi(input);
	}

	// Initialize Players vector
	vector<Player> players;
	for (int i = 0; i < numPlayers; i++) {
		Player newPlayer = Player(numPlayers);
		players.push_back(newPlayer);
	}

	
	//game.beginningClaim(players);
	// TODO: hard code deck

	//Debug input
	game.board.at(0).updateTroops(10);
	game.board.at(0).updateCommander_id(0);
	game.board.at(1).updateTroops(10);
	game.board.at(1).updateCommander_id(0); 
	game.board.at(2).updateTroops(10);
	game.board.at(2).updateCommander_id(0); 
	game.board.at(3).updateTroops(10);
	game.board.at(3).updateCommander_id(1);
	game.board.at(4).updateTroops(10);
	game.board.at(4).updateCommander_id(1);
	game.board.at(5).updateTroops(10);
	game.board.at(5).updateCommander_id(1);
	game.board.at(6).updateTroops(10);
	game.board.at(6).updateCommander_id(0);
	

    //main game loop
	bool inPlay = true;
	int roundIndex = 0;
	while (inPlay)
	{
		for (int currentPlayer = 0; currentPlayer < numPlayers; currentPlayer++)
		{
			//placeTroops(currentPlayer, &players, &game);
			

			// attack regions
			

			// move troops
			bool inputing = true;
			bool chainExists = false;
			while (inputing)
			{
				string troopMovement;
				printf("Move your troops, Player%i. Enter in the form <Origin Region ID> <Target Region ID> <# of Troops>.\n", currentPlayer);
				//getline(cin, troopMovement);
				getline(cin, troopMovement);
				vector<int> splitInt;
				stringstream ss(troopMovement);
				string element;
				while (getline(ss, element, ' '))
				{
					splitInt.push_back(stoi(element));
				}
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
							printf("coolio.\n");
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
			
		}


		//how to intialize Card
		//Card testy{ TERRITORY::China, TROOP::Artillery };



		++roundIndex; // end of round
	}
}