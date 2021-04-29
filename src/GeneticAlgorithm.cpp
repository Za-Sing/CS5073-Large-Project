#include "GeneticAlgorithm.h"

using namespace std;

// Function for random dice rolls
int GeneticAlgorithm::rollDie()
{
	int roll;
	roll = rand() % 6 + 1; //six sided die roll
	return roll;
}

// Function to write to a .CSV file
void writeCSVDouble(string filename, vector<double> vals, string dataName){
    
		// Create an output filestream object
	ofstream file(filename);

	// data designation marker
	file << dataName << "\n";

	// write to csv
	for (int i = 0; i < vals.size(); i-=-1) {
		file << vals.at(i) << "\n";
	}

	// Close the file
	file.close();
}

// Helper function to check if a number is divisible by four
bool GeneticAlgorithm::divByFour(string s) {
	int len = s.length();
	// If there is single digit
	if (len == 1)
		return ((s[0] - '0') % 4 == 0);
	// If number formed by last two digits is divisible by 4.
	int last = s[len - 1] - '0';
	int secondLast = s[len - 2] - '0';
	return ((secondLast * 10 + last) % 4 == 0);
}

// Helper function to aid sorting a 2D vector by column
bool sortcol(const vector<double>& v1, const vector<double>& v2) {
	return v1[2] < v2[2];
}

// Helper function to check if a player gets a continent bonus
int GeneticAlgorithm::continentBonus(vector<Region> ownedRegions)
{
	int numRegions = ownedRegions.size();
	int newTroops = 0;

	// Check if player owns North America
	int matches = 0;
	for (int i = 0; i < NORTH_AMERICA.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == NORTH_AMERICA.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == NORTH_AMERICA.size()) {
		newTroops += 5;
	}

	// Check if player owns South America
	matches = 0;
	for (int i = 0; i < SOUTH_AMERICA.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == SOUTH_AMERICA.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == SOUTH_AMERICA.size()) {
		newTroops += 2;
	}

	// Check if player owns Europe
	matches = 0;
	for (int i = 0; i < EUROPE.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == EUROPE.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == EUROPE.size()) {
		newTroops += 5;
	}

	// Check if player owns Asia
	matches = 0;
	for (int i = 0; i < ASIA.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == ASIA.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == ASIA.size()) {
		newTroops += 7;
	}

	// Check if player owns Africa
	matches = 0;
	for (int i = 0; i < AFRICA.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == AFRICA.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == AFRICA.size()) {
		newTroops += 3;
	}

	// Check if player owns Australia
	matches = 0;
	for (int i = 0; i < AUSTRALIA.size(); ++i) {
		for (int j = 0; j < numRegions; ++j) {
			if (ownedRegions[j].getID() == AUSTRALIA.at(i).getID()) {
				++matches;
			}
		}
	}
	if (matches == AUSTRALIA.size()) {
		newTroops += 2;
	}

	return newTroops;
}

// Function to check all possible home/enemy region pairs for an attack 
// and find the best option based on evolved parameters.
void GeneticAlgorithm::findBestAttack(int currentPlayer, vector<Region> board) 
{
	// First step is make a vector that holds all the owned regions
	vector<Region> myRegions;
	for (int i = 0; i < board.size(); ++i) {
		if (board.at(i).getCommander_id() == currentPlayer) {
			myRegions.push_back(board.at(i));
		}
	}
	// Now we look to find the best attackability score by looking through all possible region attacking combos
	double bestAttackability = 0;
	for (int i = 0; i < board.size(); ++i) {
		for (int j = 0; j < myRegions.size(); ++j) {
			if (board.at(i).getCommander_id() != currentPlayer) {
				// If this region borders one of the owned regions
				if (count(board.at(i).getBorder_ids().begin(), board.at(i).getBorder_ids().end(), myRegions.at(j).getID()) == 1) {
					// Compute the attackability score of this attack possibility
					double attackability = ((double)myRegions.at(j).getTroops() / (double)board.at(i).getTroops()) * troopRatioWeight;
					// Check if winning this region would award a continent bonus.
					double curBonus = continentBonus(myRegions);
					vector<Region> temp = myRegions;
					temp.push_back(board.at(i));
					double possibleBonus = continentBonus(temp);
					if (possibleBonus > curBonus) {
						attackability + contBonusWeight;
					}
					// Now check if this is the best score and update the region choices appropriately
					if (attackability > bestAttackability) {
						bestAttackability = attackability;
						regionToAttack = board.at(i).getID();
						regionFromAttack = myRegions.at(j).getID();
					}
				}
			}
		}
	}
}

// Constructor, randomizes all decision factors
GeneticAlgorithm::GeneticAlgorithm()
{
	// Start troop weights between 0.0 and 1.0
	troopRatioWeight = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0));
	contBonusWeight = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0));
}


// Function to pre-evolve the ATTACK decision parameters.
// NOTE: population size MUST be divisible by 4 in order for proper selection, cloning and mutation to occur.
void GeneticAlgorithm::preEvolveAttack(int generations, int popSize, double mutationProb)
{
	isRandom = false;
	srand(time(NULL));
	
	// Lots of temp vars
	bool attackWon = false, firstGen = true;
	// First col is troopWeight, second col is contWeight. Last column in these vectors will be the fitness score for the (whole) individual.
	vector<vector<double>> weightVals(popSize, vector<double>(3, 0)), bestWeightVals(popSize / 4, vector<double>(3, 0));
	// First column is win bool, second column is troops used / troops returned
	vector<vector<double>> results(popSize, vector<double>(2, 0));  
	// A vector to hold the top ten best values over all generations
	vector<vector<double>> globalBest(generations, vector<double>(3, 0));
	// A vector to track the average fitness of each generation
	vector<double> avgGlobalFitness(generations, 0);

	// Create a variety of Region pairs on which to train. trainingRegions[i][0] is the owned Region, trainingRegions[i][1] is the enemy Region
	vector<vector<Region>> trainingRegions(1000, vector<Region>(2, Region(0, "Alaska", vector<int>{1, 3, 24})));
	for (int i = 0; i < 1000; ++i) {
		trainingRegions[i][0] = Region(0, "Alaska", vector<int>{1, 3, 24});
		trainingRegions[i][0].addTroops(rand() % (35 + 1) + 2);
		trainingRegions[i][1] = Region(1, "Northwest_Territory", vector<int>{0, 3, 4, 2});
		trainingRegions[i][1].addTroops(rand() % (35 + 1) + 1);
	}

	// Will run for g generations
	for (int g = 0; g < generations; ++g) {
		printf("/*****GENERATION %d*****\\\n", g);
		// Create the initial population
		if (firstGen) {
			for (int i = 0; i < popSize; ++i) {
				weightVals[i][0] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0));
				weightVals[i][1] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0));	
			}
		}

		// Attack with current parameters
		vector<double> temp = vector<double>();
		for (int i = 0; i < popSize; ++i) {
			temp = gaAttack(trainingRegions.at(rand() % (trainingRegions.size())).at(0), trainingRegions.at(rand() % (trainingRegions.size())).at(1), 
							weightVals[i][0], weightVals[i][1]);  
			results[i][0] = temp[0];
			results[i][1] = temp[1];
		}

		// Calculate fitness. Fitness is determined by 1 - (return troops / sent troops)
		for (int i = 0; i < popSize; ++i) {
			// If the attack was won
			if (results[i][0] == 1.0) {
				weightVals[i][2] = results[i][1];
			}
			if (results[i][0] == 0.0) {
				weightVals[i][2] = 0; 
			}
		}

		// DEBUG
		printf("Population and fitness: \n");
		for (int i = 0; i < popSize; ++i) {
			printf("%d: %f %f %f\n", i, weightVals[i][0], weightVals[i][1], weightVals[i][2]);
		}

		// Average the fitness of the population per gen
		double fitSum = 0;
		for (int i = 0; i < popSize; ++i) {
			fitSum += weightVals[i][2];
		}
		avgGlobalFitness[g] = (fitSum / popSize);

		// Select the best 25% of the population
		// Sort the population based on the whole individual's fitness
		sort(weightVals.begin(), weightVals.end(), sortcol);
		for (int i = 0; i < popSize / 4; ++i) {
			bestWeightVals[i][0] = weightVals[popSize - 1 - i][0];
			bestWeightVals[i][1] = weightVals[popSize - 1 - i][1];
			bestWeightVals[i][2] = weightVals[popSize - 1 - i][2];
		}

		// Collect the best result from each generation
		globalBest[g][0] = bestWeightVals[0][0];
		globalBest[g][1] = bestWeightVals[0][1];
		globalBest[g][2] = bestWeightVals[0][2];

		// DEBUG
		/*printf("Best 25%: \n");
		for (int i = 0; i < popSize / 4; ++i) {
			printf("%d: %f %f\n", i, bestWeightVals[i][0], bestWeightVals[i][1]);
		}*/

		// Perform cloning on best 25% of the population
		int h = 0;
		for (int i = 0; i < popSize / 4; ++i) {
			// Each value will be cloned 4 times
			weightVals[i + h][0] = bestWeightVals[i][0];
			weightVals[i + h][1] = bestWeightVals[i][1];
			weightVals[i + h][2] = bestWeightVals[i][2]; ++h;

			weightVals[i + h][0] = bestWeightVals[i][0];
			weightVals[i + h][1] = bestWeightVals[i][1]; 
			weightVals[i + h][2] = bestWeightVals[i][2]; ++h;
			
			weightVals[i + h][0] = bestWeightVals[i][0];
			weightVals[i + h][1] = bestWeightVals[i][1]; 
			weightVals[i + h][2] = bestWeightVals[i][2]; ++h;
			
			weightVals[i + h][0] = bestWeightVals[i][0];
			weightVals[i + h][1] = bestWeightVals[i][1];
			weightVals[i + h][2] = bestWeightVals[i][2];
		}

		// DEBUG
		/*printf("Cloned population\n");
		for (int i = 0; i < popSize; ++i) {
			printf("%d: %f %f\n", i, weightVals[i][0], weightVals[i][1]);
		}*/

		// Perform mutation on all but the parents
		for (int i = 0; i < popSize; ++i) {
			// Skip the 25% of the population that is the parents.
			if (divByFour(to_string(i + 4))) {
				continue;
			}
			if ((static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0)) <= mutationProb)) {
				// Adds a random number between [-0.1, 0.1]
				double randVal = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 0.1 - 0.1));
				if (weightVals[i][0] + randVal >= 0.0 && weightVals[i][0] + randVal <= 1.0) {
					weightVals[i][0] += randVal;
				}
			}
			if ((static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0)) <= mutationProb)) {
				// Adds a random number between [-0.1, 0.1]
				double randVal = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 0.1 - 0.1));
				if (weightVals[i][1] + randVal >= 0.0 && weightVals[i][1] + randVal <= 1.0) {
					weightVals[i][1] += randVal;
				}
			}
		}

		// DEBUG
		/*printf("Mutated population\n");
		for (int i = 0; i < popSize; ++i) {
			printf("%d: %f %f\n", i, weightVals[i][0], weightVals[i][1]);
		}*/
		printf("\\**********************/\n");
	}

	// Set the GA parameters by averaging the best 25% of the last generation
	double weight1Sum = 0, weight2Sum = 0;
	for (int i = 0; i < popSize / 4; ++i) {
		weight1Sum += bestWeightVals[i][0];
		weight2Sum += bestWeightVals[i][1];
	}
	troopRatioWeight = (weight1Sum / (popSize / 4));
	contBonusWeight = (weight2Sum / (popSize / 4));

	//DEBUG
	printf("GLOBAL BEST FROM EACH GEN: \n");
	for (int i = 0; i < generations; ++i) {
		printf("%d: %f %f %f\n", i, globalBest[i][0], globalBest[i][1], globalBest[i][2]);
	}

	//DEBUG
	printf("AVG FITNESS FROM EACH GEN: \n");
	for (int i = 0; i < generations; ++i) {
		printf("%d: %f\n", i, avgGlobalFitness[i]);
	}

	//DEBUG
	//writeCSVDouble("aveGlobalFitness.csv", avgGlobalFitness, "avgGlobalFitness");
}


// This function is for use in pre-training. It simulates an attack sequence and then returns a boolean and the ratio of troops lost
vector<double> GeneticAlgorithm::gaAttack(Region ownRegion, Region enemyRegion, double troopRatioWeight, double contBonusWeight)
{
	bool bonus = false, attackWon = false;
	double attackability = 0, troopLostRatio = 0, continueProb = 0.7;
	vector<double> returnValue(2, 0);
	// Randomly set troop numbers and whether or not a successful attack would complete a cont bonus
	int ownTroops = ownRegion.getTroops(), enemyTroops = enemyRegion.getTroops();
	int attack[3];
	int defend[2];
	if ((static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0)) <= 0.5)) {
		bonus = true;
	}

	// Calculate attackability score
	attackability = ((double) ownTroops / (double)enemyTroops) * troopRatioWeight;
	if (bonus) {
		attackability + contBonusWeight;
	}
	printf("Attackability: %f\n", attackability);


	int ogOwnTroops = ownTroops;
	int ogEnemyTroops = enemyTroops;
	// Decide if an attack will take place
	if (attackability >= 1.0) {
		while(ownTroops > 1 && enemyTroops > 0){

			//initialize empty dice
			for (int i = 0; i < 2; i++) {
				attack[i] = 0;
				defend[i] = 0;
			}
			attack[2] = 0;

			int attackingTroops = 3;

			if (ownTroops == 3) {
				attackingTroops = 2;
			}
			else if (ownTroops == 2) {
				attackingTroops = 1;
			}


			int defendingTroops = 2;

			if (enemyTroops == 1) {
				defendingTroops = 1;
			}

			// Get attacker dice rolls
			for (int i = 0; i < attackingTroops; i++) {
				attack[i] = rollDie();
			}

			// Get defender dice rolls
			for (int i = 0; i < defendingTroops; i++) {
				defend[i] = rollDie();
			}

			//compare max dice

			int attackLoss = 0;
			int defendLoss = 0;

			int n = sizeof(attack) / sizeof(attack[0]);
			int m = sizeof(defend) / sizeof(defend[0]);

			sort(attack, attack + n);
			sort(defend, defend + m);

			if (attack[2] > defend[1]) {
				defendLoss++;
				enemyTroops--;
			}
			else {
				attackLoss++;
				ownTroops--;
			}

			if (attackingTroops == 2 || attackingTroops == 3) {
				if (defendingTroops == 2) {
					if (attack[1] > defend[0]) {
						defendLoss++;
						enemyTroops--;
					}
					else {
						attackLoss++;
						ownTroops--;
					}
				}
			}

		}

		//pushback return value with 1 if win, 0 if loss
		if (enemyTroops == 0) {
			returnValue[0] = 1.0;
		}
		else {
			returnValue[0] = 0.0;
		}

		//pushback return value with remaining troops / original troops
		returnValue[1] = (double) ownTroops / (double) ogOwnTroops;

		return returnValue;
	}
	else {
		return { -1.0, -1.0 };
	}
}

/*
* Method for getting GA decision for every game input. returns string to 
* be used inplace of terminal input. Gamestate is used to determine which 
* game input is being requested.
* 
*/
string GeneticAlgorithm::gaPlay(int gameState, int currentPlayer, int newTroops, vector<Region> board)
{
	string x;
	switch (gameState)
	{
	case(0):		//BeginningClaim: choose region to occupy
	{
		vector<Region> availableRegions;
		for (int i = 0; i < board.size(); i++)
		{
			if (board.at(i).getCommander_id() == 99)
			{
				availableRegions.push_back(board.at(i));
			}
		}
		int pick = rand() % availableRegions.size();
		return to_string(availableRegions[pick].getID());
		break;
	}
	case(1):		//BeginningClaim: place a troop in a region
	{
		vector<Region> myRegions;
		for (int i = 0; i < board.size(); i++)
		{
			if (board.at(i).getCommander_id() == currentPlayer)
			{
				myRegions.push_back(board.at(i));
			}
		}
		int pick = rand() % myRegions.size();
		return to_string(myRegions[pick].getID());
		break;
	}
	case(2):	//placeTroops
	{
		string troopPlacement;
		vector<Region> myRegions;
		for (int i = 0; i < board.size(); i++)
		{
			if (board.at(i).getCommander_id() == currentPlayer)
			{
				myRegions.push_back(board.at(i));
			}
		}
		int pick = rand() % myRegions.size();
		troopPlacement += to_string(myRegions[pick].getID());
		int numTroops = rand() % (newTroops + 1);
		troopPlacement += " ";
		troopPlacement += to_string(numTroops);
		return troopPlacement;
		break;
	}
	case(3):
	{
		if ((static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0)) <= 0.85)) {
			return "y";
		}
		else {
			return "n";
		}
		break;
	}
	case(4):		// Attack Sequence: pick Region to attack
	{
		if (isRandom) // If the bot is meant to operate completely randomly
		{
			vector<Region> myRegions;
			for (int i = 0; i < board.size(); i++)
			{
				if (board.at(i).getCommander_id() == currentPlayer)
				{
					myRegions.push_back(board.at(i));
				}
			}
			vector<int> eligibleRegions;
			for (int i = 0; i < board.size(); i++)
			{
				for (int j = 0; j < myRegions.size(); ++j) {
					if (board.at(i).getCommander_id() != myRegions.at(j).getCommander_id()) {
						for (int k = 0; k < myRegions.at(j).getBorder_ids().size(); ++k) {
							// Add each eligible region to the vector if it is not already there
							if (count(board.at(i).getBorder_ids().begin(), board.at(i).getBorder_ids().end(), myRegions.at(j).getBorder_ids().at(k)) == 1
								&& count(eligibleRegions.begin(), eligibleRegions.end(), myRegions.at(j).getBorder_ids().at(k)) == 0) {
								eligibleRegions.push_back(myRegions.at(j).getBorder_ids().at(k));
							}
						}
					}
				}
			}
			// Now choose which of these to attack:
			regionToAttack = eligibleRegions.at(rand() % (eligibleRegions.size()));
			return to_string(regionToAttack);
		}
		// The Brisk attackSequence() first chooses region to attack, and then region from which to attack.
		// However, the GA will work best if it rates the attackability of all possible home/enemy region combos 
		// and then picks the best one. To account for this inconsistency, there is a member var to hold both regions chosen.
		// They will be returned here in the corresponding switch cases.
		else 
		{
			findBestAttack(currentPlayer, board);
			return to_string(regionToAttack);
		}
		break;
	}
	case(5):		// Attack Sequence: choose the Region from which to attack:
	{
		if (isRandom) 
		{
			vector<Region> myRegions;
			for (int i = 0; i < board.size(); i++)
			{
				if (board.at(i).getCommander_id() == currentPlayer)
				{
					myRegions.push_back(board.at(i));
				}
			}
			vector<int> eligibleRegions;
			for (int i = 0; i < myRegions.size(); i++)
			{
				// Add each eligible region to the vector if it is not already there
				if (count(myRegions.at(i).getBorder_ids().begin(), myRegions.at(i).getBorder_ids().end(), regionToAttack) == 1
					&& count(eligibleRegions.begin(), eligibleRegions.end(), myRegions.at(i).getID()) == 0) {
					eligibleRegions.push_back(myRegions.at(i).getID());
				}
			}
			// Now choose which of these to attack:
			return to_string(eligibleRegions.at(rand() % (eligibleRegions.size())));
		}
		// As discussed above, this returns the already-determined home region
		else 
		{
			return to_string(regionFromAttack);
		}
		break;
	}
	case(6):		// Attack Sequence: choose number of troops to attack with:
	{
		if (board.at(currentPlayer).getTroops() == 2) {
			return to_string(1);
		}
		if (board.at(currentPlayer).getTroops() == 3) {
			return to_string(rand() % (2 + 1) + 1);
		}
		if (board.at(currentPlayer).getTroops() >= 3) {
			return to_string(rand() % (3 + 1) + 1);
		}
		break;
	}
	case(7):		// Attack Sequence: choose number of troops to defend with:
	{
		if (board.at(currentPlayer).getTroops() == 2) {
			return to_string(1);
		}
		if (board.at(currentPlayer).getTroops() >= 3) {
			return to_string(rand() % (2 + 1) + 1);
		}
		break;
	}
	case(8):		// Attack Sequence: choose whether or not to continue attacking:
	{
		if ((static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.0)) <= 0.85)) {
			return "y";
		}
		else {
			return "n";
		}
		break;
	}
	case(9):
	{
		/*
		string troopMovement;
		vector<Region> myRegions;
		for (int i = 0; i < board.size(); i++)
		{
			if (board.at(i).getCommander_id() == currentPlayer)
			{
				myRegions.push_back(board.at(i));
			}
		}
		int pick = rand() % myRegions.size();
		troopMovement += to_string(myRegions[pick].getID());
		int numTroops = rand() % newTroops;
		troopMovement += " ";
		troopMovement += "0";
		*/
		return "-1 -1 -1";
		break;
	}
	}
}
