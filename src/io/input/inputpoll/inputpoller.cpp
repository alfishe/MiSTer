#include "inputpoller.h"

#include "../../../common/logger/logger.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "../../../3rdparty/tinyformat/tinyformat.h"
#include "../../../common/helpers/collectionhelper.h"
#include "../input.h"
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
		m_eventsBuffer = (epoll_event*)calloc(MAX_INPUT_DEVICES * MAX_INPUT_EVENTS, sizeof(epoll_event));
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
		epoll_event event;
		event.data.fd = fd;
		event.events = EPOLLIN | EPOLLET;

		int res = epoll_ctl(m_fd_epoll, EPOLL_CTL_ADD, fd, &event);
		if (res != -1)
		{
			// Lock parallel threads to access (active till return from method and lock destruction)
			lock_guard<mutex> lock(m_mutexPoll);

			m_devices.insert({fd, device});

			DEBUG("%s: added '%s'", __PRETTY_FUNCTION__, device.model.c_str());
		}
		else
		{
			LOGERROR("%s: unable to add device to epoll '%s'", __PRETTY_FUNCTION__, device.model.c_str());

			close(fd);
		}
	}
	else
	{
		LOGERROR("%s: unable to open device '%s' for polling", __PRETTY_FUNCTION__, path.c_str());
	}
}

void InputPoller::removeInputDevice(InputDevice& device)
{
	// Lock parallel threads to access (active till return from method and lock destruction)
	lock_guard<mutex> lock(m_mutexPoll);

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
	input_event events[MAX_INPUT_EVENTS];

	int len = read(fd, &events, sizeof(events));

	if (len > 0)
	{
		unsigned eventsCount = len / sizeof(input_event);

		TRACE("%s: received: %d raw events", __PRETTY_FUNCTION__, eventsCount);

		unsigned packetStartIdx = 0;
		unsigned idx = 0;

		while (idx < eventsCount)
		{
			// Process single event (everything between event start and EV_SYN)
			for (; idx < eventsCount; idx++)
			{
				input_event& event = events[idx];

				// Determine end of packet
				if (event.type == EV_SYN && event.code == SYN_REPORT)
				{
					int packetLen = idx - packetStartIdx;
					if (packetLen > 0)
					{
						translateEvents(fd, &events[packetStartIdx], (unsigned)packetLen);
					}

					packetStartIdx = idx + 1;
				}
				else if (idx == eventsCount - 1)
				{
					// We've reached end of events list but no synchronization record met
					int packetLen = idx - packetStartIdx;
					if (packetLen > 0)
					{
						translateEvents(fd, &events[packetStartIdx], (unsigned)packetLen);
					}
				}
			}
		}
	}
}

// Translate single logical event from device into higher level messages
void InputPoller::translateEvents(int fd, input_event* events, unsigned numEvents)
{
	if (events == nullptr || numEvents == 0)
	{
		LOGWARN("%s: invalid parameters supplied", __PRETTY_FUNCTION__);
		return;
	}

	// Check if descriptor is registered
	if (!key_exists(m_devices, fd))
	{
		LOGWARN("%s: unknown device fd: 0x%x. No notifications will be created.", __PRETTY_FUNCTION__, fd);
		return;
	}

	// Resolve device type from descriptor
	InputDevice& device = m_devices[fd];
	InputDeviceTypeEnum deviceType = device.type;

	TRACE("Device %s:'%s' received %d event(s), EV_SYN excluded", device.dumpDeviceType().c_str(), device.model.c_str(), numEvents);

	string topic;
	MessagePayloadBase* payload = nullptr;

	switch (deviceType)
	{
		case InputDeviceTypeEnum::Mouse:
			{
				topic = EVENT_MOUSE;
				MInputEvents* event = new MInputEvents();
				createMouseEvent(event, events, numEvents);
				payload = event;
			}
			break;
		case InputDeviceTypeEnum::Keyboard:
			{
				topic = EVENT_KEYBOARD;
				MInputEvents* event = new MInputEvents();
				createKeyboardEvent(event, events, numEvents);
				payload = event;
			}
			break;
		case InputDeviceTypeEnum::Joystick:
			{
				topic = EVENT_JOYSTICK;
				MInputEvents* event = new MInputEvents();
				createJoystickEvent(event, events, numEvents);
				payload = event;
			}
			break;
		default:
			LOGWARN("%s: unable to process events for device type '%s'", __PRETTY_FUNCTION__, device.dumpDeviceType().c_str());
			break;
	};

	// Broadcast event notification
	if (!topic.empty() && payload != nullptr)
	{
		MessageCenter& center = MessageCenter::defaultCenter();
		center.post(topic, this, payload);
	}
}

void InputPoller::createMouseEvent(MInputEvents* mouseEvent, input_event* events, unsigned numEvents)
{
	uint16_t code;
	int32_t value;

	for (unsigned i = 0; i < numEvents; i++)
	{
		input_event& event = events[i];

		code = event.code;
		value = event.value;

		switch (event.type)
		{
			case EV_REL:
				{
					MInputEvent relMoveEvent;
					relMoveEvent.type = RelativeMove;

					bool toAdd = true;
					switch (code)
					{
						case REL_X:
							relMoveEvent.event.relativeMoveEvent.deltaX = value;
							break;
						case REL_Y:
							relMoveEvent.event.relativeMoveEvent.deltaY = value;
							break;
						case REL_WHEEL:
							relMoveEvent.event.relativeMoveEvent.wheelMove = value;
							break;
						default:
							toAdd = false;
							break;
					};

					if (toAdd)
					{
						mouseEvent->push_back(relMoveEvent);
					}
				}
				break;
			case EV_KEY:
				{
					MInputEvent keyEvent;
					keyEvent.type = Key;
					keyEvent.event.keyEvent.key = code;
					keyEvent.event.keyEvent.state = (bool)value;
					mouseEvent->push_back(keyEvent);
				}
				break;
			default:
				// The rest is irrelevant for the mouse
				break;
		}
	}
}

void InputPoller::createKeyboardEvent(MInputEvents* keyboardEvent, input_event* events, unsigned numEvents)
{
	uint16_t code;
	int32_t value;

	for (unsigned i = 0; i < numEvents; i++)
	{
		input_event& event = events[i];

		code = event.code;
		value = event.value;

		switch (event.type)
		{
			case EV_KEY:
				{
					MInputEvent keyEvent;
					keyEvent.type = Key;
					keyEvent.event.keyEvent.key = code;
					keyEvent.event.keyEvent.state = (bool)value;
					keyboardEvent->push_back(keyEvent);
				}
				break;
			case EV_LED:
				break;
			case EV_MSC:
				// Scan codes skipped for now, but can be introduced later
				break;
			default:
				// The rest is irrelevant for the keyboard
				break;
		}
	}
}

void InputPoller::createJoystickEvent(MInputEvents* joystickEvent, input_event* events, unsigned numEvents)
{
	uint16_t code;
	int32_t value;

	for (unsigned i = 0; i < numEvents; i++)
	{
		input_event& event = events[i];

		code = event.code;
		value = event.value;

		switch (event.type)
		{
			case EV_ABS:
				{
					MInputEvent absoluteMoveEvent;
					absoluteMoveEvent.type = AbsoluteMove;
					//absoluteMoveEvent.event.absoluteMoveEvent;
				}
				break;
			case EV_KEY:
				{
					MInputEvent keyEvent;
					keyEvent.type = Key;
					keyEvent.event.keyEvent.key = code;
					keyEvent.event.keyEvent.state = (bool)value;
					joystickEvent->push_back(keyEvent);
				}
				break;
			case EV_LED:
				break;
			default:
				// The rest is irrelevant for the keyboard
				break;
		}
	}
}

// Debug methods
void InputPoller::dumpEPollEvents(input_event* events, unsigned numEvents)
{
	for (unsigned i = 0; i < numEvents; i++)
	{
		input_event& event = events[i];

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
				code = tfm::format("0x%x (%s)", event.code, BaseInputDevice::dumpLEDType(event.code));
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

