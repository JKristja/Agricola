#pragma once
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// Base struct for board space, can be one of five sub-types
struct PlayerBoardSpace {
	enum struct SpaceType { empty, room, field, pasture, stable }; 
	int position;
	SpaceType type;

	PlayerBoardSpace(int position, SpaceType type) :
		position(position), type(type) {}
};

/*
* Empty board space: can be converted into any of room/field/pasture/stable;
* Must be adjacent to room/field/pasture to convert unless first of its type;
* Can always be converted to stable;
* Point value = -1
*/
struct EmptySpace : PlayerBoardSpace {
	bool to_room, to_field, to_pasture, to_stable;

	EmptySpace(int position) : 
		to_room(false), to_field(true), to_pasture(true), to_stable(true),
		PlayerBoardSpace(position ,SpaceType::empty) {}
};

/*
* Room board space: cannot be converted to any other type;
* Can contain 1 pet if no pet exists elsewhere;
* Can contain 1 player piece if piece not yet played;
* Can only be one of "wood", "clay", "stone"
* Can be upgraded wood -> clay -> stone;
* Point values:
*	clay:	1
*	stone:	2
*/
struct RoomSpace : PlayerBoardSpace {
	enum struct RoomType { wood, clay, stone };
	RoomType room_type;
	bool pet, meeple;
	
	RoomSpace(int position, RoomType type, bool meeple) :
		room_type(type), pet(false), meeple(meeple), 
		PlayerBoardSpace(position, SpaceType::room) {}
};

/*
* Field board space: cannot be converted to any other type;
* Can be empty or contain up to 3 grain or 2 vegetable;
* Point value is dependent on total fields:
*	0 / 1 / 2 / 3 / 4 / 5+ fields:	-1 / -1 / 1 / 2 / 3 / 4
*/
struct FieldSpace : PlayerBoardSpace {
	enum struct FieldType { empty, grain, vegetable };
	FieldType field_type;
	int	quantity;

	FieldSpace(int position) : field_type(FieldType::empty), quantity(0),
		PlayerBoardSpace(position, SpaceType::field) {}

	/* 
	* Plants type into an empty field;
	* Returns true if successful; false if target field not empty;
	*/
	bool sow(FieldType type);
	
	/*
	* Reduces quantity by 1; reverts field type to empty if quantity becomes 0;
	* Returns FieldType at start of harvest;
	*/
	FieldType harvest();
};

/*
* Pasture board space: cannot be converted to any other type;
* Can be empty or contain a stable and/or one type of animal;
* A single pasture may be composed of multiple pasture spaces;
* Animal capacity is dependant on number of stables within pasture;
* Point value is dependent on total pastures:
*	0 / 1 / 2 / 3 / 4+ pastures:	-1 / 1 / 2 / 3 / 4
*/
struct PastureSpace : PlayerBoardSpace {
	enum struct AnimalType { none, sheep, boar, cattle };
	bool stable;
	int linked_stables, capacity, quantity;
	AnimalType animal_type;
	vector<PastureSpace*> linked_pastures;

	PastureSpace(int position, bool stable, int linked_stables) :
		stable(stable), linked_stables(linked_stables), 
		capacity(2 * pow(2, linked_stables)),
		quantity(0), animal_type(AnimalType::none),
		PlayerBoardSpace(position, SpaceType::pasture) {}
};

/*
* Stable board space: can be converted to pasture type;
* Can be empty or contain a single animal;
* Point value = 0
*/
struct StableSpace : PlayerBoardSpace {
	bool animal;
	PastureSpace::AnimalType animal_type;

	StableSpace(int position) :	animal(false), 
		animal_type(PastureSpace::AnimalType::none),
		PlayerBoardSpace(position, SpaceType::stable) {}
};

/*
Represents a Player board in the Game;
*/
class PlayerBoard {
public:
	// Construcst a Player Board
	PlayerBoard();

	// Destroy a Player Board
	~PlayerBoard();

	PlayerBoardSpace* getBoardSpace(int index);

	/*
	* Converts Player board space from current type to to_type;
	* Returns true if successful; false if invalid conversion;
	* Replaces existing pointer, and deletes old reference
	*/
	bool convertSpace(int index, PlayerBoardSpace::SpaceType to_type);

	/*
	* Similar to convertSpace, but only for multi-part pasture;
	* Creates linked multi-space pasture from given orthogonal indices
	* First index will be set as group leader
	* Returns true if successful;
	* Returns false if indices not all orthogonal, or if any spaces are invalid
	*/
	bool convertToLinkedPasture(vector<int> indices);

	/*
	* Upgrade all room spaces to next level (wood->clay->stone);
	* Returns true if successful; false if already stone;
	*/
	bool upgradeRooms();

	/*
	* Adds pet to room at given index;  Returns true on success;
	* Returns false if pet exists elsewhere, or if space at index not a room
	*/
	bool addPet(int room_index, PastureSpace::AnimalType pet_type);

	/*
	* Moves pet to room at given index;  Returns true on success;
	* Returns false if no pet exists, or if space at index not a room
	*/
	bool movePet(int to_index);

	/*
	* Removes pet from player board;  Returns true on success;
	* Returns false if no pet exists
	*/
	bool removePet();

	bool hasPet() const;

	PastureSpace::AnimalType getPetType() const;

	int getNumRooms() const;

	int getNumFields() const;

	int getNumPastures() const;

	int getNumStables() const;

	int getNumFences() const;

	int getNumAnimals() const;

	int getAnimalCapacity() const;

private:
	bool pet;
	int num_rooms, num_fields, num_pastures, num_stables, num_fences, pet_index;
	int num_animals, animal_capacity;
	PlayerBoardSpace* spaces[15];
	PastureSpace::AnimalType pet_type;

	void initializeSpaces();
	
	//Helper to notify empty neighbours of new neighbour type
	void notifyNeighbours(int index, PlayerBoardSpace::SpaceType caller);

	//Helper to notify specific empty neighbour of new neighbour type
	void notifyNeighbour(int index, PlayerBoardSpace::SpaceType caller);

	//Helper function to convert space at index to room type
	void convertToRoom(int index);

	//Helper function to convert space at index to field type
	void convertToField(int index);
	
	/*
	* Helper function to convert space at indices to pasture type;
	* Removes any animals from included stables and sets animal type to none;
	* @param indices: indices of spaces to be converted to linked pasture
	*/
	void convertToPasture(vector<int> indices);

	//Helper function to convert space at index to stable type
	void convertToStable(int index);
};