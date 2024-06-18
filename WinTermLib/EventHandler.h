/*
	Author: gabu
*/
#pragma once
#include <Windows.h>
#include <vector>

namespace WinTermLib
{
	class IEventHandler
	{
	public:
		static const WORD ANY_KEY = 0x00;
		virtual std::vector<INPUT_RECORD> getEventList();	// User-facing method. Will fill _eventBuffer and return event list as a (potentially empty) vector of INPUT_RECORDs.
		virtual std::vector<INPUT_RECORD> getEventsByType(WORD eventType);

		bool getVKeyDown(KEY_EVENT_RECORD ker, DWORD virtualKeyCode = ANY_KEY);	// If virtualKeyCode is left at its default value, any keypress down returns true
	protected:
		~IEventHandler() { delete[] _eventBuffer; }	// Possibly unnecessary, I think _eventBuffer freed upon EventHandler going out of scope?
		virtual DWORD getEventCount() = 0;	// Store the amount total count of current events. Used when instantiating _eventBuffer
		virtual void fillEventBuffer() = 0;	// Store getEventCount() amount of events into _eventBuffer
		
		INPUT_RECORD* _eventBuffer;
		DWORD _eventCount = 0;
	};
}
