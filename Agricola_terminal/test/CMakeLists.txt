cmake_minimum_required(VERSION "3.19.2")

set(THIS ${CMAKE_PROJECT_NAME}_test)

set(SOURCES
	MainTest.cpp
	GameTest.cpp
	CardTest.cpp
	MainBoardTest.cpp
	PlayerBoardTest.cpp
	PlayerTest.cpp
)

add_executable(${THIS} ${SOURCES})

target_link_libraries(${THIS} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest)

add_test(
	NAME ${THIS}
	COMMAND ${THIS}
)

