cmake_minimum_required(VERSION "3.19.2")

set(HEADERS
	Card.h
	Game.h
	MainBoard.h
	Player.h
	PlayerBoard.h
)

set(SOURCES
	Card.cpp
	Game.cpp
	Main.cpp
	MainBoard.cpp
	Player.cpp
	PlayerBoard.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCES} ${HEADERS})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${SOURCES} DESTINATION src)