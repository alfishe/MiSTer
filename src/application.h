#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "common/events/events.h"

class Application : EventObserver, EventSource
{
public:
	static Application& instance();							// Singleton instance
	Application(Application&&) = delete;						// Disable move constructor (C++11 feature)
	Application(const Application& that) = delete; 			// Disable copy constructor (C++11 feature)
	Application& operator =(Application const&) = delete;		// Disable assignment operator (C++11 feature)
	virtual ~Application();
private:
	Application() {}; 										// Disallow direct instances creation

// Application lifecycle delegates
public:
	void onStart();
	void onTerminate();

// Helper methods
protected:
	void startupDiagnostics();

// Debug methhods
	void testEvents();

// EventObserver delegate
public:
	void onMessageEvent(const EventMessageBase& event);
};

#endif /* APPLICATION_H_ */
