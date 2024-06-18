#include "EventHandler.h"

namespace WinTermLib
{
	std::vector<INPUT_RECORD> IEventHandler::getEventList()
	{
		std::vector<INPUT_RECORD> retVal;
		getEventCount();
		if (_eventCount != 0)
		{
			fillEventBuffer();
			for (int i = 0; i < _eventCount; i++)
				retVal.push_back(_eventBuffer[i]);
		}
		return retVal;
	}

	std::vector<INPUT_RECORD> IEventHandler::getEventsByType(WORD eventType)
	{
		std::vector<INPUT_RECORD> retVal;
		retVal = getEventList();

		// Remove all events that don't match the passed eventType
		retVal.erase(std::remove_if(retVal.begin(), retVal.end(),
			[eventType](INPUT_RECORD ir) { return ir.EventType != eventType; }), retVal.end());
		return retVal;
	}

	bool IEventHandler::getVKeyDown(KEY_EVENT_RECORD ker, DWORD virtualKeyCode)
	{
		if (!virtualKeyCode)	// Default value of virtualKeyCode is 0x00, which resolves to false
		{
			return ker.bKeyDown;
		}
		else
		{
			return ker.wVirtualKeyCode == virtualKeyCode;
		}
	}
}