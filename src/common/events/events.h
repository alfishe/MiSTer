#ifndef COMMON_EVENTS_EVENTS_H_
#define COMMON_EVENTS_EVENTS_H_

#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../types.h"

using namespace std;

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
typedef class EventObserver EventObserver;

typedef EventObserver* EventObserverPtr;
typedef set<EventObserverPtr> EventObserversSet;
typedef map<string, EventObserversSet> EventObserversMap;

typedef map<EventObserverPtr, StringSet> EventObserversReverseMap;



class MessageEvent
{
public:
	const string name;
	const EventSource& source;
	const void* param;

public:
	MessageEvent(const string& name, const EventSource& source, const void* param) : name(name), source(source), param(param) {};
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
    virtual void onMessageEvent(MessageEvent event) = 0;
};
typedef class EventObserver EventObserver;

#endif /* COMMON_EVENTS_EVENTS_H_ */
