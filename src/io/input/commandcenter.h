#ifndef IO_INPUT_COMMANDCENTER_H_
#define IO_INPUT_COMMANDCENTER_H_

#include "../../common/events/events.h"

using namespace std;

class CommandCenter : public EventObserver
{
public:
	// Singleton instance
	static CommandCenter& instance();
	CommandCenter(CommandCenter&&) = delete;						// Disable move constructor (C++11 feature)
	CommandCenter(const CommandCenter& that) = delete; 			// Disable copy constructor (C++11 feature)
	CommandCenter& operator =(CommandCenter const&) = delete;		// Disable assignment operator (C++11 feature)
	virtual ~CommandCenter();

// EventObserver delegates
public:
	void onMessageEvent(const EventMessageBase& event);

private:
	CommandCenter();		// Disable explicit object creation (only singleton instace allowed)
};

#endif /* IO_INPUT_COMMANDCENTER_H_ */
