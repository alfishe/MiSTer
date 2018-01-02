#include "runnable.h"

#include "../../common/logger/logger.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

Runnable::~Runnable()
{
	try
	{
		stop();
	}
	catch(...)
	{
		LOGERROR("Error occured while trying to stop thread");
	}
}

void Runnable::start()
{
	if (m_stopped)
	{
		m_stopped = false;
		m_stop = false;

		m_thread = thread(&Runnable::threadStart, this);
	}
	else
	{
		LOGERROR("Unable to start the thread already running");
	}
}

void Runnable::stop()
{
	if (m_thread_id < 0)
		return;

	if (!m_stopped)
	{
		TRACE("Thread tid: %d (0x%x) {%s} requested to stop", m_thread_id, m_thread_id, m_name.c_str());

		// Request thread to stop
		m_stop = true;

		if (m_thread.joinable())
		{
			m_thread.join();
		}

		TRACE("Thread tid: 0x%d (0x%x) {%s} successfully stopped", m_thread_id, m_thread_id, m_name.c_str());
		m_thread_id = -1;

		m_stopped = true;
	}
	else
	{
		LOGWARN("Request to stop already stopped thread arrived. Thread tid: %d (0x%x) {%s}", m_thread_id, m_thread_id, m_name.c_str());
	}
}

void Runnable::threadStart()
{
	// Get GDB compatible thread ID (gettid via syscall)
	m_thread_id = syscall(SYS_gettid);
	TRACE("Started new thread with tid: %d (0x%x) {%s}", m_thread_id, m_thread_id, m_name.c_str());

	// Set thread name
	if (m_name.size() > 0)
	{
		pthread_getname_np(m_thread.native_handle(), (char*)m_name.c_str(), m_name.size());
		prctl(PR_SET_NAME, m_name.c_str(), 0, 0, 0);
	}

	run();

	TRACE("Thread tid: 0x%d (0x%x) {%s} is stopping...", m_thread_id, m_thread_id, m_name.c_str());
}
