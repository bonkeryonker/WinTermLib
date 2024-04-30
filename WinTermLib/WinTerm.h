/*
	Author: gabu
	Thanks to http://www.benryves.com/tutorials/?t=winconsole&c=all for the help!
	(And http://www.twitch.tv/northernlion for keeping me company while writing this)
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>

namespace WinTermLib
{
	class WinTerm
	{
	public:
		WinTerm(short TermSizeX, short TermSizeY);
		void writeCharANSI(char asciiChar, WORD attributes = DefaultCharacterAttributes, COORD charPos = { 0,0 });
		void fillScreenTest(CHAR_INFO testChar, HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE));
		void flush();
		HANDLE getFrameBuffer();
		static void clearScreen() { system("cls"); };
		static const WORD DefaultCharacterAttributes = 15U; // White Text on Black Background
	private:
		HANDLE _frameBuffer;	// Handle for modifying console contents before writing to stdout
		HANDLE _wHnd;	// Standard Output Write Handle
		HANDLE _rHnd;	// Standard Input Read Handle
		SMALL_RECT _windowSize;
		COORD _bufferSize;
		CHAR_INFO _testChiBuf[160 * 90];
		std::vector<CHAR_INFO> _chiBuffer;	// Temporary buffer to store CHAR_INFO data. Used to read/write between _framebuffer and stdout
		size_t _chiBufSize;

		int saveHandleToBuffer(HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE));	// Method to copy the contents of the current console screen to the _frameBuffer variable (via _chiBuffer)
		int copyBufferToHandle(HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE));	// Method to copy the contents of the framebuffer to the current console screen
	};
}