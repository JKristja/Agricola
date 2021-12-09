#include "gtest/gtest.h"
#include "MainBoard.h"
#include "Player.h"

struct TwoPlayerFamilyBoard : public ::testing::Test {
	MainBoard* board;
	Player* player1;
	Player* player2;
	int current_round;

	virtual void SetUp() {
		board = new MainBoard(2, true);
		player1 = new Player(1, true);
		player2 = new Player(2, true);
		current_round = 1;
	}

	virtual void TearDown() {
		delete board;
		delete player1;
		delete player2;
	}
};

TEST_F(TwoPlayerFamilyBoard, defaultStartingSpaces) {
	//verify default spaces 0 through 15 (0-5 are "empty" for 2 Player)
	string expectedActions[16] = { "empty", "empty", "empty", "empty", "empty",
		"empty", "build rooms and/or stables", "starting player and storehouse",
		"take 1 grain", "plow 1 field", "build stable and/or bake bread",
		"day laborer", "3 wood", "1 clay", "1 reed", "fishing" };

	for (int i = 0; i < 16; i++)
		EXPECT_EQ(board->getBoardSpace(i)->action, expectedActions[i]);
}

TEST_F(TwoPlayerFamilyBoard, stageAssignment) {
	string actualActions[14];

	for (int i = 0; i < 14; i++)
		actualActions[i] = board->getBoardSpace(i + 16)->action;

	//verify Stage 1 
	string expectedStage1[4] = { "fences", "major or minor improvement", 
		"1 sheep", "sow and/or bake bread" };
	for (int i = 0; i < 4; i++) {
		int index = distance(actualActions,
			find(begin(actualActions), end(actualActions), expectedStage1[i]));
		EXPECT_GE(index, 0);
		EXPECT_LT(index, 4);
	}

	//verify Stage 2
	string expectedStage2[3] = { "family growth also minor improvement",
		"renovation also major or minor improvement", "1 stone" };
	for (int i = 0; i < 3; i++) {
		int index = distance(actualActions,
			find(begin(actualActions), end(actualActions), expectedStage2[i]));
		EXPECT_GE(index, 4);
		EXPECT_LT(index, 7);
	}

	//verify Stage 3
	string expectedStage3[2] = { "take 1 vegetable", "1 wild boar" };
	for (int i = 0; i < 2; i++) {
		int index = distance(actualActions,
			find(begin(actualActions), end(actualActions), expectedStage3[i]));
		EXPECT_GE(index, 7);
		EXPECT_LT(index, 9);
	}
	
	//verify Stage 4
	string expectedStage4[2] = { "1 cattle", "1 stone (2)" };
	for (int i = 0; i < 2; i++) {
		int index = distance(actualActions,
			find(begin(actualActions), end(actualActions), expectedStage4[i]));
		EXPECT_GE(index, 9);
		EXPECT_LT(index, 11);
	}

	//verify Stage 5
	string expectedStage5[2] = { "family growth without space",
		"plow 1 field and/or sow" };
	for (int i = 0; i < 2; i++) {
		int index = distance(actualActions,
			find(begin(actualActions), end(actualActions), expectedStage5[i]));
		EXPECT_GE(index, 11);
		EXPECT_LT(index, 13);
	}

	//verify Stage 6
	EXPECT_EQ(distance(actualActions,
		find(begin(actualActions), end(actualActions), "renovation also fences")),
		13);
}

TEST_F(TwoPlayerFamilyBoard, pendingResources) {
	//verify no pending resources at start of game
	for (int i = 0; i < 30; i++)
		EXPECT_TRUE(board->getBoardSpace(i)->pending_resources.empty());

	//add pending resources to next 2 rounds for player 1
	PendingResource p1Wood = PendingResource(player1->getID(), 3, "wood");
	for (int i = 1; i <= 2; i++)
		board->getBoardSpace(current_round + i)->pending_resources.push_back(p1Wood);

	//add two pending resources to next next round for player 2
	PendingResource p2Stone = PendingResource(player2->getID(), 2, "stone");
	PendingResource p2Cattle = PendingResource(player2->getID(), 1, "cattle");
	board->getBoardSpace(current_round + 2)->pending_resources.push_back(p2Stone);
	board->getBoardSpace(current_round + 2)->pending_resources.push_back(p2Cattle);
	
	for (int i = 1; i <= 2; i++)
		ASSERT_FALSE(
			board->getBoardSpace(current_round + i)->pending_resources.empty());

	//verify pending resources then verify correct resource removal
	ASSERT_EQ(
		board->getBoardSpace(current_round + 1)->pending_resources.size(), 1);
	EXPECT_EQ(board->getBoardSpace(current_round + 1)->pending_resources[0],
		p1Wood);
	board->getBoardSpace(current_round + 1)->clearPending(player1->getID());
	EXPECT_TRUE(
		board->getBoardSpace(current_round + 1)->pending_resources.empty());
	
	ASSERT_EQ(
		board->getBoardSpace(current_round + 2)->pending_resources.size(), 3);
	EXPECT_EQ(board->getBoardSpace(current_round + 2)->pending_resources[0],
		p1Wood);
	EXPECT_EQ(board->getBoardSpace(current_round + 2)->pending_resources[1],
		p2Stone);
	EXPECT_EQ(board->getBoardSpace(current_round + 2)->pending_resources[2],
		p2Cattle);
	board->getBoardSpace(current_round + 2)->clearPending(player1->getID());
	ASSERT_EQ(
		board->getBoardSpace(current_round + 2)->pending_resources.size(), 2);
	EXPECT_EQ(board->getBoardSpace(current_round + 2)->pending_resources[0],
		p2Stone);
	EXPECT_EQ(board->getBoardSpace(current_round + 2)->pending_resources[1],
		p2Cattle);
	board->getBoardSpace(current_round + 2)->clearPending(player2->getID());
	EXPECT_TRUE(
		board->getBoardSpace(current_round + 2)->pending_resources.empty());
}

TEST_F(TwoPlayerFamilyBoard, availableSpaces) {
	//verify only default and first round space available at start
	for (int i = 0; i < 30; i++)
		EXPECT_EQ(board->getBoardSpace(i)->occupied,
			(i < 6 || i > 16) ? -1 : 0);

	//"place" player marker, verify correct spaces available
	int occupiedP1 = 16;
	board->getBoardSpace(occupiedP1)->occupied = 1;

	//set 6th round, verify correct spaces available
	for (int i = 2; i <= 6; i++)
		board->getBoardSpace(i + 15)->occupied = 0;
	for (int i = 0; i < 30; i++)
		EXPECT_EQ(board->getBoardSpace(i)->occupied,
			(i < 6 || i > 21) ? -1 : ((i == occupiedP1)? 1 : 0));

	//"place" player marker, verify correct spaces available
	int occupiedP2 = 18;
	board->getBoardSpace(occupiedP2)->occupied = 2;

	for (int i = 0; i < 30; i++)
		EXPECT_EQ(board->getBoardSpace(i)->occupied,
			(i < 6 || i > 21) ? -1 : 
			(i == occupiedP1 ? 1 : 
				(i == occupiedP2 ? 2 : 0)));

	//"place" player marker, verify correct spaces available
	int occupied2P1 = 12;
	board->getBoardSpace(occupied2P1)->occupied = 1;

	for (int i = 0; i < 30; i++)
		EXPECT_EQ(board->getBoardSpace(i)->occupied,
			(i < 6 || i > 21) ? -1 :
			((i == occupiedP1 || i == occupied2P1) ? 1 :
				(i == occupiedP2 ? 2 : 0)));
}

TEST_F(TwoPlayerFamilyBoard, availableImprovements) {
	//verify all improvements available initially
	for (int i = 0; i < 10; i++)
		EXPECT_EQ(board->getMajorImprovement(i)->getOwnedBy(), 0);

	//take some improvements, verify correct still available
	board->getMajorImprovement(3)->setOwnedBy(player1->getID());
	board->getMajorImprovement(6)->setOwnedBy(player1->getID());
	board->getMajorImprovement(7)->setOwnedBy(player2->getID());
	for (int i = 0; i < 10; i++)
		EXPECT_EQ(board->getMajorImprovement(i)->getOwnedBy(),
			((i == 3 || i == 6) ? 1 : (i == 7 ? 2 : 0)));

	//take all improvements, verify none available
	for (int i = 0; i < 5; i++)
		board->getMajorImprovement(i)->setOwnedBy(player1->getID());
	for (int i = 5; i < 10; i++)
		board->getMajorImprovement(i)->setOwnedBy(player2->getID());
	for (int i = 0; i < 10; i++)
		EXPECT_GT(board->getMajorImprovement(i)->getOwnedBy(), 0);
}