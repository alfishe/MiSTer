#ifndef COMMON_EVENTS_EVENTQUEUE_H_
#define COMMON_EVENTS_EVENTQUEUE_H_

#include <condition_variable>
#include <mutex>

#include "../thread/runnable.h"
#include "events.h"

class EventQueue : public Runnable
{
// Synchronization primitives
protected:
	atomic<bool> m_initialized;
	mutex m_mutexObservers;
	condition_variable m_cvObservers;


// Data structures
protected:
	EventObserversMap m_observers;

// Internal counters
protected:
	int m_processedEvents = 0;
	int m_subscribersCount = 0;

public:
	EventQueue();
	virtual ~EventQueue();
	EventQueue(const EventQueue& that) = delete; 			// Copy constructor is forbidden here (C++11 feature)
	EventQueue& operator =(EventQueue const&) = delete;		// Disable assignments (C++11 feature)

public:
	bool init();
	void dispose();

public:
	void addObserver(const string& name, const EventObserverPtr& observer);
	void removeObserver(const EventObserverPtr& observer);
	void removeObserver(const string& name, const EventObserverPtr& observer);
	void removeObservers();

// Statitic methods
public:
	void resetCounters();

// Runnable override method(s)
protected:
	// Async thread body
	void run();
};

#endif /* COMMON_EVENTS_EVENTQUEUE_H_ */
