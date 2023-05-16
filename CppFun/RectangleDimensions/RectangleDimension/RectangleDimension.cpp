// This program creates a screen form for user input.
// from the user.
     // Needed to set cursor positions
#include <string>
#include "WinConsoleMethods.h"

struct Vector2
{
	short x;
	short y;
	Vector2() 
	{	
		x = 0;
		y = 0;
	}
	Vector2(int _x, int _y) 
	{
		x = _x;
		y = _y;
	}
};

void displayPrompts(HANDLE, const Vector2&);
void getUserInput(HANDLE, Vector2&, const Vector2&);
void displayData(HANDLE, const Vector2&, const Vector2&);
bool shouldDrawRectangle(HANDLE screen, const Vector2&);

int main()
{
	Vector2 input = Vector2(); // input is a Vector2 structure that has 2 member variables
	Vector2 promptShouldDrawPos = Vector2(0, 25);
	Vector2 promptStartOutputPos = Vector2(3, 25);
	Vector2 rectangleStartOutputPos = Vector2(12, 25);

	// Get the handle to standard output device (the console)
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	bool firstDraw = true;
	bool shouldDrawRect = false;
	while (firstDraw || shouldDrawRect)
	{
		if (firstDraw)
			firstDraw = false;
		if (shouldDrawRect)
			clearConsole(screen);

		displayPrompts(screen, promptStartOutputPos);
		getUserInput(screen, input, promptStartOutputPos);
		displayData(screen, input, rectangleStartOutputPos);
		
		shouldDrawRect = shouldDrawRectangle(screen, promptShouldDrawPos);
	}

	return 0;
}

bool shouldDrawRectangle(HANDLE screen, const Vector2& promptPosition)
{
	char input;
	placeCursor(screen, promptPosition.x, promptPosition.y);
	std::cout << "Do you want to draw another rectangle? Input 'y' or 'Y' to drawn another rectangle. Input any other character to stop drawing....";
	std::cout << "INPUT: ";
	std::cin >> input;
	if (input == 'y' || input == 'Y')
	{
		return true;
	}

	placeCursor(screen, 25, promptPosition.y);
	std::cout << "No more rectangles.";
	return false;
}

/******************************************************
 *                   displayPrompts                   *
 ******************************************************/
void displayPrompts(HANDLE screen, const Vector2& cursorStartPos)
{
	//if (dimensions.length == 0 || dimensions.width == 0)
	//	dimensions = Vector2(25, 8);
	placeCursor(screen, cursorStartPos.x, cursorStartPos.y);
	std::cout << "******* Data Entry Form *******" << std::endl;
	placeCursor(screen, cursorStartPos.x + 2, cursorStartPos.y);
	std::cout << "Length: " << std::endl;
	placeCursor(screen, cursorStartPos.x + 4, cursorStartPos.y);
	std::cout << "Width : " << std::endl;
	placeCursor(screen, cursorStartPos.x + 5, 0);
	std::cout << std::right << std::setfill('.') << std::setw(getConsoleRect(screen).Right) << '.' << std::flush;
}

/******************************************************
 *                    getUserInput                    *
 ******************************************************/
void getUserInput(HANDLE screen, Vector2& input, const Vector2& inputPosOnScreen)
{

	placeCursor(screen, inputPosOnScreen.x + 2, inputPosOnScreen.y + 8);
	std::cin >> input.x;
	std::string invalidInputMessage = "Invalid input. Length/width must both be greater than 1. Please enter new input value. ";
	while (input.x < 2)
	{
		placeCursor(screen, inputPosOnScreen.x + 5, inputPosOnScreen.y);
		std::cout << invalidInputMessage << '\r';
		placeCursor(screen, inputPosOnScreen.x + 2, inputPosOnScreen.y + 8);
		std::cin >> input.x;
	}
	placeCursor(screen, inputPosOnScreen.x + 5, 0);
	clearCurrentConsoleLine(screen);
	placeCursor(screen, inputPosOnScreen.x + 5, inputPosOnScreen.y);
	std::cout << " Valid Input Received ";

	placeCursor(screen, inputPosOnScreen.x + 4, inputPosOnScreen.y + 8);
	std::cin >> input.y;
	while (input.y < 2)
	{
		placeCursor(screen, inputPosOnScreen.x + 5, inputPosOnScreen.y);
		std::cout << invalidInputMessage << '\r';
		placeCursor(screen, inputPosOnScreen.x + 4, inputPosOnScreen.y + 8);
		std::cin >> input.y;
	}
	placeCursor(screen, inputPosOnScreen.x + 5, 0);
	clearCurrentConsoleLine(screen);
	placeCursor(screen, inputPosOnScreen.x + 5, inputPosOnScreen.y);
	std::cout << " Valid Input Received ";

	//assert(("Message: Rectangle length and width must be greater than 1.", input.x > 1 && input.y > 1));
}

/******************************************************
 *                     displayData                    *
 ******************************************************/
void displayData(HANDLE screen, const Vector2& input, const Vector2& rectangleOutputStartPos)
{
	placeCursor(screen, rectangleOutputStartPos.x, rectangleOutputStartPos.y);
	std::cout << "Here is the data you entered.";
	placeCursor(screen, rectangleOutputStartPos.x + 1, rectangleOutputStartPos.y);
	std::cout << "Length  : " << input.x;
	placeCursor(screen, rectangleOutputStartPos.x + 2, rectangleOutputStartPos.y);
	std::cout << "Width   : " << input.y << std::flush;

	// Setup Rect Strings for Each Line
	std::string topAndBottomOutput = "";
	std::string innerRectOutput = "";
	for (int i = 0; i < input.x; i++)
	{
		if( i == input.x - 1)
		{
			topAndBottomOutput.append("*");
			innerRectOutput += '*';
			break;
		}

		if (i == 0 )
		{
			topAndBottomOutput.append("* ");
			innerRectOutput += "* ";
			continue;
		}
		
		topAndBottomOutput += "* ";
		innerRectOutput += "  ";
	}
	
	// Draw Rectangle
	placeCursor(screen, rectangleOutputStartPos.x + 4, rectangleOutputStartPos.y);
	std::cout << topAndBottomOutput;
	for (int i = 0; i < input.y - 2; i++)
	{
		placeCursor(screen, rectangleOutputStartPos.x + 4 + 1 + i, rectangleOutputStartPos.y);
		std::cout << innerRectOutput;
	}
	placeCursor(screen, rectangleOutputStartPos.x + 4 + input.y - 1, rectangleOutputStartPos.y);
	std::cout << topAndBottomOutput << std::endl;
}
