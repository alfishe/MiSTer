#ifndef IO_INPUT_DEVICEDETECTOR_H_
#define IO_INPUT_DEVICEDETECTOR_H_

#include <thread>
#include "../../../3rdparty/tinyformat/tinyformat.h"
#include "../../../common/consts.h"
#include "../../../common/messagetypes.h"
#include "../../../common/events/events.h"
#include "../../../common/events/messagecenter.h"
#include "../../../common/thread/runnable.h"
#include "../input.h"

using namespace std;

// Monitors changes in /dev/input to detect input devices connections/disconnections
// Intended to be working as singleton in async mode
class DeviceDetector : public Runnable, EventSource
{
protected:
	atomic<bool> m_initialized;
	int fd_inotify = INVALID_FILE_DESCRIPTOR;
	int wd_inotify = INVALID_FILE_DESCRIPTOR;
public:
	// We can set pretty relaxed timeout here. Input device status change - pretty rare and slow-going event
	int pollingInterval = 200; // 200ms

public:
	// Singleton instance
	static DeviceDetector& instance();
	DeviceDetector() = delete;									// Disable explicit object creatib (C++11 feature)
	DeviceDetector(const DeviceDetector& that) = delete; 			// Disable copy constructor (C++11 feature)
	DeviceDetector& operator =(DeviceDetector const&) = delete;	// Disable assignment operator (C++11 feature)
	virtual ~DeviceDetector() {};

public:
	bool init();
	void dispose();

protected:
	int checkEvents();
	bool readEvents();
	bool processEvents(uint8_t* buffer, size_t size, size_t bytesRead);

// Runnable override method(s)
protected:
	// Async thread body
	void run();

private:
	DeviceDetector(const string& name)
	{
		m_name = name;
		m_initialized = false;
	}
};

#endif /* IO_INPUT_DEVICEDETECTOR_H_ */
