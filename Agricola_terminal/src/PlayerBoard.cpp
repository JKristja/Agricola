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
	for (int i = 0; i < 15; ++i) {
		delete this->spaces[i];
	}
}

PlayerBoardSpace* PlayerBoard::getBoardSpace(int index) {
	return this->spaces[index];
}

void PlayerBoard::initializeSpaces() {
	// set empty board with mid and bottom leftmost spaces starting room spaces
	for (int i = 0; i < 15; ++i)
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
				convertToPasture(vector<int> {index}, 
					PastureSpace::AnimalType::none);
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
			convertToPasture(vector<int> {index},
				((StableSpace*)space)->animal_type);
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

bool PlayerBoard::validNeighbour(int index1, int index2) {
	bool is_neighbour = false;
	if (index1 % 5 != 0)
		if (index2 == index1 - 1)
			is_neighbour = true;
	if (index1 % 5 != 4)
		if (index2 == index1 + 1)
			is_neighbour = true;
	if (index1 > 4)
		if (index2 == index1 - 5)
			is_neighbour = true;
	if (index1 < 10)
		if (index2 == index1 + 5)
			is_neighbour = true;

	return is_neighbour;
}

void PlayerBoard::convertToRoom(int index) {
	RoomSpace* current_house = ((RoomSpace*)this->spaces[5]);

	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] =
		new RoomSpace(index, current_house->room_type, false);
	delete old_space;

	++this->num_rooms;
}

void PlayerBoard::convertToField(int index) {
	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] = new FieldSpace(index);
	delete old_space;

	if (num_fields == 0)
		for (int i = 0; i < sizeof(spaces) / sizeof(PlayerBoardSpace*); ++i)
			if (spaces[i]->type == PlayerBoardSpace::SpaceType::empty)
				((EmptySpace*)spaces[i])->to_field = false;

	++this->num_fields;
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
void PlayerBoard::convertToPasture(vector<int> indices, 
	PastureSpace::AnimalType animal_type) {
	if (indices.size() == 0)
		return;

	bool stable = false;
	int linked_stables = 0;
	int linked_quantity = 0;
	vector<PastureSpace*> linked_pastures;
	
	for (int index : indices)
		if (spaces[index]->type == PlayerBoardSpace::SpaceType::stable)
			++linked_stables;

	for (int index : indices) {
		PlayerBoardSpace* old_space = spaces[index];
		if (old_space->type == PlayerBoardSpace::SpaceType::stable) {
			stable = true;
			if (animal_type != PastureSpace::AnimalType::none &&
				((StableSpace*)old_space)->animal &&
				((StableSpace*)old_space)->animal_type == animal_type) {
				++linked_quantity;
			}
			else if (((StableSpace*)old_space)->animal) {
				--this->num_animals;
			}
		}
		PastureSpace* new_space = new PastureSpace(index, stable, 
			linked_stables, indices.size(), animal_type);
		spaces[index] = new_space;
		linked_pastures.push_back(new_space);
		delete old_space;
	}

	for (int index : indices) {
		((PastureSpace*)spaces[index])->linked_pastures = linked_pastures;
		((PastureSpace*)spaces[index])->linked_quantity = linked_quantity;
	}

	if (num_pastures == 0)
		for (int i = 0; i < sizeof(spaces) / sizeof(PlayerBoardSpace*); ++i)
			if (spaces[i]->type == PlayerBoardSpace::SpaceType::empty)
				((EmptySpace*)spaces[i])->to_pasture = false;

	++this->num_pastures;
	this->animal_capacity += 2 * indices.size() * pow(2, linked_stables);
	this->num_animals += linked_quantity;
}

bool PlayerBoard::convertToLinkedPasture(vector<int> indices,
	PastureSpace::AnimalType animal_type) {
	if (indices.size() == 0)
		return false;
	//check all spaces convertable and at least one valid empty/stable to link
	int connections = 1;
	bool valid = false;
	for (int i = 0; i < indices.size(); ++i) {
		PlayerBoardSpace::SpaceType space_type = spaces[indices[i]]->type;
		if (space_type == PlayerBoardSpace::SpaceType::empty) {
			if (((EmptySpace*)spaces[indices[i]])->to_pasture)
				valid = true;
		}
		else if (space_type == PlayerBoardSpace::SpaceType::stable) {
			if (((StableSpace*)spaces[indices[i]])->to_pasture)
				valid = true;
		}
		else {
			return false;
		}
		//check valid indices orthogonal to each other
		for (int j = i + 1; j < indices.size(); ++j) {
			int current = indices[i];
			int neighbour = indices[j];
			if (validNeighbour(current, neighbour))
				++connections;
		}
	}

	if (!valid || connections != indices.size())
		return false;

	convertToPasture(indices, animal_type);
	for (int index : indices)
		notifyNeighbours(index, PlayerBoardSpace::SpaceType::pasture);

	return true;
}

void PlayerBoard::convertToStable(int index) {
	PlayerBoardSpace* old_space = this->spaces[index];
	this->spaces[index] = 
		new StableSpace(index, ((EmptySpace*)old_space)->to_pasture);
	delete old_space;

	++this->num_stables;
}

bool PlayerBoard::addStable(int index) {
	if (spaces[index]->type == PlayerBoardSpace::SpaceType::empty) {
		this->convertSpace(index, PlayerBoardSpace::SpaceType::stable);
		return true;
	}

	if (spaces[index]->type == PlayerBoardSpace::SpaceType::pasture) {
		PastureSpace* pasture_space = (PastureSpace*)spaces[index];
		if (pasture_space->stable)
			return false;

		pasture_space->stable = true;
		int existing_pasture_cap = pasture_space->linked_capacity;
		for (auto adjoined : pasture_space->linked_pastures) {
			++adjoined->linked_stables;
			adjoined->linked_capacity *= 2;
		}
		this->animal_capacity += existing_pasture_cap;
		++this->num_stables;
		return true;
	}
	return false;
}

bool PlayerBoard::addAnimal(int index, int num_animals,
	PastureSpace::AnimalType type) {
	if (num_animals <= 0 || type == PastureSpace::AnimalType::none)
		return false;

	//check valid space
	if (spaces[index]->type == PlayerBoardSpace::SpaceType::stable) {
		StableSpace* board_space = (StableSpace*)spaces[index];
		if (num_animals != 1 || board_space->animal)
			return false;
		board_space->animal = true;
		board_space->animal_type = type;
		++this->num_animals;
		return true;
	} 
	else if (spaces[index]->type == PlayerBoardSpace::SpaceType::pasture) {
		PastureSpace* board_space = (PastureSpace*)spaces[index];
		if (board_space->animal_type != PastureSpace::AnimalType::none &&
			board_space->animal_type != type)
			return false;
		if (board_space->linked_quantity + num_animals > 
			board_space->linked_capacity)
			return false;
		for (auto pasture : board_space->linked_pastures) {
			pasture->animal_type = type;
			pasture->linked_quantity += num_animals;
		}
		this->num_animals += num_animals;
		return true;
	}
	else {
		return false;
	}
}


bool PlayerBoard::removeAnimal(int index, int num_animals) {
	if (num_animals <= 0)
		return false;
	
	if (spaces[index]->type == PlayerBoardSpace::SpaceType::stable) {
		StableSpace* board_space = (StableSpace*)spaces[index];
		if (!board_space->animal || num_animals != 1)
			return false;
		board_space->animal = false;
		board_space->animal_type = PastureSpace::AnimalType::none;
		--this->num_animals;
	}
	else if (spaces[index]->type == PlayerBoardSpace::SpaceType::pasture) {
		PastureSpace* board_space = (PastureSpace*)spaces[index];
		if (board_space->linked_quantity < num_animals)
			return false;
		for (auto pasture : board_space->linked_pastures) {
			pasture->linked_quantity -= num_animals;
			if (pasture->linked_quantity == 0)
				pasture->animal_type = PastureSpace::AnimalType::none;
		}
		--this->num_animals;
	}
	else {
		return false;
	}
}

bool PlayerBoard::upgradeRooms() {
	for (int i = 0; i < sizeof(spaces) / sizeof(PlayerBoardSpace*); ++i) {
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
	++this->num_animals;
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
	--this->num_animals;
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