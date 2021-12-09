#pragma once

#include <iostream>
#include <vector>

using namespace std;

/*
* Cost struct for Card costs
*/
struct Cost {
	int wood, clay, reed, stone, grain, vegetable, sheep, boar, cattle, food;

	Cost() : wood(0), clay(0), reed(0), stone(0), grain(0), vegetable(0),
		sheep(0), boar(0), cattle(0), food(0) {}

	Cost(int w, int cl, int r, int st, int g, int v, int sh, int b, int ca,
		int f) : wood(w), clay(cl), reed(r), stone(st), grain(g), vegetable(v),
		sheep(sh), boar(b), cattle(ca), food(f) {}
};

/*
* Represents a Card in the Game, contains sub-classes for Major Improvement,
* Minor Improvement, and Job Cards
*/
class Card {
public:
	Card(string name, int point_value);

	Card() : name(""), point_value(0), owned_by(0) {}

	int getPointValue() const;

	int getOwnedBy() const;

	void setOwnedBy(int playerID);

	string getName() const;

	void setCost(int wood, int clay, int reed, int stone, int grain,
		int vegetable, int sheep, int boar, int cattle, int food);

	Cost getCost() const;

protected:
	int point_value, owned_by;
	string name;
	Cost cost;
};

class MajorImprovement : public Card {
public:
	MajorImprovement(string name, int point_value, bool tradeable);

	bool isTradeable() const;

	/*
	* sets this->trade to supplied trade improvement and sets tradeable to true
	* adds to existing trade improvements if overwrite is false
	*/
	void setTrade(MajorImprovement* trade, bool overwrite);
	 
	/*
	* Returns vector of pointers to major improvement that can be traded for this
	*/
	vector<MajorImprovement*> getTrade() const;

private:
	bool tradeable;
	vector<MajorImprovement*> valid_trades;
};


//TODO Update as part of Non-Family game
class MinorImprovement : public Card {
public:
	MinorImprovement();
	void setRequirement();
	string getRequirement();
private:
	string requirement;
};

//TODO Update as part of Non-Family game
class Job : public Card {
public:
	Job();
	void setRequirement();
	string getRequirement();
private:
	string requirement;
};