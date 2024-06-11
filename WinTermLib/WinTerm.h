/*
	Author: gabu
	Thanks to http://www.benryves.com/tutorials/?t=winconsole&c=all for the help!
	(And http://www.twitch.tv/northernlion for keeping me company while writing this)
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>

namespace WinTermLib
{
	class WinTerm
	{
	public:
		WinTerm(short TermSizeX, short TermSizeY, bool matchBufferToWindowSize = false);
		~WinTerm() { delete[] _chiBuffer; } // Probably unnecessary, I think _chiBuffer will be automatically freed when WinTerm goes out of scope? Look into this more!
		void writeCharANSI(char asciiChar, COORD charPos = { 0,0 }, WORD attributes = DefaultCharacterAttributes);
		void fillBuffer(char asciiChar, WORD attributes = DefaultCharacterAttributes);	// Fill _chiBuffer entirely with the same character
		void flush();	// Display contents of _chiBuffer to screen (_wHnd)
		HANDLE getFrameBuffer();
		COORD getMaxWindowSize();

		static void clearScreen() { system("cls"); };
		static const WORD DefaultCharacterAttributes = 15U; // White Text on Black Background
	private:
		BOOL updateBufferInfo();	// Update _csbi with latest screen buffer info

		HANDLE _frameBuffer;	// Handle for modifying console contents before writing to stdout
		HANDLE _wHnd;	// Standard Output Write Handle
		HANDLE _rHnd;	// Standard Input Read Handle
		CONSOLE_SCREEN_BUFFER_INFO _csbi;	// Struct to hold data about current Screen Buffer. Not guaranteed to be up-to-date, refresh by calling updateBufferInfo()

		SMALL_RECT _windowSize;	// Size of window

		COORD _bufferDims;	// COORD dimensions of the internal window buffer. Used to iterate through the single dimension _chiBuffer array as though it were 2d
		size_t _chiBufSize;	// Number of elements in _chiBuffer
		CHAR_INFO* _chiBuffer;	// Effectively acts as a frame buffer, written to by all writeChar functions. Contents will be displayed upon calling flush()
	};
}