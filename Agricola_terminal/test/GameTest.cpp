#include "gtest/gtest.h"

TEST(GameTests, exampleTest) {
	EXPECT_TRUE(true);
}

/*
TEST(TwoPlayerFamilyBoard, boardSpaceAccessError) {
	//attempt access to out of range boardSpace

	//attempt access to out of range MajorImprovement

	//attempt delete pending Resource when non-existant

}

TEST(TwoPlayerFamilyBoard, nextRoundUpkeep) {
	//verify accumulationspaces correct at start of game
	for (int i = 0; i < 30; i++)
		if (board->getBoardSpace(i)->accumulation) {
			AccumulationSpace* space =
				((AccumulationSpace*)board->getBoardSpace(i));
			if (i < 16) {
				EXPECT_EQ(space->amount, space->upkeep_amount);
			}
			else {
				EXPECT_EQ(space->amount, 0);
			}
		}
	//place player marker accumulationspace - "take" resources


	//progress round by 1, verify correct accumulationspaces
}
*/