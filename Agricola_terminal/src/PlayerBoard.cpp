#include "PlayerBoard.h"

PlayerBoard::PlayerBoard() {
	this->rooms = 2;
	this->pet = false;
	this->fields = 0;
	this->pastures = 0;
	this->stables = 0;
	this->fences = 0;
	initializeSpaces();
}

PlayerBoard::~PlayerBoard() {
	for (int i = 0; i < 15; i++) {
		delete this->spaces[i];
	}
}

PlayerBoardSpace* PlayerBoard::getBoardSpace(int index) {
	return this->spaces[index];
}

void PlayerBoard::initializeSpaces() {
	// set empty board with mid and bottom leftmost spaces starting room spaces
	for (int i = 0; i < 15; i++)
		if (i != 5 && i != 10)
			this->spaces[i] = new EmptySpace(i);

	RoomSpace::RoomType wood = RoomSpace::RoomType::wood;
	this->spaces[5] = new RoomSpace(5, wood, true);
	this->spaces[10] = new RoomSpace(10, wood, true);

	// update neighbouring spaces 
	((EmptySpace*)this->spaces[0])->to_room = true;
	((EmptySpace*)this->spaces[6])->to_room = true;
	((EmptySpace*)this->spaces[11])->to_room = true;
}

bool PlayerBoard::convertSpace(int index, PlayerBoardSpace::SpaceType to_type) {
	PlayerBoardSpace* space = spaces[index];
	if (space->type == PlayerBoardSpace::SpaceType::empty) {
		switch (to_type) {
		case PlayerBoardSpace::SpaceType::room:
			if (((EmptySpace*)space)->to_room) {
				convertToRoom(index);
				return true;
			}
			break;
		case PlayerBoardSpace::SpaceType::field:
			if (((EmptySpace*)space)->to_field) {
				convertToField(index);
				return true;
			}
			break;
		case PlayerBoardSpace::SpaceType::pasture:
			if (((EmptySpace*)space)->to_pasture) {
				convertToPasture(index);
				return true;
			}
			break;
		case PlayerBoardSpace::SpaceType::stable:
			if (((EmptySpace*)space)->to_stable) {
				convertToStable(index);
				return true;
			}
			break;
		}
	}
	else if (space->type == PlayerBoardSpace::SpaceType::stable) {
		if (to_type == PlayerBoardSpace::SpaceType::pasture) {
			//stub
			return true;
		}
	}
	return false;
}

void PlayerBoard::convertToRoom(int index) {
	RoomSpace* current_house = ((RoomSpace*)this->spaces[5]);

	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] =
		new RoomSpace(index, current_house->room_type, false);
	delete old_space;
}

void PlayerBoard::convertToField(int index) {
	//stub
}

void PlayerBoard::convertToPasture(int index) {
	//stub
}

void PlayerBoard::convertToStable(int index) {
	//stub
}

bool PlayerBoard::upgradeRooms() {

}

bool PlayerBoard::hasPet() const {
	return pet;
}

int PlayerBoard::getNumRooms() const {
	return rooms;
}

int PlayerBoard::getNumFields() const {
	return fields;
}

int PlayerBoard::getNumPastures() const {
	return pastures;
}

int PlayerBoard::getNumStables() const {
	return stables;
}

int PlayerBoard::getNumFences() const {
	return fences;
}