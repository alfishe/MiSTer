#include "eventqueue.h"

#include "../../common/logger/logger.h"

#include <chrono>
#include <sstream>
#include <unistd.h>
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../helpers/collectionhelper.h"

using namespace chrono;
using namespace std::chrono_literals;

EventQueue::EventQueue()
{
	m_name = "eventqueue";

	init();
}

EventQueue::~EventQueue()
{
	if (m_initialized)
	{
		dispose();
	}
}

bool EventQueue::init()
{
	bool result = false;

	m_initialized = true;

	return result;
}

void EventQueue::dispose()
{
	stop();
}

// Public methods
void EventQueue::addObserver(const string& name, const EventObserverPtr& observer)
{
	if (name.empty())
	{
		LOGWARN("%s: Empty name parameter supplied", __PRETTY_FUNCTION__);
		return;
	}

	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexObservers);

	// Add observer to correspondent topic in forward map
	if (!key_exists(m_observersReverse, observer))
	{
		m_observers.insert({name, EventObserversSet()});
	}
	m_observers[name].insert(observer);

	// Register observer in register map (for faster removal)
	if (!key_exists(m_observersReverse, observer))
	{
		m_observersReverse.insert({observer, StringSet()});
	}
	m_observersReverse[observer].insert(name);
}

void EventQueue::removeObserver(const EventObserverPtr& observer)
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexObservers);

	// Perform observer lookup in reverse map
	if (key_exists(m_observersReverse, observer))
	{
		// Remove from forward map
		auto& topics = m_observersReverse[observer];
		for (auto topic : topics)
		{
			if (key_exists(m_observers, topic))
			{
				auto& observers = m_observers[topic];
				observers.erase(observers.find(observer));
			}

			erase_entry_if_empty(m_observers, topic);
		}

		// Remove from reverse map
		m_observersReverse.erase(observer);
	}
}

void EventQueue::removeObserver(const string& name, const EventObserverPtr& observer)
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexObservers);

	// Remove from forward map
	if (key_exists(m_observers, name))
	{
		auto& observers = m_observers[name];
		observers.erase(observers.find(observer));

		erase_entry_if_empty(m_observers, name);
	}

	// Remove from reverse map
	if (key_exists(m_observersReverse, observer))
	{
		auto& topics = m_observersReverse[observer];
		topics.erase(topics.find(name));

		erase_entry_if_empty(m_observersReverse, observer);
	}
}

void EventQueue::removeObservers()
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexObservers);

	m_observers.clear();
	m_observersReverse.clear();
}

void EventQueue::post(const EventMessageBase& event)
{
	//DEBUG("Event queue before push\n%s", dumpEventQueue().c_str());

	// Lock parallel threads to access
	unique_lock<mutex> lock(m_mutexEvents);
	m_events.push_front(event);
	lock.unlock();

	m_cvEvents.notify_one();

	//DEBUG("Event queue after push\n%s", dumpEventQueue().c_str());
}

// Debug methods
string EventQueue::dumpObservers()
{
	stringstream ss;

	ss << dumpObserversMap();
	ss << '\n';
	ss << dumpObserversReverseMap();

	string result = ss.str();
	return result;
}

string EventQueue::dumpObserversMap()
{
	stringstream ss;

	ss << tfm::format("Forward map has: %d topics", m_observers.size());
	ss << '\n';

	for (auto it : m_observers)
	{
		int count = it.second.size();
		ss << tfm::format("  %d observers for the topic '%s'", count, it.first.c_str());
		ss << '\n';

		if (count > 0)
		{
			for (auto observer : it.second)
			{
				ss << "    0x" << observer;
				ss << '\n';
			}
		}
	}

	string result = ss.str();
	return result;
}

string EventQueue::dumpObserversReverseMap()
{
	stringstream ss;

	ss << tfm::format("Reverse map has: %d observers", m_observersReverse.size());
	ss << '\n';

	for (auto it : m_observersReverse)
	{
		int count = it.second.size();
		ss << tfm::format("  %d topics for the observer 0x%x", count, it.first);
		ss << '\n';

		if (count > 0)
		{
			for (auto topic : it.second)
			{
				ss << "    '" << topic << "'";
				ss << '\n';
			}
		}
	}

	string result = ss.str();
	return result;
}

string EventQueue::dumpEventQueue()
{
	// Lock parallel threads to access (unique_lock allows arbitrary lock/unlock)
	lock_guard<mutex> lock(m_mutexEvents);

	return dumpEventQueueNoLock();
}

// Version to call from already locked access code
string EventQueue::dumpEventQueueNoLock()
{
	stringstream ss;

	ss << tfm::format("Event queue contains: %d messages", m_events.size());

	if (m_events.size() > 0)
		ss << '\n';

	for (auto event : m_events)
	{
		ss << tfm::format("{'%s', 0x%x}", event.topic.c_str(), event.source);
		ss << '\n';
	}

	string result = ss.str();
	return result;
}

// Helper methods
bool EventQueue::tryPop(EventMessageBase& event)
{
	bool result = true;

	unique_lock<mutex> lock(m_mutexEvents);
	if (m_cvEvents.wait_for(lock, 50ms,
		[&]()
		{
			return m_events.empty() || m_stop;
		}
	));

	if (m_events.empty() || m_stop)
	{
		result = false;

		//if (m_events.empty())
		//	DEBUG("%s: queue is empty, nothing to fetch", __PRETTY_FUNCTION__);
	}
	else
	{
		//DEBUG("Event queue before pop\n%s", dumpEventQueueNoLock().c_str());

		event = m_events.back();
		m_events.pop_back();

		lock.unlock();

		//DEBUG("Event queue after pop\n%s", dumpEventQueueNoLock().c_str());
	}

	return result;
}

void EventQueue::pop(EventMessageBase& event)
{
	unique_lock<mutex> lock(m_mutexEvents);
	if (m_cvEvents.wait_for(lock, 50ms,
		[&]()
		{
			return m_events.empty() || m_stop;
		}
	));

	event = m_events.back();
	m_events.pop_back();

	lock.unlock();
}

void EventQueue::clearQueue()
{
	// Clearing queue by swapping queue to an empty one
	lock_guard<mutex> lock(m_mutexEvents);
	m_events = MessageEventsQueue();
}

// Broadcast single event to subscribers
void EventQueue::processEvent(EventMessageBase& event)
{
	string& name = event.topic;
	EventObserversSet observers;

	// Locked access to collection of subscribers
	unique_lock<mutex> lock(m_mutexObservers);
	if (key_exists(m_observers, name))
	{
		// Make local copy not to block observers collection(s) access for a long
		observers = m_observers[name];
	}
	lock.unlock();
	// -Locked access to collection of subscribers

	if (observers.size() > 0)
	{
		int observersProcessed = 0;
		int errorCount = 0;

		for (auto observer : observers)
		{
			try
			{
				observer->onMessageEvent(&event);
				observersProcessed++;
			}
			catch (const exception& e)
			{
				errorCount++;
				LOGERROR("Exception in %s: %s\n", __PRETTY_FUNCTION__, e.what());
			}
		}

		DEBUG("%s: Event for topic '%s' processing finished. Observers served: %d, errors: %d\n", __PRETTY_FUNCTION__, name.c_str(), observersProcessed, errorCount);
	}
	else
	{
		LOGWARN("%s: No subscribers for topic '%s'. Dropping the message\n", __PRETTY_FUNCTION__, name.c_str());
	}
}

// Runnable methods
void EventQueue::run()
{
	// Protection against improper use
	if (!m_initialized)
	{
		LOGERROR("EventQueue was not initialized properly. Unable to run async thread.");
		return;
	}
	else
	{
		LOGINFO("EventQueue: thread started with tid: %d (0x%x)", m_thread_id, m_thread_id);
	}

	int loopIterationsCount = 0;
	int eventsCount = 0;
	int errorCount = 0;

	// Temporary holder instance
	EventMessageBase event;

	// Event loop
	while (!m_stop)
	{
		// Count number of iterations passed in event loop
		loopIterationsCount++;

		try
		{
			if (tryPop(event))
			{
				eventsCount++;

				processEvent(event);
			}

			// Make 1ms pause returning control to OS
			usleep(1 * 1000);
		}
		catch (const exception& e)
		{
			errorCount++;
		}
	}

	LOGINFO("EventQueue: thread with tid: %d (0x%x) loop stopped]\n    Loop iterations passed: %d", m_thread_id, m_thread_id, loopIterationsCount);
}

// Statitic methods
void EventQueue::resetCounters()
{
	m_processedEvents = 0;
	m_subscribersCount = 0;
}
