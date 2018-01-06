#include "devicedetector.h"

#include "../../../common/logger/logger.h"

#include <poll.h>
#include <unistd.h>
#include <exception>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <limits.h>

using namespace std;

DeviceDetector& DeviceDetector::instance()
{
	static DeviceDetector instance("device_detector");

	return instance;
}

bool DeviceDetector::init()
{
	bool result = false;

	// Create and initialize new inotify instance
	fd_inotify = inotify_init();

	if (fd_inotify != INVALID_FILE_DESCRIPTOR)
	{
		// Subscribe for changes in /dev/input
		wd_inotify = inotify_add_watch(fd_inotify, LINUX_DEVICE_INPUT, IN_MODIFY | IN_CREATE | IN_DELETE);

		if (wd_inotify != INVALID_FILE_DESCRIPTOR)
		{
			m_initialized = true;
			result = true;

			LOGINFO("%s: successfully initialized", __PRETTY_FUNCTION__);
		}
		else
		{
			LOGERROR("%s: Unable to set inotify watch on path: '%s'", __PRETTY_FUNCTION__, LINUX_DEVICE_INPUT);

			dispose();
		}
	}
	else
	{
		LOGERROR("%s: Unable to initialize new inotify instance", __PRETTY_FUNCTION__);
	}

	return result;
}

void DeviceDetector::dispose()
{
	// Stop the thread
	stop();

	// Unsubscribe from inotify events for the watch
	if (wd_inotify != INVALID_FILE_DESCRIPTOR)
	{
		inotify_rm_watch(fd_inotify, wd_inotify);
		wd_inotify = INVALID_FILE_DESCRIPTOR;
	}

	// Free up inotify resources
	if (fd_inotify != INVALID_FILE_DESCRIPTOR)
	{
		close(fd_inotify);
		fd_inotify = INVALID_FILE_DESCRIPTOR;
	}
}

void DeviceDetector::run()
{
	// Protection against improper use
	if (!m_initialized)
	{
		LOGERROR("DeviceDetector was not initialized properly. Unable to run async thread.");
		return;
	}
	else
	{
		LOGINFO("DeviceDetector: thread started with tid: %d (0x%x)", m_thread_id, m_thread_id);
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
			eventsCount = checkEvents();
			if (eventsCount > 0)
			{
				LOGINFO("DeviceDetector: %d new events", eventsCount);

				readEvents();
			}
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

		// No need for additional sleep loop. checkEvents() already has <pollingInterval> polling timeout
		// Sleep until next polling cycle
		//usleep(pollingInterval * 1000);
	}

	LOGINFO("DeviceDetector: thread with tid: %d (0x%x) loop stopped]\n    Loop iterations passed: %d", m_thread_id, m_thread_id, loopIterationsCount);
}

int DeviceDetector::checkEvents()
{
	int result = 0;

	struct pollfd pfd = { fd_inotify, POLLIN, 0 };
	int ret = poll(&pfd, 1, pollingInterval);  // Poll for changes with timeout

	// ret < 0 - Error occured
	// ret == 0 - Timeout occured
	// ret > 0 - Events appeared
	if (ret > 0)
	{
		result = ret;
	}

	/* Same check but via select()
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd_inotify, &rfds);
	result = select(FD_SETSIZE, &rfds, NULL, NULL, 50);
	*/

	return result;
}

bool DeviceDetector::readEvents()
{
	static const size_t MAX_EVENTS = 16;	// Max number of events to process in a single batch
	static const size_t MAX_FILENAME_LENGTH = NAME_MAX + 1; // Max filename length (plus null-termination byte)
	static const size_t EVENT_SIZE = sizeof(struct inotify_event); // Size of one event record
	static const size_t EVENT_BUFFER_SIZE = MAX_EVENTS * (EVENT_SIZE + MAX_FILENAME_LENGTH); // Total buffer size (bytes)

	bool result = false;

	uint8_t buffer[EVENT_BUFFER_SIZE];
	int bytesRead = read(fd_inotify, &buffer, sizeof(buffer));

	if (bytesRead > 0)
	{
		LOGINFO("%s: %d bytes read for inotify event", __PRETTY_FUNCTION__, bytesRead);
		result = processEvents(buffer, sizeof(buffer), bytesRead);
	}
	else if (bytesRead == 0)
	{
		LOGWARN("%s: zero bytes read for inotify event", __PRETTY_FUNCTION__);
	}
	else
	{
		LOGWARN("%s: error while reading for inotify event\n%s\n", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}

bool DeviceDetector::processEvents(uint8_t* buffer, size_t size, size_t bytesRead)
{
	static const size_t EVENT_SIZE = sizeof(struct inotify_event); // Size of one event record

	bool result = false;

	// Sanity validation
	if (buffer == nullptr)
	{
		LOGERROR("%s: Null buffer supplied in parameters", __PRETTY_FUNCTION__);
		return result;
	}

	if (bytesRead > size)
	{
		LOGERROR("%s: bytesRead > size in parameters ", __PRETTY_FUNCTION__);
		return result;
	}
	// -Sanity validation

	MessageCenter& center = MessageCenter::defaultCenter();

	size_t i = 0;
	while (i < bytesRead)
	{
		struct inotify_event* event = (struct inotify_event*)&buffer[i];

		if (event->len > 0)
		{
			if (event->mask & IN_CREATE)
			{
				if (event->mask & IN_ISDIR)
					LOGINFO("The directory %s was created", event->name);
				else
					LOGINFO("The file %s was created with WD %d", event->name, event->wd);

				void* param = new string(event->name); // Allocating here. Should be destroyed by receipient (see InputManager::onMessageEvent())
				center.post(EVENT_DEVICE_INSERTED, this, param);
			}

			if (event->mask & IN_MODIFY)
			{
				if (event->mask & IN_ISDIR)
					LOGINFO( "The directory %s was modified.\n", event->name );
				else
					LOGINFO( "The file %s was modified with WD %d\n", event->name, event->wd);
			}

			if (event->mask & IN_DELETE)
			{
				if (event->mask & IN_ISDIR)
					LOGINFO("The directory %s was deleted.\n", event->name );
				else
					LOGINFO("The file %s was deleted with WD %d\n", event->name, event->wd);

				void* param = new string(event->name); // Allocating here. Should be destroyed by receipient (see InputManager::onMessageEvent())
				center.post(EVENT_DEVICE_REMOVED, this, param);
			}

			// Recalc offset in buffer based on event len
			i += EVENT_SIZE + event->len;
		}
		else
		{
			LOGWARN("%s: inotify_event->len is zero", __PRETTY_FUNCTION__);
			i += EVENT_SIZE;
		}
	}

	return result;
}
