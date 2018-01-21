#include "keyboard.h"

#include "../../common/logger/logger.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include "../../3rdparty/openbsd/string.h"
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/consts.h"
#include "../../common/exception/misterexception.h"

Keyboard::Keyboard(const string& name, const string& path) : BaseInputDevice(name, path)
{
}

Keyboard::~Keyboard()
{
	closeDevice();
}

bool Keyboard::isKeyPressed(uint16_t key)
{
	pollKeys();
	bool result = isKeyPressed(bit_key_state, key);

	return result;
}

bool Keyboard::isKeyPressed(unsigned long* keyBits, uint16_t key)
{
	bool result = isBitSet(keyBits, key);

	return result;
}

bool Keyboard::pollKeys()
{
	bool result = false;

	// Clear key states bit array
	memset(bit_key_state, 0, sizeof(bit_key_state));

	if (ioctl(fd, EVIOCGKEY(sizeof(bit_key_state)), &bit_key_state) >= 0)
	{
		result = true;
	}

	return result;
}

int Keyboard::getPressedKeysCount()
{
	int result = bitCount(bit_key_state, sizeof(bit_key_state) / sizeof(bit_key_state[0]));

	/* Dumb implementation
	for (unsigned i = 0; i < KEY_MAX; i++)
	{
		if (isBitSet(bit_key_state, i))
		{
			result++;
		}
	}
	*/

	return result;
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

// Debug methods
string Keyboard::dumpKeyBits()
{
	string result = dumpKeyBits(bit_key_state);

	return result;
}

string Keyboard::dumpKeyBits(unsigned long* keyBits)
{
	string result;
	stringstream ss;

	int setBits = 0;
	for (unsigned i = 0; i < KEY_MAX; i++)
	{
		if (isBitSet(keyBits, i))
		{
			if (setBits != 0)
				ss << ", ";

			if (i < keyNames.size())
			{
				ss << tfm::format("0x%04x %s", i, keyNames[i].c_str());
			}
			else
			{
				ss << tfm::format("0x%04x UNKNOWN_%04x", i, i);
			}

			setBits++;
		}
	}

	result = tfm::format("Keys set: %d\n%s\n", setBits, ss.str().c_str());

	return result;
}
