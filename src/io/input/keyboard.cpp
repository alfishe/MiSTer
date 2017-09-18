#include "keyboard.h"

#include "../../common/logger/logger.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include "../../3rdparty/openbsd/string.h"
#include "../../common/consts.h"

keyboard::keyboard(int fd)
{
	this->fd = fd;
}

keyboard::~keyboard()
{
}

/*
 * Retrieves device name from the device itself
 * Reference: http://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/064/6429/6429l4.html
 */
const char * keyboard::getDeviceName()
{
	char *result = nullptr;

	char buffer[255];

	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		if (ioctl(fd, EVIOCGNAME(sizeof(buffer)), buffer) != -1)
		{
			strlcpy(deviceName, buffer, sizeof(buffer));
			result = deviceName;
		}
		else
		{
			LOGERROR("%s: unable to get device name\n%s", __PRETTY_FUNCTION__, logger::geterror());
		}
	}
	else
	{
		LOGERROR("%s: Device descriptor is not valid\n", __PRETTY_FUNCTION__);
	}

	return result;
}

/*
 * Checks if keyboard has LEDs
 */
bool keyboard::hasLED()
{
	bool result = false;

	uint8_t evtype_b[(EV_MAX + 7) / 8];

	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		memset(&evtype_b, 0, sizeof(evtype_b));

		if (ioctl(fd, EVIOCGBIT(0, sizeof(evtype_b)), evtype_b) != -1)
		{
			if (isBitSet(EV_LED, evtype_b))
			{
				LOGINFO("has LEDs");
				result = true;
			}
		}
		else
		{
			LOGERROR("%s: unable to get input device capabilities\n%s", __PRETTY_FUNCTION__, logger::geterror());
		}
	}
	else
	{
		LOGERROR("%s: Device descriptor is not valid\n", __PRETTY_FUNCTION__);
	}

	return result;
}

/*
 * Check LEDs state for the keyboard
 * Reference: http://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/064/6429/6429l11.html
 */
void keyboard::getLEDState()
{
	uint8_t led_b[LED_MAX];

	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		memset(led_b, 0, sizeof(led_b));
		if (ioctl(fd, EVIOCGLED(sizeof(led_b)), led_b) != -1)
		{
			for (int i = 0; i < LED_MAX; i++)
			{
				if (isBitSet(i, led_b))
				{
					TRACE("LED 0x%02x", i);

					switch (i)
					{
						case LED_NUML:
							TRACE("NumLock LED");
							break;
						case LED_CAPSL:
							TRACE("CapsLock LED");
							break;
						case LED_SCROLLL:
							TRACE("ScrollLock LED");
							break;
						default:
							TRACE("Unknown LED: 0x%04hx", i);
							break;
					}
				}
			}
		}
		else
		{
			LOGERROR("%s: unable to get keyboard LEDs state\n%s", __PRETTY_FUNCTION__, logger::geterror());
		}
	}
	else
	{
		LOGERROR("%s: Device descriptor is not valid\n", __PRETTY_FUNCTION__);
	}

	// All LEDS available (defined in input-event-codes.h)
/*
	#define LED_NUML		0x00
	#define LED_CAPSL		0x01
	#define LED_SCROLLL		0x02
	#define LED_COMPOSE		0x03
	#define LED_KANA			0x04
	#define LED_SLEEP		0x05
	#define LED_SUSPEND		0x06
	#define LED_MUTE			0x07
	#define LED_MISC			0x08
	#define LED_MAIL			0x09
	#define LED_CHARGING		0x0a
*/
}

void keyboard::setLEDState(uint8_t state)
{
	input_event event;
	event.type = EV_LED;
	event.code = state;

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
