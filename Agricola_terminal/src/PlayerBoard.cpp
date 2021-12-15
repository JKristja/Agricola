#include "PlayerBoard.h"

PlayerBoard::PlayerBoard() {
this->num_rooms = 2;
this->pet = false;
this->pet_index = -1;
this->pet_type = PastureSpace::AnimalType::none;
this->num_fields = 0;
this->num_pastures = 0;
this->num_stables = 0;
this->num_fences = 0;
this->animal_capacity = 1;
this->num_animals = 0;
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
	bool converted = false;
	PlayerBoardSpace* space = spaces[index];
	if (space->type == PlayerBoardSpace::SpaceType::empty) {
		switch (to_type) {
		case PlayerBoardSpace::SpaceType::room:
			if (((EmptySpace*)space)->to_room) {
				convertToRoom(index);
				converted = true;	
			}
			break;
		case PlayerBoardSpace::SpaceType::field:
			if (((EmptySpace*)space)->to_field) {
				convertToField(index);
				converted = true;
			}
			break;
		case PlayerBoardSpace::SpaceType::pasture:
			if (((EmptySpace*)space)->to_pasture) {
				convertToPasture(vector<int> {index});
				converted = true;
			}
			break;
		case PlayerBoardSpace::SpaceType::stable:
			if (((EmptySpace*)space)->to_stable) {
				convertToStable(index);
				converted = true;
			}
			break;
		}
	}
	else if (space->type == PlayerBoardSpace::SpaceType::stable) {
		if (to_type == PlayerBoardSpace::SpaceType::pasture) {
			convertToPasture(vector<int> {index});
			converted = true;
		}
	}

	if (converted)
		notifyNeighbours(index, to_type);

	return converted;
}

void PlayerBoard::notifyNeighbours(int index, PlayerBoardSpace::SpaceType caller) {
	//up
	if (index > 4)
		if (spaces[index - 5]->type == PlayerBoardSpace::SpaceType::empty)
			notifyNeighbour(index - 5, caller);
	//right
	if (index % 5 != 4)
		if (spaces[index + 1]->type == PlayerBoardSpace::SpaceType::empty)
			notifyNeighbour(index + 1, caller);
	//down
	if (index < 10)
		if (spaces[index + 5]->type == PlayerBoardSpace::SpaceType::empty)
			notifyNeighbour(index + 5, caller);
	//left
	if (index % 5 != 0)
		if (spaces[index - 1]->type == PlayerBoardSpace::SpaceType::empty)
			notifyNeighbour(index - 1, caller);
}

void PlayerBoard::notifyNeighbour(int index, PlayerBoardSpace::SpaceType caller) {
	if (spaces[index]->type != PlayerBoardSpace::SpaceType::empty)
		return;
	
	switch (caller) {
	case PlayerBoardSpace::SpaceType::room:
		((EmptySpace*)spaces[index])->to_room = true;
		break;
	case PlayerBoardSpace::SpaceType::field:
		((EmptySpace*)spaces[index])->to_field = true;
		break;
	case PlayerBoardSpace::SpaceType::pasture:
		((EmptySpace*)spaces[index])->to_pasture = true;
		break;
	case PlayerBoardSpace::SpaceType::stable:
		((EmptySpace*)spaces[index])->to_stable = true;
		break;
	}
}

void PlayerBoard::convertToRoom(int index) {
	RoomSpace* current_house = ((RoomSpace*)this->spaces[5]);

	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] =
		new RoomSpace(index, current_house->room_type, false);
	delete old_space;

	this->num_rooms += 1;
}

void PlayerBoard::convertToField(int index) {
	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] = new FieldSpace(index);
	delete old_space;

	if (num_fields == 0)
		for (int i = 0; i < sizeof(spaces) / sizeof(PlayerBoardSpace*); i++)
			if (spaces[i]->type == PlayerBoardSpace::SpaceType::empty)
				((EmptySpace*)spaces[i])->to_field = false;

	this->num_fields += 1;
}

bool FieldSpace::sow(FieldType type) {
	if (quantity != 0 || type == FieldType::empty)
		return false;

	this->field_type = type;
	(type == FieldType::grain) ? quantity = 3 : quantity = 2;
	return true;
}

FieldSpace::FieldType FieldSpace::harvest() {
	if (this->field_type == FieldType::empty)
		return FieldType::empty;

	FieldType harvested_type = this->field_type;
	this->quantity -= 1;
	if (quantity == 0)
		this->field_type = FieldType::empty;

	return harvested_type;
}

// UPDATE TO HAVE ANIMAL TYPE INPUT
// UPDATE LINKED PASTURE CONVERT TO ALLOW FOR ANIMAL TYPE
// PLAYER ACTION WILL HAVE TO CHECK IF MIXED TYPE AND PROMPT SELECTION
// IF NO MIXED TYPE PLAYER ACTION WILL DEFAULT TO EXISTING TYPE

void PlayerBoard::convertToPasture(vector<int> indices) {
	if (indices.size() == 0)
		return;

	bool stable;
	int linked_stables = 0;
	vector<PastureSpace*> linked_pastures;
	
	for (int i = 0; i < indices.size(); i++)
		if (spaces[indices[i]]->type == PlayerBoardSpace::SpaceType::stable)
			linked_stables += 1;
	
	for (int i = 0; i < indices.size(); i++) {
		PlayerBoardSpace* old_space = spaces[indices[i]];
		stable = (old_space->type == PlayerBoardSpace::SpaceType::stable);
		PastureSpace* new_space = new PastureSpace(i, stable, linked_stables);
		spaces[indices[i]] = new_space;
		linked_pastures.push_back(new_space);
		delete old_space;
	}

	for (int i = 0; i < indices.size(); i++)
		((PastureSpace*)spaces[indices[i]])->linked_pastures = linked_pastures;

	this->num_pastures += 1;
	this->animal_capacity += 2 * indices.size() * pow(2, linked_stables);
}

bool PlayerBoard::convertToLinkedPasture(vector<int> indices) {
	//stub
	return false;
}

void PlayerBoard::convertToStable(int index) {
	//stub
}

bool PlayerBoard::upgradeRooms() {
	for (int i = 0; i < sizeof(spaces) / sizeof(PlayerBoardSpace*); i++) {
		if (spaces[i]->type == PlayerBoardSpace::SpaceType::room) {
			RoomSpace* room_i = ((RoomSpace*)spaces[i]);
			if (room_i->room_type == RoomSpace::RoomType::stone)
				return false;
			if (room_i->room_type == RoomSpace::RoomType::clay) {
				room_i->room_type = RoomSpace::RoomType::stone;
			}
			else {
				room_i->room_type = RoomSpace::RoomType::clay;
			}
		}
	}
	return true;
}

bool PlayerBoard::addPet(int room_index, PastureSpace::AnimalType pet_type) {
	if (spaces[room_index]->type != PlayerBoardSpace::SpaceType::room || this->pet)
		return false;

	((RoomSpace*)spaces[room_index])->pet = true;
	this->pet = true;
	this->pet_index = room_index;
	this->pet_type = pet_type;
	return true;
}

bool PlayerBoard::movePet(int to_index) {
	if (spaces[to_index]->type != PlayerBoardSpace::SpaceType::room || !this->pet)
		return false;

	((RoomSpace*)spaces[pet_index])->pet = false;
	((RoomSpace*)spaces[to_index])->pet = true;
	this->pet_index = to_index;
	return true;
}

bool PlayerBoard::removePet() {
	if (!this->pet)
		return false;

	((RoomSpace*)spaces[pet_index])->pet = false;
	this->pet = false;
	this->pet_type = PastureSpace::AnimalType::none;
	this->pet_index = -1;
	return true;
}

bool PlayerBoard::hasPet() const {
	return pet;
}

PastureSpace::AnimalType PlayerBoard::getPetType() const {
	return pet_type;
}

int PlayerBoard::getNumRooms() const {
	return num_rooms;
}

int PlayerBoard::getNumFields() const {
	return num_fields;
}

int PlayerBoard::getNumPastures() const {
	return num_pastures;
}

int PlayerBoard::getNumStables() const {
	return num_stables;
}

int PlayerBoard::getNumFences() const {
	return num_fences;
}

int PlayerBoard::getNumAnimals() const {
	return num_animals;
}

int PlayerBoard::getAnimalCapacity() const {
	return animal_capacity;
}