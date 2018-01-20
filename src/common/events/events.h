#ifndef COMMON_EVENTS_EVENTS_H_
#define COMMON_EVENTS_EVENTS_H_

#include "../logger/logger.h"

#include <functional>
#include <deque>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "../types.h"

using namespace std;

// Marker for event publisher
typedef struct EventSource EventSource;
typedef EventSource* EventSourcePtr;

// Event queue
typedef class EventMessageBase MessageEvent;
typedef deque<MessageEvent> MessageEventsQueue;

// Base class for all event-producers
class EventSource
{
public:
	const string getClassName() const
	{
		string result;

		const string prettyFunction(__PRETTY_FUNCTION__);
		size_t colons = prettyFunction.find("::");
	    if (colons != string::npos)
	    {
			size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;
			size_t end = colons - begin;

			result = prettyFunction.substr(begin, end);
	    }
	    else
	    {
	        result = "::";
	    }

	    return result;
	}
};

// Base class for every userData payload
struct MessagePayloadBase
{
	virtual ~MessagePayloadBase() {};
};

// Base class for every message in a system delivered via EventQueue
struct EventMessageBase
{
public:
	string topic;
	EventSourcePtr source;
	MessagePayloadBase* payload;

public:
	EventMessageBase()
	{
		//TRACE("EventMessageBase()");
	};

	EventMessageBase(const string& topic, const EventSourcePtr source, MessagePayloadBase* payload)
	{
		//TRACE("EventMessageBase('%s')", topic.c_str());

		this->topic = topic;
		this->source = source;
		this->payload = payload;
	};

	EventMessageBase(const EventMessageBase& that)
	{
		//TRACE("=EventMessageBase('%s')", that.topic.c_str());

		topic = that.topic;
		source = that.source;
		payload = that.payload;
	}

	virtual ~EventMessageBase()
	{
		//TRACE("~EventMessageBase()");
	}
};
typedef struct EventMessageBase EventMessageBase;

class EventObserver
{
public:
	virtual ~EventObserver() {};

    function<void (EventMessageBase&)> getNotifyFunc()
    {
        auto messageObserver = [=](EventMessageBase& event) -> void
        	{
            this->onMessageEvent(event);
        };

        return messageObserver;
    }

protected:
    friend class EventQueue; // Allow only EventQueue derived classes to trigger notification events
    virtual void onMessageEvent(const EventMessageBase& event) = 0;
};

// Observer / class method delegate
typedef class EventObserver EventObserver;
typedef EventObserver* EventObserverPtr;
typedef set<EventObserverPtr> EventObserversSet;
typedef map<string, EventObserversSet> EventObserversMap;
typedef map<EventObserverPtr, StringSet> EventObserversReverseMap;

// Observer / lambda delegate
typedef function<void(const EventObserver*, const EventMessageBase& event)> EventHandler;
typedef tuple<EventObserver*, EventHandler> EventDelegateTuple;
typedef vector<EventDelegateTuple> EventFunctors;
typedef map<string, EventFunctors> EventObserversMapF;

#endif /* COMMON_EVENTS_EVENTS_H_ */
