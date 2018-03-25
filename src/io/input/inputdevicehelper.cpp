#include "inputdevicehelper.h"

#include "../../common/logger/logger.h"

const string InputDeviceHelper::getDeviceModel(int fd)
{
	static const int EVENT_BUFFER_SIZE = 256;

	string result;
	result.resize(EVENT_BUFFER_SIZE);

	int res = ioctl(fd, EVIOCGNAME(EVENT_BUFFER_SIZE), result.c_str());
	if (res >= 0 && res <= EVENT_BUFFER_SIZE)
	{
		result.resize(res);
	}

	return result;
}

InputDeviceTypeEnum InputDeviceHelper::getDeviceType(int fd)
{
	InputDeviceTypeEnum result = InputDeviceTypeEnum::Unknown;

	if (!isDescriptorValid(fd))
		return result;

	// Ioctl input-related bit arrays
	BitType bit_ev[BITFIELD_LONGS_PER_ARRAY(EV_MAX)];
	BitType bit_key[BITFIELD_LONGS_PER_ARRAY(KEY_MAX)];
	BitType bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];

	getDeviceEventBits(fd, bit_ev, sizeof(bit_ev));
	getDeviceKeyBits(fd, bit_key, sizeof(bit_key));

	// Detect Mouse device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// If device supports relative coordinates and supports at least left button - most likely it's a mouse
		if (hasEventType(bit_ev, EV_REL) &&
			hasEventType(bit_ev, EV_KEY) && hasEventCode(bit_key, BTN_LEFT)
			)
		{
			result = InputDeviceTypeEnum::Mouse;
		}
	}

	// Detect Joystick device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// If device supports absolute coordinates and has gamepad button or joystick button - most likely it's a joystick
		if (hasEventType(bit_ev, EV_ABS) &&
			hasEventType(bit_ev, EV_KEY) &&
			(hasEventCode(bit_key, BTN_GAMEPAD) || hasEventCode(bit_key, BTN_JOYSTICK))
			)
		{
			result = InputDeviceTypeEnum::Joystick;
		}
	}

	// Detect Keyboard
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// Keyboard needs to have at least few letter buttons
		// TODO: Numpads detection (keyboard HID but have only numeric keys)
		if (hasEventType(bit_ev, EV_KEY) &&
			hasEventCode(bit_key, KEY_A) &&
			hasEventCode(bit_key, KEY_Z)
			)
		{
			uint16_t ledBits = getDeviceLEDBits(fd, bit_led, sizeof(bit_led));
			if (ledBits > 0)
			{
				result = InputDeviceTypeEnum::Keyboard;
			}
		}
	}

	return result;
}

uint32_t InputDeviceHelper::getDeviceEventBits(int fd, BitType* bit_ev, size_t length)
{
	uint32_t result = 0x00000000;

	if (!isDescriptorValid(fd))
		return result;

	if (ioctl(fd, EVIOCGBIT(0, length), bit_ev) >= 0)
	{
		result = (uint32_t)bit_ev[0];

		//TRACE("Device event bits: 0x%08x | %s", result, DisplayHelper::formatBits(result).c_str());
		//TRACE("Event bits: %s", dumpEventBits(result).c_str());
		//TRACE("");
	}
	else
	{
		LOGERROR("Unable to retrieve event bits for device");
	}

	return result;
}

BitType* InputDeviceHelper::getDeviceKeyBits(int fd, BitType* bit_key, size_t length)
{
	if (!isDescriptorValid(fd))
		return bit_key;

	if (ioctl(fd, EVIOCGBIT(EV_KEY, length), bit_key) >= 0)
	{
	}
	else
	{
		LOGERROR("Unable to retrieve Key bits for device");
	}

	return bit_key;
}

uint16_t InputDeviceHelper::getDeviceLEDBits(int fd, BitType* bit_led, size_t length)
{
	uint16_t result = 0x0000;

	if (!isDescriptorValid(fd))
		return result;

	if (ioctl(fd, EVIOCGBIT(EV_LED, length), bit_led) >= 0)
	{
		result = (uint16_t)bit_led[0];

		//TRACE("Device LED bits: 0x%04x | %s", result, DisplayHelper::formatBits(result).c_str());
		//TRACE("LED bits: %s", dumpLEDBits(result).c_str());
		//TRACE("");
	}
	else
	{
		LOGERROR("Unable to retrieve LED bits for device");
	}

	return result;
}
