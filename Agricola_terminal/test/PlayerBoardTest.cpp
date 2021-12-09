#include "gtest/gtest.h"
#include "PlayerBoard.h"

struct GenericPlayerBoard : public ::testing::Test {
	PlayerBoard* player_board;

	virtual void SetUp() {
		player_board = new PlayerBoard();
	}

	virtual void TearDown() {
		delete player_board;
	}
};

TEST_F(GenericPlayerBoard, boardSetUp) {
	PlayerBoardSpace* board_space;

	//verify two rooms, no pet, no fields/pastures/stables/fences
	EXPECT_EQ(player_board->getNumRooms(), 2);
	EXPECT_FALSE(player_board->hasPet());
	EXPECT_EQ(player_board->getNumFields(), 0);
	EXPECT_EQ(player_board->getNumPastures(), 0);
	EXPECT_EQ(player_board->getNumStables(), 0);
	EXPECT_EQ(player_board->getNumFences(), 0);

	//Verify two wood rooms with markers in pos 5 and 10, rest empty
		for (int i = 0; i < 15; i++) {
		board_space = player_board->getBoardSpace(i);
		EXPECT_EQ(board_space->position, i);
		if (i == 5 || i == 10) {
			ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
			RoomSpace* room_space = (RoomSpace*)board_space;
			EXPECT_EQ(room_space->room_type, RoomSpace::RoomType::wood);
			EXPECT_FALSE(room_space->pet);
			EXPECT_TRUE(room_space->meeple);
		} 
		else {
			ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
			EmptySpace* empty_space = (EmptySpace*)board_space;
			EXPECT_TRUE(empty_space->to_field);
			EXPECT_TRUE(empty_space->to_pasture);
			EXPECT_TRUE(empty_space->to_stable);
			if (i == 0 || i == 6 || i == 11) {
				EXPECT_TRUE(empty_space->to_room);
			}
			else {
				EXPECT_FALSE(empty_space->to_room);
			}
		}
	}
}

TEST_F(GenericPlayerBoard, convertSpaceToRoom) {
	//test valid conversion to wood room
	EXPECT_EQ(player_board->getNumRooms(), 2);
	EXPECT_FALSE(player_board->hasPet());
	PlayerBoardSpace* board_space = player_board->getBoardSpace(0);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	ASSERT_TRUE(((EmptySpace*)board_space)->to_room);
	ASSERT_TRUE(player_board->convertSpace(0, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(0);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_FALSE(((RoomSpace*)board_space)->pet);
	EXPECT_FALSE(((RoomSpace*)board_space)->meeple);
	EXPECT_EQ(player_board->getNumRooms(), 3);

	//test valid conversion to stone room
	EXPECT_TRUE(player_board->upgradeRooms());
	EXPECT_TRUE(player_board->upgradeRooms());
	board_space = player_board->getBoardSpace(1);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	ASSERT_TRUE(player_board->convertSpace(1, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(1);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_EQ(((RoomSpace*)board_space)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(player_board->getNumRooms(), 4);

	//test invalid conversion to room from existing room
	board_space = player_board->getBoardSpace(5);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	ASSERT_FALSE(player_board->convertSpace(5, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(5);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_FALSE(((RoomSpace*)board_space)->pet);
	EXPECT_TRUE(((RoomSpace*)board_space)->meeple);
	EXPECT_EQ(player_board->getNumRooms(), 4);

	//test invalid conversion to room from non-adjoining space
	board_space = player_board->getBoardSpace(7);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	ASSERT_FALSE(((EmptySpace*)board_space)->to_room);
	ASSERT_FALSE(player_board->convertSpace(7, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(7);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	EXPECT_FALSE(((EmptySpace*)board_space)->to_room);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_field);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_pasture);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_stable);
	EXPECT_EQ(player_board->getNumRooms(), 4);
}

TEST_F(GenericPlayerBoard, roomUpgrade) {
	PlayerBoardSpace* room1 = player_board->getBoardSpace(5);
	PlayerBoardSpace* room2 = player_board->getBoardSpace(10);
	player_board->convertSpace(6, PlayerBoardSpace::SpaceType::room);
	PlayerBoardSpace* room3 = player_board->getBoardSpace(6);
	EXPECT_EQ(player_board->getNumRooms(), 3);

	//test upgrade from wood to clay
	ASSERT_EQ(room1->type, PlayerBoardSpace::SpaceType::room);
	ASSERT_EQ(room2->type, PlayerBoardSpace::SpaceType::room);
	ASSERT_EQ(room3->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_EQ(((RoomSpace*)room1)->room_type, RoomSpace::RoomType::wood);
	EXPECT_EQ(((RoomSpace*)room2)->room_type, RoomSpace::RoomType::wood);
	EXPECT_EQ(((RoomSpace*)room3)->room_type, RoomSpace::RoomType::wood);
	EXPECT_TRUE(player_board->upgradeRooms());
	EXPECT_EQ(((RoomSpace*)room1)->room_type, RoomSpace::RoomType::clay);
	EXPECT_EQ(((RoomSpace*)room2)->room_type, RoomSpace::RoomType::clay);
	EXPECT_EQ(((RoomSpace*)room3)->room_type, RoomSpace::RoomType::clay);
	EXPECT_EQ(player_board->getNumRooms(), 3);

	//test upgrade from clay to stone
	EXPECT_TRUE(player_board->upgradeRooms());
	EXPECT_EQ(((RoomSpace*)room1)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(((RoomSpace*)room2)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(((RoomSpace*)room3)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(player_board->getNumRooms(), 3);

	//test invalid upgrade from stone
	ASSERT_FALSE(player_board->upgradeRooms());
	EXPECT_EQ(((RoomSpace*)room1)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(((RoomSpace*)room2)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(((RoomSpace*)room3)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(player_board->getNumRooms(), 3);
}

TEST_F(GenericPlayerBoard, housePet) {
	//test add valid pet
	PlayerBoardSpace* room1 = player_board->getBoardSpace(5);
	PlayerBoardSpace* room2 = player_board->getBoardSpace(10);

	EXPECT_TRUE(
		player_board->addPet(room1->position, PastureSpace::AnimalType::boar));
	EXPECT_TRUE(player_board->hasPet());
	EXPECT_TRUE(((RoomSpace*)room1)->pet);
	EXPECT_FALSE(((RoomSpace*)room2)->pet);
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::boar);

	//test add pet when pet exists
	EXPECT_FALSE(
		player_board->addPet(room2->position, PastureSpace::AnimalType::sheep));
	EXPECT_TRUE(player_board->hasPet());
	EXPECT_TRUE(((RoomSpace*)room1)->pet);
	EXPECT_FALSE(((RoomSpace*)room2)->pet);
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::boar);

	//test add pet to non-room
	EXPECT_FALSE(player_board->addPet(11, PastureSpace::AnimalType::boar));
	EXPECT_TRUE(player_board->hasPet());
	EXPECT_TRUE(((RoomSpace*)room1)->pet);
	EXPECT_FALSE(((RoomSpace*)room2)->pet);
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::boar);

	//test valid move pet
	EXPECT_TRUE(player_board->movePet(room2->position));
	EXPECT_TRUE(player_board->hasPet());
	EXPECT_FALSE(((RoomSpace*)room1)->pet);
	EXPECT_TRUE(((RoomSpace*)room2)->pet);
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::boar);

	//test invalid move to non-room
	EXPECT_FALSE(player_board->movePet(14));
	EXPECT_TRUE(player_board->hasPet());
	EXPECT_FALSE(((RoomSpace*)room1)->pet);
	EXPECT_TRUE(((RoomSpace*)room2)->pet);
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::boar);

	//test remove pet (existing and empty)
	EXPECT_TRUE(player_board->removePet());
	EXPECT_FALSE(player_board->hasPet());
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::none);
	EXPECT_FALSE(((RoomSpace*)room1)->pet);
	EXPECT_FALSE(((RoomSpace*)room2)->pet);
	EXPECT_FALSE(player_board->removePet());

	//test invalid move non-existant pet

}

TEST_F(GenericPlayerBoard, convertSpaceToField) {
	//test valid conversion to first field

	//test valid conversion for additional field

	//test invalid conversion for additional field
}

TEST_F(GenericPlayerBoard, fieldSpace) {
	//test plant grain

	//test plant vegetable

	//test harvest grain/vegetable
}

TEST_F(GenericPlayerBoard, convertSpaceToPasture) {
	//test valid conversion to first pasture of mixed size/orientation

	//test valid conversion to additional pasture of mixed size/orientation

	//test invalid conversion to additional pasture of mixed size/orientation

	//test valid conversion from stable

	//test invalid conversion from stable
}

TEST_F(GenericPlayerBoard, pastureSpace) {
	//test pasture size

	//test pasture capacity

	//test stable

	//test add/remove animal (existing type and conflicting types)
}

TEST_F(GenericPlayerBoard, convertSpaceToStable) {
	//test valid conversion to stable

	//test invalid conversion to stable
}