#pragma once
#include <iostream>
#include <iomanip>
#include <windows.h> 
#include <assert.h>

class outputController
{
private:
	HANDLE screen;
	COORD lastSetPosition;
	COORD unsetCoordValue;

	HANDLE getConsoleScreenWindow()
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}

	COORD getCoordConsoleCursorPosition()
	{
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		if (GetConsoleScreenBufferInfo(screen, &bufferInfo))
		{
			return bufferInfo.dwCursorPosition;
		}
		else
		{
			assert(("Invalid result from GetConsoleScreenBufferInfo(HANDLE, CONSOLE_SCREEN_BUFFER_INFO)", false));
			COORD invalid = { 0, 0 };
			return invalid;
		}
	}


	_SMALL_RECT getConsoleRect()
	{
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		if (GetConsoleScreenBufferInfo(screen, &bufferInfo))
		{
			return bufferInfo.srWindow;
		}
		else
		{
			assert(("Invalid result from GetConsoleScreenBufferInfo(HANDLE, CONSOLE_SCREEN_BUFFER_INFO)", false));
			_SMALL_RECT invalid = { 0, 0, 0, 0 };
			return invalid;
		}
	}

	COORD getConsoleSize()
	{

	}

public: 
	outputController()
	{
		screen = getConsoleScreenWindow();
		unsetCoordValue.X = -13108;
		unsetCoordValue.Y = -13108;
	}

	bool equalsUnsetCoord(COORD value)
	{
		if (value.X == unsetCoordValue.X && value.Y == unsetCoordValue.Y)
		{
			return true;
		}

	}

	COORD getCursorPosition()
	{
		return getCoordConsoleCursorPosition();		
	}

	COORD getLastSetPosition()
	{
		return lastSetPosition;
	}

	void placeCursor(const int row, const int col)
	{                       // COORD is a defined C++ structure that
		COORD position;     // holds a pair of X and Y coordinates
		position.Y = row;
		position.X = col;
		SetConsoleCursorPosition(screen, position);
		lastSetPosition = position;
	}

	COORD placeCursorAtLastSetPosition()
	{
		SetConsoleCursorPosition(screen, lastSetPosition);
		return lastSetPosition;
	}

	void up()
	{
		shiftCursorFromCurrentPos(-1, 0);
	}

	void down()
	{
		shiftCursorFromCurrentPos(1, 0);
	}

	void right()
	{
		shiftCursorFromCurrentPos(0, 1);
	}

	void left()
	{
		shiftCursorFromCurrentPos(0, -1);
	}

	COORD CalculateCOORDValueAsIfShifted(COORD coordToBeShifted, const int rowShift, const int colShift)
	{
		coordToBeShifted.Y += rowShift;
		coordToBeShifted.X += colShift;
		return coordToBeShifted;
	}

	COORD shiftCursorFromCurrentPos(const int rowShift, const int colShift)
	{
		COORD position = getCoordConsoleCursorPosition();
		position.Y += rowShift;
		position.X += colShift;
		SetConsoleCursorPosition(screen, position);
		
		lastSetPosition = position;
		return lastSetPosition;
	}

	COORD shiftCursorFromLastSetPos(const int rowShift, const int colShift)
	{
		lastSetPosition.Y += rowShift;
		lastSetPosition.X += colShift;
		SetConsoleCursorPosition(screen, lastSetPosition);

		return lastSetPosition;
	}

	void clearLine()
	{
		_SMALL_RECT consoleRect = getConsoleRect();
		COORD cursorStartPos = getCoordConsoleCursorPosition();
		placeCursor(cursorStartPos.Y, consoleRect.Left);
		std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << ' ' << std::flush;
		placeCursor(cursorStartPos.Y, cursorStartPos.X);
	}

	void clearToRightOnLine()
	{
		_SMALL_RECT consoleRect = getConsoleRect();
		COORD cursorStartPos = getCoordConsoleCursorPosition();
		std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right - cursorStartPos.X) << ' ' << std::flush;
		placeCursor(cursorStartPos.Y, cursorStartPos.X);
	}

	void clearToRightOnLineFromPos(COORD pos)
	{
		_SMALL_RECT consoleRect = getConsoleRect();
		COORD cursorPosBeforeClear = getCoordConsoleCursorPosition();
		
		placeCursor(pos.Y, pos.X);
		std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right - pos.X) << ' ' << std::flush;
		placeCursor(cursorPosBeforeClear.Y, cursorPosBeforeClear.X);
	}

	void clearBelowGivenLine(int rowLine)
	{
		COORD cursorBeforeClearing = getCoordConsoleCursorPosition();
		_SMALL_RECT consoleRect = getConsoleRect();

		placeCursor(rowLine + 1, 0);
		for (int i = 0; i < consoleRect.Bottom; i++)
		{
			std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << ' ' << std::flush;
			placeCursor(getCoordConsoleCursorPosition().Y, getCoordConsoleCursorPosition().X);
		}
		placeCursor(cursorBeforeClearing.Y, cursorBeforeClearing.X);

	}

	void clearBelowCurrentLine()
	{
		COORD cursorBeforeClearing = getCoordConsoleCursorPosition();
		_SMALL_RECT consoleRect = getConsoleRect();

		placeCursor(cursorBeforeClearing.Y + 1, 0);
		for (int i = 0; i < consoleRect.Bottom; i++)
		{
			std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << ' ' << std::flush;
			placeCursor(getCoordConsoleCursorPosition().Y, getCoordConsoleCursorPosition().X);
		}
		placeCursor(cursorBeforeClearing.Y, cursorBeforeClearing.X);
	}

	void clearConsole()
	{
		_SMALL_RECT consoleRect = getConsoleRect();
		placeCursor(consoleRect.Top, consoleRect.Left);
		for (int i = 0; i < consoleRect.Bottom; i++)
		{
			std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << ' ' << std::flush;
			placeCursor(consoleRect.Top + i, consoleRect.Left);
		}
	}
};
