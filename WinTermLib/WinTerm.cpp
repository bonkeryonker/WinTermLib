#include "WinTerm.h"

namespace WinTermLib
{
	WinTerm::WinTerm(short TermSizeX, short TermSizeY)
	{
		///	TODO: 
		/// Allow for custom window titles to be passed in via constructor

		// Set up handles for read/write
		_wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
		_rHnd = GetStdHandle(STD_INPUT_HANDLE);

		// Set up handle for framebuffer for buffered character display
		// (https://learn.microsoft.com/en-us/windows/console/reading-and-writing-blocks-of-characters-and-attributes?redirectedfrom=MSDN)
		_frameBuffer = CreateConsoleScreenBuffer(
			GENERIC_READ |	// read/write access
			GENERIC_WRITE,
			FILE_SHARE_READ |
			FILE_SHARE_WRITE,	// Shared
			NULL,	// Default security attributes
			CONSOLE_TEXTMODE_BUFFER,	// Must be TEXTMODE
			NULL);	// Reserved; must be NULL
		

		// Change window title
		SetConsoleTitle(TEXT("Win32 Console Example using WinTermLib!"));

		//Set up required window size
		short tempY = TermSizeY - 1;	// Decrement by one to compensate for 0,0 origin
		short tempX = TermSizeX - 1;	// Decrement by one to compensate for 0,0 origin
		_windowSize = { 0, 0, tempY, tempX };
		SetConsoleWindowInfo(_wHnd, TRUE, &_windowSize);
		//SetConsoleWindowInfo(_frameBuffer, TRUE, &_windowSize);

		// Convert window size to the length of a single-dimensional array representing all characters on screen
		// (Will be size of _chiBuffer internally)
		_chiBufSize = TermSizeY * TermSizeX;
		_chiBuffer.resize(_chiBufSize);


		// Change the internal buffer size to match window size
		_bufferSize = { TermSizeY, TermSizeX };
		SetConsoleScreenBufferSize(_wHnd, _bufferSize);
	}

	void WinTerm::fillScreenTest(CHAR_INFO testChar, HANDLE windowHandle)
	{
		for (int y = 0; y < _bufferSize.Y; ++y)
		{
			for (int x = 0; x < _bufferSize.X; ++x)
			{
				_testChiBuf[x + _bufferSize.X * y] = testChar;
			}
		}

		// Write the characters:
		WriteConsoleOutputA(windowHandle, _testChiBuf, _bufferSize, {0,0}, &_windowSize);
	}

	void WinTerm::flush()
	{
		// Copy contents of _framebuffer to _wHnd
		saveHandleToBuffer(_frameBuffer);
		copyBufferToHandle(_wHnd);
		// Copy contents of _wHnd back to _framebuffer
		saveHandleToBuffer(_frameBuffer);
	}

	HANDLE WinTerm::getFrameBuffer()
	{
		return _frameBuffer;
	}

	int WinTerm::saveHandleToBuffer(HANDLE windowHandle)
	{
		BOOL fSuccess = ReadConsoleOutput(
			windowHandle,	// Screen buffer to read from (stdout)
			_chiBuffer.data(),	// Temp buffer to copy into (.data() refers to underlying array used by vector)
			_bufferSize,	// col-row size of _chiBuffer
			{0,0},	// Top left dest. cell in _chiBuffer
			&_windowSize);	// Screen buffer source rectangle

		if (!fSuccess)
		{
			printf("ReadConsoleOutput failed - (%d)\n", GetLastError());
			return 1;
		}
	}

	int WinTerm::copyBufferToHandle(HANDLE windowHandle)
	{
		BOOL fSuccess = WriteConsoleOutput(
			windowHandle,	// Screen buffer to write to
			_chiBuffer.data(),	// Buffer to copy from
			_bufferSize,	// col-row size of _chiBuffer
			{ 0,0 },	// Top left dest. cell in _chiBuffer
			&_windowSize);	//Screen buffer source rectangle
		if (!fSuccess)
		{
			printf("WriteConsoleOutput failed - (%d)\n", GetLastError());
			return 1;
		}
	}
}