#include "Card.h"
#include <string>
#include <vector>

using namespace std;

Card::Card(string name, int point_value) {
	this->name = name;
	this->point_value = point_value;
	this->owned_by = 0;
}

int Card::getPointValue() const{
	return point_value;
}

int Card::getOwnedBy() const {
	return owned_by;
}

void Card::setOwnedBy(int playerID) {
	this->owned_by = playerID;
}

string Card::getName() const{
	return name;
}

void Card::setCost(int wood, int clay, int reed, int stone, int grain, 
	int vegetable, int sheep, int boar, int cattle, int food) {
	cost.wood = wood;
	cost.clay = clay;
	cost.reed = reed;
	cost.stone = stone;
	cost.grain = grain;
	cost.vegetable = vegetable;
	cost.sheep = sheep;
	cost.boar = boar;
	cost.cattle = cattle;
	cost.food = food;
}

Cost Card::getCost() const {
	return cost;
}

MajorImprovement::MajorImprovement(string name, int point_value, bool tradeable) {
	this->name = name;
	this->point_value = point_value;
	this->tradeable = tradeable;
}

bool MajorImprovement::isTradeable() const {
	return tradeable;
}

void MajorImprovement::setTrade(MajorImprovement* trade, bool overwrite) {
	if (overwrite)
		valid_trades.clear();
	
	valid_trades.push_back(trade);
}

vector<MajorImprovement*> MajorImprovement::getTrade() const {
	return valid_trades;
}

MinorImprovement::MinorImprovement() {
	//stub
}

void MinorImprovement::setRequirement() {
	//stub
}

string MinorImprovement::getRequirement() {
	return ""; //stub
}

Job::Job() {
	//stub
}

void Job::setRequirement() {
	//stub
}

string Job::getRequirement() {
	return ""; //stub
}