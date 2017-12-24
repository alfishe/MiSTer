#include "baseinputdevice.h"

#include "../../common/logger/logger.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/helpers/stringhelper.h"


// Check if correspondent <bit> set in a bitset
#define test_bit(bit, bitset) (bitset [bit / 8] & (1 << (bit % 8)))

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)

// Initialize static field arrays

vector<string> BaseInputDevice::eventNames =
{
	"EV_SYN",
	"EV_KEY",
	"EV_REL",
	"EV_ABS",
	"EV_MSC",
	"EV_SW",
	"EV_LED",
	"EV_SND",
	"EV_REP",
	"EV_FF",
	"EV_PWR",
	"EV_FF_STATUS"
};

vector<string> BaseInputDevice::ledNames =
{
	"LED_NUML",			// 0x00
	"LED_CAPSL",			// 0x01
	"LED_SCROLLL",		// 0x02
	"LED_COMPOSE",		// 0x03
	"LED_KANA",			// 0x04
	"LED_SLEEP",			// 0x05
	"LED_SUSPEND",		// 0x06
	"LED_MUTE",			// 0x07
	"LED_MISC",			// 0x08
	"LED_MAIL",			// 0x09
	"LED_CHARGING",		// 0x0A
	"UNDEFINED 0x0B",	// 0x0B
	"UNDEFINED 0x0C",	// 0x0C
	"UNDEFINED 0x0D",	// 0x0D
	"UNDEFINED 0x0E",	// 0x0E
	"UNDEFINED 0x0F"		// 0x0F
};

// -Initialize static field arrays

string BaseInputDevice::getDeviceName(int fd)
{
	string result;

	return result;
}

uint32_t BaseInputDevice::getDeviceEventBits(int fd)
{
	uint32_t result = 0x00000000;

	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), &result) >= 0)
	{

	}
	else
	{
		LOGERROR("Unable to retrieve event bits for device");
	}

	return result;
}

uint16_t BaseInputDevice::getDeviceLEDs(int fd)
{
	uint16_t result = 0x0000;

	// Reserve buffer for packed bit array storing LED flags
	uint8_t led_b[(LED_MAX + 7) / 8];
	memset(&led_b, 0, sizeof(led_b));

	if (ioctl(fd, EVIOCGLED(sizeof(led_b)), led_b) >= 0)
	{

	}
	else
	{

	}

	return result;
}

string BaseInputDevice::printDeviceInfo(int fd)
{
	string result;

	uint8_t type;
	uint16_t code;
	uint32_t version;
	uint16_t id[4];
	char name[256] = "Unknown";
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	unsigned long state[KEY_CNT] = {0};

	// Retrieve device version information
	if (ioctl(fd, EVIOCGVERSION, &version))
	{
		LOGERROR("printDeviceInfo: can't get version");
		return result;
	}
	string strVersion = tfm::format("Input driver version is %d.%d.%d\n",
									version >> 16,
									(version >> 8) & 0xff,
									version & 0xff);

	// Retrieve IDs (Bus, Vendor - VID, Product - PID, Version)
	ioctl(fd, EVIOCGID, id);
	string strIDs = tfm::format("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
								id[ID_BUS],
								id[ID_VENDOR],
								id[ID_PRODUCT],
								id[ID_VERSION]);

	// Retrieve device name
	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	string strName = tfm::format("Input device name: \"%s\"\n", name);

	// Retrieve information about supported events
	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	string strEvents = "Supported events:\n";

	int stateval;
	bool have_state;
	for (type = 0; type < EV_MAX; type++)
	{
		if (test_bit(type, bit[0]) && type != EV_REP)
		{
			strEvents.append(tfm::format("  Event type 0x%X (uint16)\n", type));

			have_state = getDeviceState(fd, type, state, sizeof(state));
			if (type != EV_SYN)
			{
				ioctl(fd, EVIOCGBIT(type, KEY_MAX), bit[type]);

				for (code = 0; code < KEY_MAX; code++)
				{
					if (test_bit(code, bit[type]))
					{
						if (have_state)
						{
							stateval = test_bit(code, state);
							//strEvents.append("    Event code %d (%s) state %d\n", code, codename(type, code), stateval);
						}
						else
						{
							//strEvents.append("    Event code %d (%s)\n", code, codename(type, code));
						}

						//if (type == EV_ABS)
						//	print_absdata(fd, code);
					}
				}
			}
		}
	}

	return result;
}

bool BaseInputDevice::getDeviceState(int fd, uint8_t type, unsigned long *array, size_t size)
{
	bool result = false;

	int rc = 0;
	switch (type)
	{
		case EV_LED:
			rc = ioctl(fd, EVIOCGLED(size), array);
			break;
		case EV_SND:
			rc = ioctl(fd, EVIOCGSND(size), array);
			break;
		case EV_SW:
			rc = ioctl(fd, EVIOCGSW(size), array);
			break;
		case EV_KEY:
			/* intentionally not printing the value for EV_KEY, let the
			 * repeat handle this */
			break;
	}

	if (rc >= 0)
	{
		result = true;
	}

	return result;
}

// ======================= Device information methods ============================

string BaseInputDevice::getInputDeviceName(int deviceIndex)
{
	// Retrieve device name using access to device:
	// '/sys/class/input/event%d/device/name'
	string query = getDeviceNameQuery();
	string nameDeviceName = tfm::format(query, deviceIndex);
	string result = readDeviceStringValue(nameDeviceName);

	// Cleaning up from non-printable characters
	result = StringHelper::cleanup(result);

	return result;
}

VIDPID BaseInputDevice::getInputDeviceVIDPID(int deviceIndex)
{
	VIDPID result;

	// Retrieve input device VID and PID using access to devices:
	// '/sys/class/input/event%d/device/id/vendor'
	// '/sys/class/input/event%d/device/id/product'
	string vidDeviceName = tfm::format(LINUX_INPUT_DEVICE_VID, deviceIndex);
	string pidDeviceName = tfm::format(LINUX_INPUT_DEVICE_PID, deviceIndex);

	result.vid = readDeviceHexValue(vidDeviceName);
	result.pid = readDeviceHexValue(pidDeviceName);

	return result;
}

string BaseInputDevice::readDeviceStringValue(const string& deviceName)
{
	string result;

	ifstream inputFile;

	inputFile.open(deviceName.c_str());
	if (inputFile.is_open())
	{
		// Read the whole line (including spaces/tabs/etc. symbols into string variable)
		// If only first word is needed then 'inputFile >> result;' will serve well
		getline(inputFile, result);

		inputFile.close();
	}

	return result;
}

uint16_t BaseInputDevice::readDeviceHexValue(const string& deviceName)
{
	uint16_t result = 0x0000;

	ifstream inputFile;

	inputFile.open(deviceName.c_str());
	if (inputFile.is_open())
	{
		inputFile >> result;

		inputFile.close();
	}

	return result;
}

// ------------------- Protected methods ---------------------------------------

// Returns formatter for the input device name
// '/sys/class/input/event%d/device/name'
const string BaseInputDevice::getDeviceNameQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE, "name");

	return result;
}

const string BaseInputDevice::getDeviceBusTypeQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "bustype");

	return result;
}

const string BaseInputDevice::getDeviceVendorQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "vendor");

	return result;
}

const string BaseInputDevice::getDeviceProductQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "product");

	return result;
}
