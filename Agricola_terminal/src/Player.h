#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "Card.h"
#include "PlayerBoard.h"

using namespace std;

/*
Represents a Player in the Game;
*/
class Player {
public:
	// Constructs a player 
	Player(int id, bool family);

	~Player();

	int getID();
	
private:
	int id, total_family, remaining_family;
	map<string, int> score;
	unordered_map<string, int> resources;
	vector<Card*> cards, jobs, improvements;
	PlayerBoard* board;

	// Initializes player resources to 0 for each resource type
	void initializeResources();

	// Initializes player hand with starting cards.  Not used for family game
	void initializeCards();
	
	/*
	* Adds resource amount to named type
	* @param name: string representing resource type
	* @param amount: amount of type to be added
	*/
	void addResource(string name, int amount);
};