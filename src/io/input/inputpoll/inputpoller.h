#ifndef IO_INPUT_INPUTPOLL_INPUTPOLLER_H_
#define IO_INPUT_INPUTPOLL_INPUTPOLLER_H_

#include <map>
#include <mutex>
#include "../../../common/consts.h"
#include "../../../common/types.h"
#include "../../../common/messagetypes.h"
#include "../../../common/events/events.h"
#include "../../../common/events/messagecenter.h"
#include "../../../common/thread/runnable.h"
#include "../input.h"

typedef struct input_event input_event;
typedef struct epoll_event epoll_event;
typedef map<int, InputDevice> EPollMap;

class InputPoller : public Runnable, EventSource
{
protected:
	atomic<bool> m_initialized;
	mutex m_mutexPoll;

	EPollMap m_devices;
	int m_fd_epoll = INVALID_FILE_DESCRIPTOR;
	epoll_event* m_eventsBuffer = nullptr;

public:
	// Latency is important for fast input events reaction
	int pollingInterval = 2; // 2ms

public:
	// Singleton instance
	static InputPoller& instance();
	InputPoller() = delete;									// Disable explicit object creatib (C++11 feature)
	InputPoller(const InputPoller& that) = delete; 			// Disable copy constructor (C++11 feature)
	InputPoller& operator =(InputPoller const&) = delete;		// Disable assignment operator (C++11 feature)
	virtual ~InputPoller() {};

public:
	bool init();
	void dispose();

public:
	void addInputDevice(InputDevice& device);
	void removeInputDevice(InputDevice& device);
	void removeInputDevice(int fd);
	void reset();

// Helper methods
protected:
	void makeNonBlocking(int fd);
	int checkEvents();
	void readEvents(int fd);
	void translateEvents(int fd, input_event* events, unsigned numEvents);

	void createMouseEvent(MInputEvents* mouseEvent, input_event* events, unsigned numEvents);
	void createKeyboardEvent(MInputEvents* keyboardEvent, input_event* events, unsigned numEvents);
	void createJoystickEvent(MInputEvents* joystickEvent, input_event* events, unsigned numEvents);

	void removeInputDeviceNoLock(int fd);

	// Debug
	void dumpEPollEvents(input_event* events, unsigned numEvents);

// Runnable override method(s)
protected:
	// Async thread body
	void run();

private:
	InputPoller(const string& name)
	{
		m_name = name;
		m_initialized = false;
	}
};

#endif /* IO_INPUT_INPUTPOLL_INPUTPOLLER_H_ */
