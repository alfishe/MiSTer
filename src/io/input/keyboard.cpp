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

Keyboard::Keyboard(const string& path) : BaseInputDevice(path)
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
	static const char* keyNames[] =
	{
		"KEY_RESERVED",			// 0x00
		"KEY_ESC",				// 0x01
		"KEY_1",					// 0x02
		"KEY_2",					// 0x03
		"KEY_3",					// 0x04
		"KEY_4",					// 0x05
		"KEY_5",					// 0x06
		"KEY_6",					// 0x07
		"KEY_7",					// 0x08
		"KEY_8",					// 0x09
		"KEY_9",					// 0x0A
		"KEY_0",					// 0x0B
		"KEY_MINUS",				// 0x0C
		"KEY_EQUAL",				// 0x0D
		"KEY_BACKSPACE",			// 0x0E
		"KEY_TAB",				// 0x0F
		"KEY_Q",					// 0x10
		"KEY_W",					// 0x11
		"KEY_E",					// 0x12
		"KEY_R",					// 0x13
		"KEY_T",					// 0x14
		"KEY_Y",					// 0x15
		"KEY_U",					// 0x16
		"KEY_I",					// 0x17
		"KEY_O",					// 0x18
		"KEY_P",					// 0x19
		"KEY_LEFTBRACE",			// 0x1A
		"KEY_RIGHTBRACE",		// 0x1B
		"KEY_ENTER",				// 0x1C
		"KEY_LEFTCTRL",			// 0x1D
		"KEY_A",					// 0x1E
		"KEY_S",					// 0x1F
		"KEY_D",					// 0x20
		"KEY_F",					// 0x21
		"KEY_G",					// 0x22
		"KEY_H",					// 0x23
		"KEY_J",					// 0x24
		"KEY_K",					// 0x25
		"KEY_L",					// 0x26
		"KEY_SEMICOLON",			// 0x27
		"KEY_APOSTROPHE",		// 0x28
		"KEY_GRAVE",				// 0x29
		"KEY_LEFTSHIFT",			// 0x2A
		"KEY_BACKSLASH",			// 0x2B
		"KEY_Z",					// 0x2C
		"KEY_X",					// 0x2D
		"KEY_C",					// 0x2E
		"KEY_V",					// 0x2F
		"KEY_B",					// 0x30
		"KEY_N",					// 0x31
		"KEY_M",					// 0x32
		"KEY_COMMA",				// 0x33
		"KEY_DOT",				// 0x34
		"KEY_SLASH",				// 0x35
		"KEY_RIGHTSHIFT",		// 0x36
		"KEY_KPASTERISK",		// 0x37
		"KEY_LEFTALT",			// 0x38
		"KEY_SPACE",				// 0x39
		"KEY_CAPSLOCK",			// 0x3A
		"KEY_F1",				// 0x3B
		"KEY_F2",				// 0x3C
		"KEY_F3",				// 0x3D
		"KEY_F4",				// 0x3E
		"KEY_F5",				// 0x3F
		"KEY_F6",				// 0x40
		"KEY_F7",				// 0x41
		"KEY_F8",				// 0x42
		"KEY_F9",				// 0x43
		"KEY_F10",				// 0x44
		"KEY_NUMLOCK",			// 0x45
		"KEY_SCROLLLOCK",		// 0x46
		"KEY_KP7",				// 0x47
		"KEY_KP8",				// 0x48
		"KEY_KP9",				// 0x49
		"KEY_KPMINUS",			// 0x4A
		"KEY_KP4",				// 0x4B
		"KEY_KP5",				// 0x4C
		"KEY_KP6",				// 0x4D
		"KEY_KPPLUS",			// 0x4E
		"KEY_KP1",				// 0x4F
		"KEY_KP2",				// 0x50
		"KEY_KP3",				// 0x51
		"KEY_KP0",				// 0x52
		"KEY_KPDOT",				// 0x53
		"UNKNOWN_0x54",			// 0x54
		"KEY_ZENKAKUHANKAKU",	// 0x55
		"KEY_102ND",				// 0x56
		"KEY_F11",				// 0x57
		"KEY_F12",				// 0x58
		"KEY_RO",				// 0x59
		"KEY_KATAKANA",			// 0x5A
		"KEY_HIRAGANA",			// 0x5B
		"KEY_HENKAN",			// 0x5C
		"KEY_KATAKANAHIRAGANA", 	// 0x5D
		"KEY_MUHENKAN",			// 0x5E
		"KEY_KPJPCOMMA",			// 0x5F
		"KEY_KPENTER",			// 0x60
		"KEY_RIGHTCTRL",			// 0x61
		"KEY_KPSLASH",			// 0x62
		"KEY_SYSRQ",				// 0x63
		"KEY_RIGHTALT",			// 0x64
		"KEY_LINEFEED",			// 0x65
		"KEY_HOME",				// 0x66
		"KEY_UP",				// 0x67
		"KEY_PAGEUP",			// 0x68
		"KEY_LEFT",				// 0x69
		"KEY_RIGHT",				// 0x6A
		"KEY_END",				// 0x6B
		"KEY_DOWN",				// 0x6C
		"KEY_PAGEDOWN",			// 0x6D
		"KEY_INSERT",			// 0x6E
		"KEY_DELETE",			// 0x6F
		"KEY_MACRO",				// 0x70
		"KEY_MUTE",				// 0x71
		"KEY_VOLUMEDOWN",		// 0x72
		"KEY_VOLUMEUP",			// 0x73
		"KEY_POWER",				// 0x74
		"KEY_KPEQUAL",			// 0x75
		"KEY_KPPLUSMINUS",		// 0x76
		"KEY_PAUSE",				// 0x77
		"KEY_SCALE",				// 0x78
		"KEY_KPCOMMA",			// 0x79
		"KEY_HANGEUL",			// 0x7A
		"KEY_HANGUEL",			// 0x7B
		"KEY_HANJA",				// 0x7C
		"KEY_YEN",				// 0x7D
		"KEY_LEFTMETA",			// 0x7E
		"KEY_RIGHTMETA",			// 0x7F
	};

	string result;
	stringstream ss;

	int setBits = 0;
	for (unsigned i = 0; i < KEY_MAX; i++)
	{
		if (isBitSet(keyBits, i))
		{
			if (setBits != 0)
				ss << ", ";

			if (i < sizeof(keyNames) / sizeof(keyNames[0]))
			{
				ss << tfm::format("0x%04x %s", i, keyNames[i]);
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
