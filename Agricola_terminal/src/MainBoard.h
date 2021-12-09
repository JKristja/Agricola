#pragma once

#include <vector>
#include <string>
#include "Card.h"

using namespace std;

// Represents pending resources due to player at beginning of round
struct PendingResource {
	int owner, amount;
	string type;

	PendingResource(int owner, int amount, string type) :
		owner(owner), amount(amount), type(type) {}

	bool operator==(PendingResource other) const;
};

/*
* Represents board space on main board - can be action or accumulation;
* Can contain pending resources if action space not yet revealed
* Occupied =	-1 if space not visible; 0 if available;
*				1/2/3/4/5 if occupied by player 1/2/3/4/5
*/
struct BoardSpace {
	bool accumulation;
	int occupied;
	string action;
	vector<PendingResource> pending_resources;

	BoardSpace(string action, bool accumulation) : action(action), occupied(-1),
		accumulation(accumulation) {}

	BoardSpace(string action, bool accumulation, bool visible) : action(action),
		accumulation(accumulation), occupied(visible ? 0 : -1) {}

	void clearPending(int owner);
};

/*
* Accumulation space;  tracks type of resource, current amount, and upkeep;
*/
struct AccumulationSpace : BoardSpace {
	string resource;
	int amount, upkeep_amount;

	AccumulationSpace(string action, string resource, int amount, int upkeep) :
		resource(resource), amount(amount), upkeep_amount(upkeep),
		BoardSpace(action, true) {}

	AccumulationSpace(string action, string resource, int amount, int upkeep,
		bool visible) :
		resource(resource), amount(amount), upkeep_amount(upkeep),
		BoardSpace(action, true, visible) {}

	void upkeep();
};

/*
Represents the main board; contains action space states and major improvements
*/
class MainBoard {
public:
	/**
	* Constructs the main board
	*/
	MainBoard(int pc, bool family);

	~MainBoard();

	BoardSpace* getBoardSpace(int index) const;

	MajorImprovement* getMajorImprovement(int index) const;

private:
	string round_spaces[14];
	BoardSpace* board_spaces[30];
	MajorImprovement* major_improvements[10];

	/*
	* Only 2 player family variant implemented for mvp;
	* TODO: Other variants
	*/
	void initializeStartingSpaces(int player_count, bool family);

	void initializeRoundSpaces();

	void initializeStages();

	void initializeMajorImprovements();
	
};