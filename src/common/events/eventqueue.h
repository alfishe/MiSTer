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

	mutex m_mutexEvents;
	condition_variable m_cvEvents;


// Data structures
protected:
	EventObserversMap m_observers;
	EventObserversReverseMap m_observersReverse;

	MessageEventsQueue m_events;

// Internal counters
protected:
	int m_processedEvents = 0;
	int m_subscribersCount = 0;
	int m_topicsCount = 0;

public:
	EventQueue();
	virtual ~EventQueue();
	EventQueue(const EventQueue& that) = delete; 			// Disable copy constructor (C++11 feature)
	EventQueue& operator =(EventQueue const&) = delete;		// Disable assignments (C++11 feature)

public:
	bool init();
	void dispose();

public:
	void addObserver(const string& name, const EventObserverPtr& observer);
	void removeObserver(const EventObserverPtr& observer);
	void removeObserver(const string& name, const EventObserverPtr& observer);
	void removeObservers();

	void post(const char* name, const EventSourcePtr source, void* param);
	void post(const string& name, const EventSourcePtr source, void* param);
	void post(const MessageEvent& event);

// Statitic methods
public:
	void resetCounters();

// Debug methods
public:
	string dumpObservers();
	string dumpObserversMap();
	string dumpObserversReverseMap();
	string dumpEventQueue();
protected:
	string dumpEventQueueNoLock();

// Helper methods
protected:
	bool tryPop(MessageEvent& event);
	void pop(MessageEvent& event);
	void clearQueue();

	inline void processEvent(MessageEvent& event) __attribute__((always_inline));

// Runnable override method(s)
protected:
	// Async thread body
	void run();
};

#endif /* COMMON_EVENTS_EVENTQUEUE_H_ */
