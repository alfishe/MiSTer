#ifndef COMMON_MESSAGETYPES_H_
#define COMMON_MESSAGETYPES_H_

#include "logger/logger.h"

#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <list>
#include <set>
#include <string>
#include <vector>
#include "consts.h"
#include "events/events.h"

using namespace std;

struct DeviceStatusEvent: public MessagePayloadBase
{
	string device;

	DeviceStatusEvent(string device) : device(device) { /*TRACE("DeviceStatusEvent(<param>)");*/ };
	virtual ~DeviceStatusEvent() { /*TRACE("~DeviceStatusEvent()");*/ };
};

struct MouseEvent: public MessagePayloadBase
{
	int x;
	int y;

	MouseEvent(int x, int y) : x(x), y(y) {};
};

struct KeyboardEvent: public MessagePayloadBase
{
	uint16_t key;
	bool state;

	KeyboardEvent(uint16_t key, bool state): key(key), state(state) {};
	virtual ~KeyboardEvent() {};
};

#endif /* COMMON_MESSAGETYPES_H_ */
