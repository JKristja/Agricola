#include <iostream>

using namespace std;

// update cmdline arguments to set players/bots/game mode
// default family game 2 player as mvp
int main()
{
	string userInput;

	do {
		cout << "Waiting for user input... q to quit" << endl;
		cin >> userInput;
	} while (userInput != "q");
	
	return 0;
}