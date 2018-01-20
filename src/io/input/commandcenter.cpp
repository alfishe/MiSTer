#include "commandcenter.h"

#include "../../common/logger/logger.h"

#include "../../common/consts.h"
#include "../../common/messagetypes.h"
#include "../../common/events/events.h"
#include "../../common/events/messagecenter.h"

CommandCenter& CommandCenter::instance()
{
	static CommandCenter instance;

	return instance;
}

CommandCenter::CommandCenter()
{
	TRACE("CommandCenter()");

	// Subscribe for input devices events
	MessageCenter& center = MessageCenter::defaultCenter();
	center.addObserver(EVENT_KEYBOARD, this);
	center.addObserver(EVENT_MOUSE, this);
	center.addObserver(EVENT_JOYSTICK, this);
}

CommandCenter::~CommandCenter()
{
	TRACE("~CommandCenter()");

	// Unsubscribe from all notifications
	MessageCenter& center = MessageCenter::defaultCenter();
	center.removeObserver(this);
}


// Runnable delegate
void CommandCenter::onMessageEvent(const EventMessageBase& event)
{
	string eventName = event.topic;

	if (event.payload == nullptr)
	{
	  LOGWARN("%s: notification with name '%s' contains no expected payload", __PRETTY_FUNCTION__, eventName.c_str());
	  return;
	}

	// Extract payload as MInputMessage
	MInputMessage* payload = (MInputMessage*)event.payload;

	TRACE("%s: notification for fd:%x, type:'%s'", __PRETTY_FUNCTION__, payload->deviceID, payload->deviceType._to_string());
}
