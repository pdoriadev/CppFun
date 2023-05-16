// This program creates a screen form for user input.
// from the user.
#include <iostream>
#include <windows.h>      // Needed to set cursor positions
#include <string>
using namespace std;

struct rectDim
{	int length;
	int width;
};

void placeCursor(HANDLE, int, int);   // Function prototypes
void displayPrompts(HANDLE);
void getUserInput(HANDLE, rectDim&);
void displayData (HANDLE, rectDim);

int main()
{
	rectDim input;              // input is a rectDim structure
	                           // that has 2 member variables

	// Get the handle to standard output device (the console)
    HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);

	displayPrompts(screen);
	getUserInput(screen, input);
	displayData (screen, input);

	return 0;
}

/******************************************************
 *                    placeCursor                     *
 ******************************************************/
void placeCursor(HANDLE screen, int row, int col)
{                       // COORD is a defined C++ structure that
    COORD position;     // holds a pair of X and Y coordinates
	position.Y = row;
	position.X = col;
    SetConsoleCursorPosition(screen, position);
}

/******************************************************
 *                   displayPrompts                   *
 ******************************************************/
void displayPrompts(HANDLE screen)
{
	placeCursor(screen, 3, 25);
	cout << "******* Data Entry Form *******" << endl;
	placeCursor(screen, 5, 25);
	cout << "Length: " << endl;
	placeCursor(screen, 7, 25);
	cout << "Width : " << endl;
}

/******************************************************
 *                    getUserInput                    *
 ******************************************************/
void getUserInput(HANDLE screen, rectDim &input)
{
	placeCursor(screen, 5, 33);
	cin >> input.length;
	placeCursor(screen, 7, 33);
	cin >> input.width;
}

/******************************************************
 *                     displayData                    *
 ******************************************************/
void displayData(HANDLE screen, rectDim input)
{
	placeCursor(screen, 10, 0);
	cout << "Here is the data you entered.\n";
	cout << "Length  : " << input.length   << endl;
	cout << "Width   : " << input.width    << endl;
}
