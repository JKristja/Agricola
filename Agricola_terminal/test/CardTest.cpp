#include "gtest/gtest.h"
#include "Card.h"


TEST(CardTest, createMajorImprovement) {
	MajorImprovement* nonTradeImp = new MajorImprovement("NonTrade", 2, false);
	MajorImprovement* nonTradeImp2 = new MajorImprovement("NonTrade", 1, false);
	MajorImprovement* tradeImp = new MajorImprovement("Tradeable", 1, true);
	tradeImp->setCost(1, 2, 3, 4, 0, 0, 4, 3, 2, 1);
	tradeImp->setTrade(nonTradeImp, true);
	tradeImp->setTrade(nonTradeImp2, false);
	
	//get name
	EXPECT_EQ(nonTradeImp->getName(), "NonTrade");
	EXPECT_EQ(tradeImp->getName(), "Tradeable");

	//get point value
	EXPECT_EQ(nonTradeImp->getPointValue(), 2);
	EXPECT_EQ(tradeImp->getPointValue(), 1);

	//get cost
	Cost expectedCost = Cost(1, 2, 3, 4, 0, 0, 4, 3, 2, 1);
	Cost tradeImpCost = tradeImp->getCost();
	EXPECT_EQ(tradeImpCost.wood, expectedCost.wood);
	EXPECT_EQ(tradeImpCost.clay, expectedCost.clay);
	EXPECT_EQ(tradeImpCost.reed, expectedCost.reed);
	EXPECT_EQ(tradeImpCost.stone, expectedCost.stone);
	EXPECT_EQ(tradeImpCost.grain, expectedCost.grain);
	EXPECT_EQ(tradeImpCost.vegetable, expectedCost.vegetable);
	EXPECT_EQ(tradeImpCost.sheep, expectedCost.sheep);
	EXPECT_EQ(tradeImpCost.boar, expectedCost.boar);
	EXPECT_EQ(tradeImpCost.cattle, expectedCost.cattle);
	EXPECT_EQ(tradeImpCost.food, expectedCost.food);

	//is tradeable?
	EXPECT_FALSE(nonTradeImp->isTradeable());
	EXPECT_TRUE(tradeImp->isTradeable());

	//get trade tradeable
	ASSERT_FALSE(tradeImp->getTrade().empty());
	EXPECT_EQ(tradeImp->getTrade()[0], nonTradeImp);
	EXPECT_EQ(tradeImp->getTrade()[1], nonTradeImp2);

	//get trade non-tradeable
	EXPECT_TRUE(nonTradeImp->getTrade().empty());

	//cleanup
	delete tradeImp;
	delete nonTradeImp;
	delete nonTradeImp2;
}

// Update for non-family game
TEST(CardTest, createMinorImprovement) {
	//stub
}

// Update for non-family game
TEST(CardTest, createJob) {
	//stub
}