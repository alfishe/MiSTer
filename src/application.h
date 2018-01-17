#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "common/events/events.h"

class Application : EventObserver, EventSource
{
public:
	// Singleton instance
	static Application& instance();
	Application(const Application& that) = delete; 			// Copy constructor is forbidden here (C++11 feature)
	Application& operator =(Application const&) = delete;		// Disable assignments
	virtual ~Application();
private:
	Application() {}; 										// Disallow direct instances creation

// Application lifecycle delegates
public:
	void onStart();
	void onTerminate();

	void testEvents();

// EventObserver delegate
public:
	void onMessageEvent(const EventMessageBase& event);
};

#endif /* APPLICATION_H_ */
