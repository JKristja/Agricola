/**
* Implementation of the Game class representing Agricola
*/

#include "Game.h"

Game::Game() {
	this->player_count = 2;
	this->current_player = 1;
	this->starting_player = 1;
	this->current_round = 1;
	this->board = new MainBoard(player_count, true);
	initializePlayers(true);
}

Game::Game(int pc, bool family) {
	this->player_count = pc;
	this->current_player = 1;
	this->starting_player = 1;
	this->current_round = 1;
	this->board = new MainBoard(player_count, family);
	if (!family) {
		initializeMinorImprovements(player_count);
		initializeJobs(player_count);
	}
	initializePlayers(family);
}

Game::~Game() {
	for (int i = 0; i <= player_count - 1; i++) {
		delete players[i];
	}
	delete board;
}

void Game::initializePlayers(bool family) {
	for (int i = 1; i <= player_count; i++)
		players.push_back(new Player(i, family));
}

// TODO: Implement minor improvements/ jobs
void Game::initializeMinorImprovements(int player_count) {
	//stub
}

void Game::initializeJobs(int player_count) {
	//stub
}