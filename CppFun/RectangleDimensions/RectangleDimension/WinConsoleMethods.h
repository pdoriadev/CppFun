#pragma once
#include <iostream>
#include <iomanip>
#include <windows.h> 

COORD getConsoleCursorPosition(HANDLE screen);
_SMALL_RECT getConsoleRect(HANDLE screen);
void placeCursor(HANDLE screen, int row, int col);
void shiftCursorFromCurrentPos(HANDLE screen, int rowShift, int colShift);
void clearCurrentConsoleLine(HANDLE screen);
void clearConsole(HANDLE screen);


COORD getConsoleCursorPosition(HANDLE screen)
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	if (GetConsoleScreenBufferInfo(screen, &bufferInfo))
	{
		return bufferInfo.dwCursorPosition;
	}
	else
	{
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

_SMALL_RECT getConsoleRect(HANDLE screen)
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	if (GetConsoleScreenBufferInfo(screen, &bufferInfo))
	{
		return bufferInfo.srWindow;
	}
	else
	{
		_SMALL_RECT invalid = { 0, 0, 0, 0 };
		return invalid;
	}
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

void shiftCursorFromCurrentPos(HANDLE screen, int rowShift, int colShift)
{
	COORD position = getConsoleCursorPosition(screen);
	position.Y += rowShift;
	position.Y += colShift;
	SetConsoleCursorPosition(screen, position);
}


void clearCurrentConsoleLine(HANDLE screen)
{
	_SMALL_RECT consoleRect = getConsoleRect(screen);
	COORD cursorStartPos = getConsoleCursorPosition(screen);
	placeCursor(screen, cursorStartPos.Y, consoleRect.Left);
	std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << '.' << std::flush;
}

void clearConsole(HANDLE screen)
{
	_SMALL_RECT consoleRect = getConsoleRect(screen);
	placeCursor(screen, consoleRect.Top, consoleRect.Left);
	for (int i = 0; i < consoleRect.Bottom; i++)
	{
		std::cout << std::right << std::setfill(' ') << std::setw(consoleRect.Right) << ' ' << std::flush;
		placeCursor(screen, consoleRect.Top + i, consoleRect.Left);
	}
}