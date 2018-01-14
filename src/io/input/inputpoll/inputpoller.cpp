#include "inputpoller.h"

#include "../../../common/logger/logger.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "../../../3rdparty/tinyformat/tinyformat.h"
#include "../../../common/helpers/collectionhelper.h"
#include "../baseinputdevice.h"

using namespace std;

InputPoller& InputPoller::instance()
{
	static InputPoller instance("input_poll");

	return instance;
}

bool InputPoller::init()
{
	bool result = false;

	// Create epoll instance with estimated number of monitored devices
	m_fd_epoll = epoll_create(MAX_INPUT_DEVICES);
	if (m_fd_epoll != INVALID_FILE_DESCRIPTOR)
	{
		m_eventsBuffer = (epoll_event*)calloc(MAX_INPUT_DEVICES * MAX_INPUT_EVENTS, sizeof(struct epoll_event));
		if (m_eventsBuffer != nullptr)
		{
			m_initialized = true;
		}
		else
		{
			LOGERROR("%s: unable to allocate memory for epoll buffers", __PRETTY_FUNCTION__);
		}
	}
	else
	{
		LOGERROR("%s: unable to create epoll", __PRETTY_FUNCTION__);
	}

	return result;
}

void InputPoller::dispose()
{
	// Stop the thread
	stop();

	// Unsubscribe from epoll and clear devices map
	reset();

	// Close epoll handler
	close(m_fd_epoll);
	m_fd_epoll = INVALID_FILE_DESCRIPTOR;

	// Free up buffer
	if (m_eventsBuffer != nullptr)
	{
		free(m_eventsBuffer);
	}
}

void InputPoller::addInputDevice(InputDevice& device)
{
	string& path = device.path;

	int fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		struct epoll_event event;
		event.data.fd = fd;
		event.events = EPOLLIN | EPOLLET;

		int res = epoll_ctl(m_fd_epoll, EPOLL_CTL_ADD, fd, &event);
		if (res != -1)
		{
			// Lock parallel threads to access (active till return from method and lock destruction)
			lock_guard<mutex> lock(m_mutexPoll);

			m_devices.insert({fd, device});
		}
		else
		{
			LOGERROR("%s: unable to add device to epoll", __PRETTY_FUNCTION__);

			close(fd);
		}
	}
	else
	{
		LOGERROR("%s: unable to open device '%s' for polling", __PRETTY_FUNCTION__, path);
	}
}

void InputPoller::removeInputDevice(InputDevice& device)
{
	pthread_mutex_t* mtx = (pthread_mutex_t*)m_mutexPoll.native_handle();

	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexPoll);

	return;

	int fd = device.fd;
	removeInputDeviceNoLock(fd);

	// Remove from the device map
	m_devices.erase(fd);
}

void InputPoller::removeInputDevice(int fd)
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexPoll);

	removeInputDeviceNoLock(fd);

	// Remove from the device map
	m_devices.erase(fd);
}

void InputPoller::removeInputDeviceNoLock(int fd)
{
	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		if (key_exists(m_devices, fd))
		{
			// Exclude from epoll processing
			struct epoll_event event;
			epoll_ctl(m_fd_epoll, EPOLL_CTL_DEL, fd, &event);

			// Close descriptor itself
			close(fd);
		}
		else
		{
			LOGWARN("%s: unable to find registered device handle", __PRETTY_FUNCTION__);
		}
	}
	else
	{
		LOGWARN("%s: invalid file descriptor passed as parameter", __PRETTY_FUNCTION__);
	}
}

void InputPoller::reset()
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexPoll);

	for (auto it = m_devices.begin(); it != m_devices.end(); it++)
	{
		int fd = it->first;

		removeInputDeviceNoLock(fd);
	}

	// Clear the whole devices map
	m_devices.clear();
}

// Helper methods
void InputPoller::makeNonBlocking(int fd)
{
	if (fd == INVALID_FILE_DESCRIPTOR)
		return;

	int flags = fcntl(fd, F_GETFL, 0);
	if (flags != -1)
	{
		flags |= O_NONBLOCK;
		flags = fcntl(fd, F_SETFL, flags);

		if (flags == -1)
		{
			LOGERROR("%s unable to set flags", __PRETTY_FUNCTION__);
		}
	}
	else
	{
		LOGERROR("%s unable to get flags", __PRETTY_FUNCTION__);
	}
}

int InputPoller::checkEvents()
{
	int result = 0;

	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexPoll);

	// Check if no devices registered - no sense to poll at all
	if (m_devices.empty())
	{
		// Make same sleep to offload CPU
		usleep(pollingInterval * 1000);

		return result;
	}

	int eventNum = epoll_wait(m_fd_epoll, m_eventsBuffer, MAX_INPUT_DEVICES * MAX_INPUT_EVENTS, pollingInterval);

	if (eventNum > 0)
	{
		result = eventNum;

		for (int i = 0; i < eventNum; i++)
		{
			epoll_event& event = m_eventsBuffer[i];
			if (event.events & EPOLLERR)
			{
				int fd = event.data.fd;

				LOGWARN("%s: fd=0x%x reported error during epoll", __PRETTY_FUNCTION__, fd);
			}

			int fd = event.data.fd;
			readEvents(fd);
		}
	}
	else if (eventNum == -1)
	{
		LOGERROR("%s: %s", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}

void InputPoller::readEvents(int fd)
{
	struct input_event events[MAX_INPUT_EVENTS];

	int len = read(fd, &events, sizeof(events));

	if (len > 0)
	{
		TRACE("%s: received: %d", __PRETTY_FUNCTION__, len);

		for (unsigned i = 0; i < len / sizeof(input_event); i++)
		{
			struct input_event& event = events[i];

			string code;
			switch (event.type)
			{
				// Usually generated to specify type of event for keys (MSC_SCAN)
				case EV_MSC:
					code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpMscType(event.code));
					break;
				// Delimiter for separate events (all event records till next EV_SYN belong to a single event)
				case EV_SYN:
					code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpSynType(event.code));
					break;
				case EV_LED:
					code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpLEDBits(event.code));
					break;
				case EV_KEY:
					code = tfm::format("0x%04x (%s)", event.code, BaseInputDevice::dumpKey(event.code));
					break;
				case EV_REL:
					code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpRelType(event.code));
					break;
				case EV_ABS:
					code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpAbsType(event.code));
					break;
				default:
					code = tfm::format("0x%x", event.code);
					break;
			}

			TRACE("Type: 0x%x (%s), code: %s, value: %d",
				event.type,
				BaseInputDevice::dumpEventType(event.type).c_str(),
				code.c_str(),
				event.value
			);

		}
	}
}

// Runnable method
void InputPoller::run()
{
	// Protection against improper use
	if (!m_initialized)
	{
		LOGERROR("InputPoller was not initialized properly. Unable to run async thread.");
		return;
	}
	else
	{
		LOGINFO("InputPoller: thread started with tid: %d (0x%x)", m_thread_id, m_thread_id);
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
			eventsCount += checkEvents();

			eventsCount++;
		}
		catch (const exception& e)
		{
			LOGERROR(e.what());
			errorCount++;
		}

		if (errorCount > 10)
		{
			LOGERROR("%s: Error count exceeded threshold. Stopping the thread...", __PRETTY_FUNCTION__);
			break;
		}

		// No need for additional sleep loop. checkEvents() already has embedded timeout in epoll_wait
	}

	LOGINFO("InputPoller: thread with tid: %d (0x%x) loop stopped]\n    Loop iterations passed: %d", m_thread_id, m_thread_id, loopIterationsCount);
}

