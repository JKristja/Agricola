#pragma once

#include <vector>
#include "Player.h"
#include "MainBoard.h"

using namespace std;

/*
Represents the Game; contains board and player states
*/
class Game {
public:
	// Constructs a default two player family game
	Game();

	/**
	* Constructs a game with given player count and variant
	* Requires 1 <= pc <= 5
	*/
	Game(int pc, bool family);

	// Destroys a Game
	~Game();

	/* TODO: GAME ACTIONS

	bool takeAction(int space_index);

	bool playCard(int card_index);

	bool useCard(int card_index);

	void showMainBoard();

	void showPlayerBoard();

	void showPlayerCards();

	*/

private:
	int player_count, starting_player, current_player, current_round;
	vector<Player*> players;
	MainBoard* board;

	// Initializes players for given player count and variant
	void initializePlayers(bool family);

	// Not used in family game
	void initializeMinorImprovements(int player_count);

	// Not used in family game
	void initializeJobs(int player_count);
};