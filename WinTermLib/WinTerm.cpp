#include "WinTerm.h"

//TODO: Additional TODOs in Constructor
//TODO: Throw errors if setting buffer/window size fails instead of just returning (Also in updateBufferInfo(), and writeCharANSI())

namespace WinTermLib
{
	constexpr const char* WinTerm::DefaultWindowTitle;

	/*
		==PUBLIC METHODS==
	*/

	WinTerm::WinTerm(short TermSizeX, short TermSizeY, std::string windowTitle, bool matchBufferToWindowSize)
	{
		//TODO: See if there's a hacky solution to disallowing user to resize window (AFAIK this is impossible with win32 console APIs as it falls under the purview of the user's terminal emulator)
		//TODO: Overload constructor to allow for buffers larger than window size (Remove line below this one)
		matchBufferToWindowSize = true;

		_wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
		_rHnd = GetStdHandle(STD_INPUT_HANDLE);

		// Save current input mode, to be restored on deconstruction of Winterm
		if (!GetConsoleMode(_rHnd, &_oldConsoleInputMode))
		{
			std::cerr << "GetConsoleMode failed with error: " << GetLastError() << std::endl;
			return;
		}

		// Enable window and mouse input events.
		DWORD customInputMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(_rHnd, customInputMode))
		{
			std::cerr << "SetConsoleMode failed with error: " << GetLastError() << std::endl;
			return;
		}

		// Set window title
		std::basic_string<TCHAR> convertedTitle(windowTitle.begin(), windowTitle.end());
		LPCTSTR lpTitle = convertedTitle.c_str();
		SetConsoleTitle(lpTitle);

		// Get initial csbi
		updateBufferInfo();

		// Define the new window size ensuring it does not exceed the maximum window size
		short tempY = min(TermSizeY - 1, _csbi.dwMaximumWindowSize.Y - 1);
		short tempX = min(TermSizeX - 1, _csbi.dwMaximumWindowSize.X - 1);
		_windowSize = { 0, 0, tempX, tempY };

		// Set window size
		if (!SetConsoleWindowInfo(_wHnd, TRUE, &_windowSize)) {
			std::cerr << "SetConsoleWindowInfo failed with error: " << GetLastError() << std::endl;
			return;
		}

		// Create window buffer as COORD for easy access
		_bufferDims = {
			TermSizeX,
			TermSizeY
		};

		// Set window buffer size
		if (!SetConsoleScreenBufferSize(_wHnd, _bufferDims)) {
			std::cerr << "SetConsoleScreenBufferSize failed with error: " << GetLastError() << std::endl;
			return;
		}
		updateBufferInfo();

		// Window Size, Title, and Buffer setup complete!
		// Declare and initialize _chiBuffer
		_chiBufSize = _bufferDims.X * _bufferDims.Y;
		_chiBuffer = new CHAR_INFO[_chiBufSize];

		// Initialize contents of frame buffer to blank characters with black background (To avoid crashes when the user calles flush() with an empty _chiBuffer)
		fillBuffer(' ', DefaultCharacterAttributes);
	}


	void WinTerm::writeCharANSI(char asciiChar, COORD charPos, WORD attributes)
	{
		// Verify that charPos is within the range of window buffer
		if ((charPos.X < 0 || charPos.X >= _bufferDims.X) || (charPos.Y < 0 || charPos.Y >= _bufferDims.Y))
		{
			std::cerr << "writeCharANSI failed." << std::endl
				<< "Buffer dimensions {" << _bufferDims.X << "," << _bufferDims.Y << "} (Tried to write to [" << charPos.X << "][" << charPos.Y << "])" << std::endl;
			return;
		}

		// Convert passed asciiChar to CHAR_INFO
		CHAR_INFO chi;
		chi.Char.AsciiChar = asciiChar;
		chi.Attributes = attributes;

		// Convert charPos coordinates to 1 dimensional _chiBuffer index
		int i = charPos.Y * _bufferDims.X + charPos.X;

		//	Write chi to _chiBuffer @ index i

		_chiBuffer[i] = chi;
	}

	void WinTerm::fillBuffer(char asciiChar, WORD attributes)
	{
		CHAR_INFO chi;
		chi.Char.AsciiChar = asciiChar;
		chi.Attributes = attributes;
		for (int i = 0; i < _chiBufSize; i++)
			_chiBuffer[i] = chi;
	}

	void WinTerm::flush()
	{
		// The write area is a SMALL_RECT is the entire window buffer, and charPos is the coordinate of the inital character to write (Should always be 0,0 in this case)
		SMALL_RECT writeArea = { 0, 0, _bufferDims.X, _bufferDims.Y };
		COORD charPos = { 0, 0 };
		WriteConsoleOutputA(_wHnd, _chiBuffer, _bufferDims, charPos, &writeArea);
	}

	COORD WinTerm::getMaxWindowSize()
	{
		// First get the current Console Screen Buffer Info struct
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(_wHnd, &csbi)) {
			std::cerr << "GetConsoleScreenBufferInfo failed with error: " << GetLastError() << std::endl;
			return { -1,-1 };
		}
		return csbi.dwMaximumWindowSize;
	}

	/*
		==PRIVATE METHODS==
	*/

	BOOL WinTerm::updateBufferInfo()
	{
		// Get the updated console screen buffer info after resizing the buffer
		if (!GetConsoleScreenBufferInfo(_wHnd, &_csbi)) {
			std::cerr << "GetConsoleScreenBufferInfo failed with error: " << GetLastError() << std::endl;
			return FALSE;
		}
		else
			return TRUE;
	}

	DWORD WinTerm::getEventCount()
	{
		return GetNumberOfConsoleInputEvents(_rHnd, &_eventCount);
	}

	void WinTerm::fillEventBuffer()
	{
		/*
			This method assumes _eventCount is NONZERO. The user should never
			interface with this method directly, instead using the public getEventList()
			method that will safely handle situations where there are no events.
		*/

		// Free existing event buffer and recreate it with the current _eventCount size
		delete[] _eventBuffer;
		_eventBuffer = new INPUT_RECORD[_eventCount];

		// Read console input events from _rHnd into _eventBuffer
		DWORD numEventsRead = 0;
		ReadConsoleInput(_rHnd, _eventBuffer, _eventCount, &numEventsRead);
	}
}