#include "eventqueue.h"

#include "../../common/logger/logger.h"

#include <chrono>
#include <unistd.h>
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

	// Lock parallel threads to access
	unique_lock<mutex> lock(m_mutexObservers);
	cv_status res = m_cvObservers.wait_for(lock, 50ms);
	if (res == cv_status::no_timeout)
	{
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

		// Notify next waiting in queue that access is allowed
		m_cvObservers.notify_one();
	}
	else
	{
		LOGWARN("%s: unable to add observer due to lock timeout", __PRETTY_FUNCTION__);
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
