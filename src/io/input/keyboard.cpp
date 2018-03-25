#include "keyboard.h"

#include "../../common/logger/logger.h"

#include <cstring>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include "../../3rdparty/openbsd/string.h"
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/consts.h"
#include "../../common/exception/misterexception.h"
#include "../../common/helpers/collectionhelper.h"

Keyboard::Keyboard(const string& name, const string& path) : BaseInputDevice(name, path)
{
}

Keyboard::~Keyboard()
{
	closeDevice();
}

void Keyboard::reset()
{
	// Reset all internal buffers and states
	m_keysState.clear();
	memset(bit_key, 0, sizeof(bit_key));
}

bool Keyboard::isKeyPressed(uint16_t key)
{
	pollKeys();
	bool result = isKeyPressed(bit_key_state, key);

	return result;
}

bool Keyboard::isKeyPressed(unsigned long* keyBits, uint16_t key)
{
	bool result = InputDeviceHelper::isBitSet(keyBits, key);

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
	int result = InputDeviceHelper::bitCount(bit_key_state, sizeof(bit_key_state) / sizeof(bit_key_state[0]));

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

void Keyboard::setKeyState(uint16_t key, bool state)
{
	if (key_exists(m_keysState, key))
	{
		m_keysState[key] = state;
	}
	else
	{
		m_keysState.insert( {key, state} );
	}
}

bool Keyboard::getKeyState(uint16_t key)
{
	bool result = false;

	if (key_exists(m_keysState, key))
	{
		result = m_keysState[key];
	}

	return result;
}

void Keyboard::setPrevKeyState(uint16_t key, bool state)
{
	if (key_exists(m_prevKeysState, key))
	{
		m_prevKeysState[key] = state;
	}
	else
	{
		m_prevKeysState.insert( {key, state} );
	}
}

bool Keyboard::getPrevKeyState(uint16_t key)
{
	bool result = false;

	if (key_exists(m_prevKeysState, key))
	{
		result = m_prevKeysState[key];
	}

	return result;
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
		if (InputDeviceHelper::isBitSet(keyBits, i))
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
