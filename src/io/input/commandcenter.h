#ifndef IO_INPUT_COMMANDCENTER_H_
#define IO_INPUT_COMMANDCENTER_H_

#include <atomic>
#include "../../common/messagetypes.h"
#include "../../common/events/events.h"
#include "../../gui/menu/basemenu.h"
#include "inputmanager.h"
#include "keyboard.h"

using namespace std;

enum MenuTypeEnum
{
	CoreSelection = 0,
	GlobalSettings,
	CoreSpecific
};

class CommandCenter : public EventObserver
{
private:
	atomic<bool> m_isMenuActive;
	MenuTypeEnum m_menuType = MenuTypeEnum::CoreSelection;
	BaseMenu* m_menu = nullptr;

public:
	// Singleton instance
	static CommandCenter& instance();
	CommandCenter(CommandCenter&&) = delete;						// Disable move constructor (C++11 feature)
	CommandCenter(const CommandCenter& that) = delete; 			// Disable copy constructor (C++11 feature)
	CommandCenter& operator =(CommandCenter const&) = delete;		// Disable assignment operator (C++11 feature)
	virtual ~CommandCenter();

// Helper methods
protected:
	void handleKeyboard(const MInputMessage& message);
	void handleCoreStarted(const CoreStartedEvent& message);

	bool handleMenu(Keyboard& keyboard);
	bool handleGlobalKeys(Keyboard& keyboard);

// Event handlers
protected:
	void onMouse(const EventMessageBase& event);

// EventObserver delegates
public:
	void onMessageEvent(const EventMessageBase& event);

private:
	CommandCenter();		// Disable explicit object creation (only singleton instace allowed)
};

#endif /* IO_INPUT_COMMANDCENTER_H_ */
