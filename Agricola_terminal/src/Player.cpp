#include "Player.h"

Player::Player(int id, bool family) {
	this->id = id;
	this->total_family = 2;
	this->remaining_family = 2;
	initializeResources();
	(id == 1) ? this->addResource("food", 2) : this->addResource("food", 3);
	if (!family)
		initializeCards();
	this->board = new PlayerBoard();
}

Player::~Player() {
	delete this->board;
}

void Player::initializeResources() {
	this->resources["wood"] = 0;
	this->resources["clay"] = 0;
	this->resources["reed"] = 0;
	this->resources["stone"] = 0;
	this->resources["free grain"] = 0;
	this->resources["planted grain"] = 0;
	this->resources["free vegetable"] = 0;
	this->resources["planted vegetable"] = 0;
	this->resources["sheep"] = 0;
	this->resources["boar"] = 0;
	this->resources["cattle"] = 0;
	this->resources["food"] = 0;
}

void Player::initializeCards() {
	//stub
}

void Player::addResource(string name, int amount) {
	this->resources[name] += amount;
}

int Player::getID() {
	return id;
}