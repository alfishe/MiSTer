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


	/*
	find_and_execute(m_observers, name,
		// Found lambda
		[=](const string& key, EventObserversVector& observers)
		{
			observers.push_back(observer);
		},
		// Not found lambda
		[=](const string& key)
		{
			m_observers.insert({key, EventObserversVector()});
			m_observers[key].push_back(observer);
		}
	);
	*/

	// Register observer in register map (for faster removal)
	if (!key_exists(m_observersReverse, observer))
	{
		m_observersReverse.insert({observer, StringSet()});
	}
	m_observersReverse[observer].insert(name);

	/*
	find_and_execute(m_observersReverse, observer,
		// Found lambda
		[=](EventObserverPtr& key, StringSet& topics)
		{
			topics.insert(name);
		},
		// Not found lambda
		[=](const EventObserverPtr key)
		{
			m_observersReverse.insert({key, StringSet()});
			m_observersReverse[key].insert(name);
		}
	);
	*/

	// Debug
	/*
	for (auto it : m_observers)
	{
		int count = it.second.size();
		DEBUG("%d observers for the key '%s'", count, it.first.c_str());
	}
	*/
	// -Debug
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

	// Event loop
	while (!m_stop)
	{
		// Count number of iterations passed in event loop
		loopIterationsCount++;

		try
		{

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
