#include "keyboard.h"

#include "../../common/logger/logger.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include "../../3rdparty/openbsd/string.h"
#include "../../common/consts.h"
#include "../../common/exception/misterexception.h"

Keyboard::Keyboard(const string& path) : BaseInputDevice(path)
{
}

Keyboard::~Keyboard()
{
	closeDevice();
}

uint16_t Keyboard::getLEDState()
{
	uint16_t result = 0x0000;

	throw MiSTerException("Not implemented");

	return result;
}

void Keyboard::setLEDState(uint16_t ledMask, bool on)
{
	struct input_event event;
	makeLEDEvent(&event, ledMask, on);

	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		if (write(fd, &event, sizeof(event)) != -1)
		{
			// Success
		}
		else
		{
			LOGERROR("%s: unable to set keyboard LEDs state\n%s", __PRETTY_FUNCTION__, logger::geterror());
		}
	}
	else
	{
		LOGERROR("%s: Device descriptor is not valid\n", __PRETTY_FUNCTION__);
	}
}

void Keyboard::makeLEDEvent(struct input_event* event, uint16_t ledMask, bool on)
{
	if (event == nullptr)
		return;

	event->type = EV_LED;
	event->code = ledMask;

	if (on)
	{
		event->value = 1;
	}
	else
	{
		event->value = 0;
	}
}
