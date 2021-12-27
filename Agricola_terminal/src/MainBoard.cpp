#include <algorithm>
#include <random>
#include "MainBoard.h"

using namespace std;

bool PendingResource::operator==(PendingResource other) const {
	return (this->owner == other.owner && this->amount == other.amount
		&& this->type == other.type);
}

void BoardSpace::clearPending(int owner) {
	auto resource = begin(pending_resources);

	while (resource != end(pending_resources)) {
		if ((*resource).owner == owner)
			resource = pending_resources.erase(resource);
		else
			resource++;
	}
}

void AccumulationSpace::upkeep() {
	if (occupied != -1)
		amount += upkeep_amount;
}

BoardSpace* MainBoard::getBoardSpace(int index) const {
	return board_spaces[index];
}

MajorImprovement* MainBoard::getMajorImprovement(int index) const {
	return major_improvements[index];
}

MainBoard::MainBoard(int pc, bool family) {
	initializeStartingSpaces(pc, family);
	initializeRoundSpaces();
	initializeStages();
	initializeMajorImprovements();
}

MainBoard::~MainBoard() {
	for (int i = 0; i < sizeof(board_spaces)/sizeof(*board_spaces); ++i)
		delete board_spaces[i];
	for (int i = 0; i < sizeof(major_improvements)/sizeof(MajorImprovement*); ++i)
		delete major_improvements[i];
}

/*
* Only 2 player family variant implemented for mvp;
* TODO: Other variants
*/
void MainBoard::initializeStartingSpaces(int player_count, bool family) {
	if (family) {
		switch (player_count) {
		case 3:
			//stub
			break;
		case 4:
			//stub
			break;
		case 5:
			//stub
			break;
		default:
			for (int i = 0; i < 6; ++i)
				board_spaces[i] = new BoardSpace("empty", false);
		}
		// default 1-2 player spaces
		board_spaces[6] = 
			new BoardSpace("build rooms and/or stables", false, true);
		board_spaces[7] = new AccumulationSpace("starting player and storehouse",
				"food", 1, 1, true);
		board_spaces[8] = new BoardSpace("take 1 grain", false, true);
		board_spaces[9] = new BoardSpace("plow 1 field", false, true);
		board_spaces[10] = 
			new BoardSpace("build stable and/or bake bread", false, true);
		board_spaces[11] = new BoardSpace("day laborer", false, true);
		board_spaces[12] = new AccumulationSpace("3 wood", "wood", 3, 3, true);
		board_spaces[13] = new AccumulationSpace("1 clay", "clay", 1, 1, true);
		board_spaces[14] = new AccumulationSpace("1 reed", "reed", 1, 1, true);
		board_spaces[15] = new AccumulationSpace("fishing", "food", 1, 1, true);
	} 
	else {
		switch (player_count) {
		case 3:
			//stub
			break;
		case 4:
			//stub
			break;
		case 5:
			//stub
			break;
		default:
			//stub
			break;
		}
		// default 1-2 player spaces
		//stub
	}
}

void MainBoard::initializeRoundSpaces() {
	round_spaces[0] = "fences";
	round_spaces[1] = "major or minor improvement";
	round_spaces[2] = "1 sheep";
	round_spaces[3] = "sow and/or bake bread";
	round_spaces[4] = "family growth also minor improvement";
	round_spaces[5] = "renovation also major or minor improvement";
	round_spaces[6] = "1 stone";
	round_spaces[7] = "take 1 vegetable";
	round_spaces[8] = "1 wild boar";
	round_spaces[9] = "1 cattle";
	round_spaces[10] = "1 stone (2)";
	round_spaces[11] = "family growth without space";
	round_spaces[12] = "plow 1 field and/or sow";
	round_spaces[13] = "renovation also fences";
}

void MainBoard::initializeStages() {
	// randomize stages with round spaces [0,3], [4,6], [7,8], [9,10], [11,12]
	// stage 6 always uses round space 13
	random_device rnd;
	mt19937 seed(rnd());
	shuffle(round_spaces, round_spaces + 4, seed);
	shuffle(round_spaces + 4, round_spaces + 7, seed);
	shuffle(round_spaces + 7, round_spaces + 9, seed);
	shuffle(round_spaces + 9, round_spaces + 11, seed);
	shuffle(round_spaces + 11, round_spaces + 13, seed);

	// assign stages 1 - 6 following starting spaces in board_spaces index + 16
	for (int i = 0; i < 14; ++i) {
		//check for accumulation spaces
		if (round_spaces[i].compare(0, 1, "1") == 0) {
			board_spaces[i + 16] = new AccumulationSpace(round_spaces[i],
				round_spaces[i].substr(2), 0, 1);
		}
		else {
			board_spaces[i + 16] = new BoardSpace(round_spaces[i], false);
		}
	}
	board_spaces[16]->occupied = 0;
	if (board_spaces[16]->accumulation) {
		AccumulationSpace* space = ((AccumulationSpace*)board_spaces[16]);
		space->amount = space->upkeep_amount;
	}
}

void MainBoard::initializeMajorImprovements() {
	major_improvements[0] = new MajorImprovement("fireplace", 1, false);
	major_improvements[0]->setCost(0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
	major_improvements[1] = new MajorImprovement("fireplace", 1, false);
	major_improvements[1]->setCost(0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
	major_improvements[2] = new MajorImprovement("cooking hearth", 1, true);
	major_improvements[2]->setCost(0, 4, 0, 0, 0, 0, 0, 0, 0, 0);
	major_improvements[2]->setTrade(major_improvements[0], true);
	major_improvements[2]->setTrade(major_improvements[1], false);
	major_improvements[3] = new MajorImprovement("cooking hearth", 1, true);
	major_improvements[3]->setCost(0, 5, 0, 0, 0, 0, 0, 0, 0, 0);
	major_improvements[3]->setTrade(major_improvements[0], true);
	major_improvements[3]->setTrade(major_improvements[1], false);
	major_improvements[4] = new MajorImprovement("well", 4, false);
	major_improvements[4]->setCost(1, 0, 0, 3, 0, 0, 0, 0, 0, 0);
	major_improvements[5] = new MajorImprovement("clay oven", 2, false);
	major_improvements[5]->setCost(0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
	major_improvements[6] = new MajorImprovement("stone oven", 3, false);
	major_improvements[6]->setCost(0, 1, 0, 3, 0, 0, 0, 0, 0, 0);
	major_improvements[7] = new MajorImprovement("joinery", 2, false);
	major_improvements[7]->setCost(2, 0, 0, 2, 0, 0, 0, 0, 0, 0);
	major_improvements[8] = new MajorImprovement("pottery", 2, false);
	major_improvements[8]->setCost(0, 2, 0, 2, 0, 0, 0, 0, 0, 0);
	major_improvements[9] = new MajorImprovement("basketmaker", 2, false);
	major_improvements[9]->setCost(0, 0, 2, 2, 0, 0, 0, 0, 0, 0);
}