#ifndef COMMON_EVENTS_MESSAGECENTER_H_
#define COMMON_EVENTS_MESSAGECENTER_H_

#include <atomic>
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "eventqueue.h"
#include "events.h"

using namespace std;

class MessageCenter
{
protected:
	EventQueue m_queue;

public:
	// Singleton instance
	static MessageCenter& instance();
	static MessageCenter& defaultCenter();
	virtual ~MessageCenter();
	MessageCenter(MessageCenter&&) = delete;						// Move constructor disabled (C++11 feature)
	MessageCenter(const MessageCenter& that) = delete; 			// Copy constructor disabled (C++11 feature)
	MessageCenter& operator =(MessageCenter const&) = delete;		// Assignment disabled (C++11 feature)

public:
	bool init();
	void dispose();

public:
	void addObserver(const string& name, const EventObserver& observer);
	void removeObserver(const EventObserver& observer);
	void removeObserver(const string& name, const EventObserver& observer);
	void removeObservers();

	void post(const string& name, const EventSource& source, void* param);

private:
	MessageCenter(); // Disallow direct instances creation with private constructor
};

#endif /* COMMON_EVENTS_MESSAGECENTER_H_ */
