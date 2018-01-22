#include "commandcenter.h"

#include "../../common/logger/logger.h"

#include <linux/input.h>
#include "../../common/consts.h"
#include "../../common/messagetypes.h"
#include "../../common/events/events.h"
#include "../../common/events/messagecenter.h"
#include "../../gui/osd/osd.h"
#include "../../gui/menu/coreselectionmenu.h"

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

	// Specific handler for menu handling
	center.addObserver(EVENT_KEYBOARD, this,
		[](const EventObserver* obj, const EventMessageBase& event)
		{
			if (obj != nullptr)
			{
				CommandCenter* center = (CommandCenter*)obj;

				MInputMessage* message = (MInputMessage*)event.payload;
				if (center != nullptr && message != nullptr)
				{
					center->handleKeyboard(*message);
				}
			}
			else
			{
				LOGERROR("Event handler called but unable to determine CommandCenter instance");
			}
		}
	);

	// Specific handler for tracking FPGA core start
	center.addObserver(EVENT_CORE_STARTED, this,
		[](const EventObserver* obj, const EventMessageBase& event)
		{
			if (obj != nullptr)
			{
				CommandCenter* center = (CommandCenter*)obj;

				CoreStartedEvent* message = (CoreStartedEvent*)event.payload;
				if (center != nullptr && message != nullptr)
				{
					center->handleCoreStarted(*message);
				}
			}
			else
			{
				LOGERROR("Event handler called but unable to determine CommandCenter instance");
			}
		}
	);
}

CommandCenter::~CommandCenter()
{
	TRACE("~CommandCenter()");

	// Unsubscribe from all notifications
	MessageCenter& center = MessageCenter::defaultCenter();
	center.removeObserver(this);
}

// Helper methods

// Step 1: Process menu command(s) (OSD enable/disable + inside menu)
// Step 2: Global hotkeys
// Step 3: Delegate handling to core adapter
void CommandCenter::handleKeyboard(const MInputMessage& message)
{
	auto& events = message.events;
	Keyboard* keyboard = (Keyboard*)InputManager::instance().findInputDeviceByName(message.name);

	for (auto& ev : events)
	{
		if (ev.type == InputEventTypeEnum::Key)
		{
			const KeyEvent& keyEvent = ev.event.keyEvent;

			// Step 1: Process menu command(s)
			bool res = handleMenu(keyboard, keyEvent);

			//if (!res)
			//	res = handleGlobal(keyEvent);
		}
	}
}

void CommandCenter::handleCoreStarted(const CoreStartedEvent& message)
{
	TRACE("%s: received %s event: value: '%s'", __PRETTY_FUNCTION__, EVENT_CORE_STARTED, message.coreName.c_str());
}

// Handle everything related to menu
bool CommandCenter::handleMenu(Keyboard* keyboard, const KeyEvent& keyEvent)
{
	bool result = false;

	if (keyboard == nullptr)
	{
		LOGWARN("%s: empty keyboard object provided as parameter", __PRETTY_FUNCTION__);
		return result;
	}

	// Handle OSD enable/disable

	// TODO: switch to configurable combination
	if (keyEvent.key == KEY_F12 && keyEvent.state)
	{
		// Initiate update for current keys state
		keyboard->pollKeys();

		m_isMenuActive = !m_isMenuActive;

		if (keyboard->isKeyPressed(KEY_RIGHTALT) || keyboard->isKeyPressed(KEY_LEFTALT))
		{
			m_menuType = MenuTypeEnum::CoreSelection;

			CoreSelectionMenu menu = CoreSelectionMenu();
			menu.readAvailableCores();
			auto items = menu.getAvailableCores();

			LOGINFO("%s: %d cores are available", __PRETTY_FUNCTION__, items.size());
			for (int i = 0; i < items.size(); i++)
			{
				LOGINFO("  %s", items[i].displayname.c_str());
			}
		}
		else if (keyboard->isKeyPressed(KEY_LEFTMETA) || keyboard->isKeyPressed(KEY_RIGHTMETA))
		{
			m_menuType = MenuTypeEnum::GlobalSettings;
		}
		else
		{
			m_menuType = MenuTypeEnum::CoreSpecific;
		}
	}

	// Handle in-menu actions
	OSD& osd = OSD::instance();

	if (m_isMenuActive)
	{
		switch (m_menuType)
		{
			CoreSelection:
				{

				}
				break;
			GlobalSettings:
				break;
			CoreSpecific:
				break;
			default:
				break;
		}

		osd.showHighres();
	}
	else
	{
		osd.hide();
	}

	return result;
}


// Event handlers
void CommandCenter::onMouse(const EventMessageBase& event)
{
	TRACE("CommandCenter::onMouse()");
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
