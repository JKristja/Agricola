#include "gtest/gtest.h"
#include "PlayerBoard.h"

struct GenericPlayerBoard : public ::testing::Test {
	PlayerBoard* player_board;
	PlayerBoardSpace* board_space;
	RoomSpace* room1;
	RoomSpace* room2;

	virtual void SetUp() {
		player_board = new PlayerBoard();
		room1 = (RoomSpace*) player_board->getBoardSpace(5);
		room2 = (RoomSpace*) player_board->getBoardSpace(10);
	}

	virtual void TearDown() {
		delete player_board;
	}
};

TEST_F(GenericPlayerBoard, boardSetUp) {
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
	board_space = player_board->getBoardSpace(0);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	ASSERT_TRUE(((EmptySpace*)board_space)->to_room);
	ASSERT_TRUE(
		player_board->convertSpace(0, PlayerBoardSpace::SpaceType::room));
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
	ASSERT_TRUE(
		player_board->convertSpace(1, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(1);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_EQ(((RoomSpace*)board_space)->room_type, RoomSpace::RoomType::stone);
	EXPECT_EQ(player_board->getNumRooms(), 4);

	//test invalid conversion to room from existing room
	board_space = player_board->getBoardSpace(room1->position);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	ASSERT_FALSE(player_board->convertSpace(room1->position,
		PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(room1->position);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_FALSE(((RoomSpace*)board_space)->pet);
	EXPECT_TRUE(((RoomSpace*)board_space)->meeple);
	EXPECT_EQ(player_board->getNumRooms(), 4);

	//test invalid conversion to room from non-adjoining space
	board_space = player_board->getBoardSpace(7);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	ASSERT_FALSE(((EmptySpace*)board_space)->to_room);
	ASSERT_FALSE(
		player_board->convertSpace(7, PlayerBoardSpace::SpaceType::room));
	board_space = player_board->getBoardSpace(7);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	EXPECT_FALSE(((EmptySpace*)board_space)->to_room);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_field);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_pasture);
	EXPECT_TRUE(((EmptySpace*)board_space)->to_stable);
	EXPECT_EQ(player_board->getNumRooms(), 4);
}

TEST_F(GenericPlayerBoard, roomUpgrade) {
	player_board->convertSpace(6, PlayerBoardSpace::SpaceType::room);
	PlayerBoardSpace* room3 = player_board->getBoardSpace(6);
	ASSERT_EQ(room3->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_EQ(player_board->getNumRooms(), 3);

	//test upgrade from wood to clay
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
	EXPECT_FALSE(player_board->movePet(room1->position));
	EXPECT_FALSE(player_board->hasPet());
	EXPECT_EQ(player_board->getPetType(), PastureSpace::AnimalType::none);
	EXPECT_FALSE(((RoomSpace*)room1)->pet);
	EXPECT_FALSE(((RoomSpace*)room2)->pet);
}

TEST_F(GenericPlayerBoard, convertSpaceToField) {
	//test valid conversion to first field
	EXPECT_EQ(player_board->getNumFields(), 0);
	EXPECT_TRUE(
		player_board->convertSpace(4, PlayerBoardSpace::SpaceType::field));
	board_space = player_board->getBoardSpace(4);
	EXPECT_EQ(player_board->getNumFields(), 1);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::field);
	EXPECT_EQ(((FieldSpace*)board_space)->field_type,
		FieldSpace::FieldType::empty);
	EXPECT_EQ(((FieldSpace*)board_space)->quantity, 0);
	EXPECT_TRUE(((EmptySpace*)(player_board->getBoardSpace(3)))->to_field);
	EXPECT_TRUE(((EmptySpace*)(player_board->getBoardSpace(9)))->to_field);
	for (int i = 0; i < 15; i++) {
		if (i != 3 && i != 9) {
			board_space = player_board->getBoardSpace(i);
			if (board_space->type == PlayerBoardSpace::SpaceType::empty)
				EXPECT_FALSE(((EmptySpace*)board_space)->to_field);
		}
	}

	//test valid conversion for additional field
	EXPECT_TRUE(
		player_board->convertSpace(9, PlayerBoardSpace::SpaceType::field));
	board_space = player_board->getBoardSpace(9);
	EXPECT_EQ(player_board->getNumFields(), 2);
	ASSERT_EQ(board_space->type, PlayerBoardSpace::SpaceType::field);
	EXPECT_EQ(((FieldSpace*)board_space)->field_type,
		FieldSpace::FieldType::empty);
	EXPECT_EQ(((FieldSpace*)board_space)->quantity, 0);
	EXPECT_TRUE(((EmptySpace*)(player_board->getBoardSpace(8)))->to_field);
	EXPECT_TRUE(((EmptySpace*)(player_board->getBoardSpace(14)))->to_field);

	//test invalid conversion for additional field from non-empty
	EXPECT_FALSE(player_board->convertSpace(room1->position,
		PlayerBoardSpace::SpaceType::field));
	EXPECT_EQ(room1->type, PlayerBoardSpace::SpaceType::room);
	EXPECT_EQ(player_board->getNumFields(), 2);

	//test invalid conversion for additional field not-adjacent
	EXPECT_FALSE(
		player_board->convertSpace(7, PlayerBoardSpace::SpaceType::field));
	board_space = player_board->getBoardSpace(7);
	EXPECT_EQ(board_space->type, PlayerBoardSpace::SpaceType::empty);
	EXPECT_FALSE(((EmptySpace*)board_space)->to_field);
	EXPECT_EQ(player_board->getNumFields(), 2);
}

TEST_F(GenericPlayerBoard, fieldSpace) {
	ASSERT_TRUE(
		player_board->convertSpace(8, PlayerBoardSpace::SpaceType::field));
	ASSERT_TRUE(
		player_board->convertSpace(3, PlayerBoardSpace::SpaceType::field));
	
	FieldSpace* field1 = (FieldSpace*) player_board->getBoardSpace(8);
	FieldSpace* field2 = (FieldSpace*) player_board->getBoardSpace(3);

	//test valid sow grain/vegetable
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::empty);
	EXPECT_EQ(field1->quantity, 0);
	EXPECT_TRUE(field1->sow(FieldSpace::FieldType::grain));
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 3);
	EXPECT_EQ(field2->field_type, FieldSpace::FieldType::empty);
	EXPECT_EQ(field2->quantity, 0);
	EXPECT_TRUE(field2->sow(FieldSpace::FieldType::vegetable));
	EXPECT_EQ(field2->field_type, FieldSpace::FieldType::vegetable);
	EXPECT_EQ(field2->quantity, 2);

	//test invalid sow grain/vegetable
	EXPECT_FALSE(field1->sow(FieldSpace::FieldType::empty));
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 3);
	EXPECT_FALSE(field1->sow(FieldSpace::FieldType::grain));
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 3);
	EXPECT_FALSE(field2->sow(FieldSpace::FieldType::grain));
	EXPECT_EQ(field2->field_type, FieldSpace::FieldType::vegetable);
	EXPECT_EQ(field2->quantity, 2);

	//test harvest grain/vegetable mixed with sow
	EXPECT_EQ(field1->harvest(), FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 2);
	EXPECT_EQ(field1->harvest(), FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 1);
	EXPECT_FALSE(field1->sow(FieldSpace::FieldType::grain));
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->quantity, 1);
	EXPECT_EQ(field1->harvest(), FieldSpace::FieldType::grain);
	EXPECT_EQ(field1->field_type, FieldSpace::FieldType::empty);
	EXPECT_EQ(field1->quantity, 0);
	EXPECT_EQ(field2->harvest(), FieldSpace::FieldType::vegetable);
	EXPECT_EQ(field2->field_type, FieldSpace::FieldType::vegetable);
	EXPECT_EQ(field2->quantity, 1);
	EXPECT_EQ(field2->harvest(), FieldSpace::FieldType::vegetable);
	EXPECT_EQ(field2->field_type, FieldSpace::FieldType::empty);
	EXPECT_EQ(field2->quantity, 0);
}

TEST_F(GenericPlayerBoard, convertSpaceToPasture) {
	//test valid conversion to first pasture
	ASSERT_TRUE(
		player_board->convertSpace(6, PlayerBoardSpace::SpaceType::pasture));
	EXPECT_EQ(player_board->getNumPastures(), 1);
	EXPECT_EQ(player_board->getAnimalCapacity(), 3);
	board_space = player_board->getBoardSpace(6);
	EXPECT_FALSE(((PastureSpace*)board_space)->stable);
	EXPECT_EQ(((PastureSpace*)board_space)->capacity, 2);
	EXPECT_EQ(((PastureSpace*)board_space)->quantity, 0);
	EXPECT_EQ(((PastureSpace*)board_space)->animal_type, 
		PastureSpace::AnimalType::none);
	vector<PastureSpace*> pasture1;
	pasture1.push_back((PastureSpace*)board_space);
	EXPECT_EQ(((PastureSpace*)board_space)->linked_pastures, pasture1);

	//test valid conversion to additional pasture of mixed size/orientation

	//test invalid conversion to additional pasture of mixed size/orientation

	//test valid conversion from stable with animal

	//test valid conversion from stable without animal

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

TEST_F(GenericPlayerBoard, stableSpace) {
	//test valid add/remove animal

	//test invalid add/remove animal
}