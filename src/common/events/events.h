#ifndef COMMON_EVENTS_EVENTS_H_
#define COMMON_EVENTS_EVENTS_H_

#include <functional>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../types.h"

using namespace std;

typedef struct EventSource EventSource;
typedef EventSource* EventSourcePtr;

typedef class EventObserver EventObserver;
typedef EventObserver* EventObserverPtr;
typedef set<EventObserverPtr> EventObserversSet;
typedef map<string, EventObserversSet> EventObserversMap;

typedef map<EventObserverPtr, StringSet> EventObserversReverseMap;

typedef class MessageEvent MessageEvent;
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

struct MessageEvent
{
public:
	string name;
	EventSourcePtr source;
	void* param;

public:
	MessageEvent() {};
	MessageEvent(const string& name, const EventSourcePtr source, const void* param) : name(name), source(source), param((void*)param) {};
	/*
	MessageEvent(const MessageEvent& that)
	{
		name = that.name;
		source = that.source;
		param = that.param;
	}
	*/
	/*
	MessageEvent& operator =(MessageEvent const& that)
	{
		name = that.name;
		source = that.source;
		param = that.param;

		return *this;
	}*/
};
typedef class MessageEvent MessageEvent;

class EventObserver
{
public:
	virtual ~EventObserver() {};

    function<void (MessageEvent)> getNotifyFunc()
    {
        auto messageObserver = [=](MessageEvent event) -> void
        	{
            this->onMessageEvent(event);
        };

        return messageObserver;
    }

protected:
    friend class EventQueue; // Allow only EventQueue derived classes to trigger notification events
    virtual void onMessageEvent(const MessageEvent event) = 0;
};

#endif /* COMMON_EVENTS_EVENTS_H_ */
