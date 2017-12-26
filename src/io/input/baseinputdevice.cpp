#include "baseinputdevice.h"

#include "../../common/logger/logger.h"

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/helpers/displayhelper.h"
#include "../../common/helpers/stringhelper.h"

#define IS_BIT_SET(var, pos) ((var) & (1 << (pos)))

// Check if correspondent <bit> set in a bitset
#define test_bit(bit, bitset) (bitset [bit / 8] & (1 << (bit % 8)))
#define NBITS(x) ((((x) - 1) / LONG_BIT) + 1)

/*
 * Macros for reading ioctl bit fields.
 */
#define BITFIELD_BITS_PER_LONG      (sizeof(unsigned long) * 8)
#define BITFIELD_LONGS_PER_ARRAY(x) ((((x) - 1) / BITFIELD_BITS_PER_LONG) + 1)
#define BITFIELD_TEST(bit, array)   ((array[((bit) / BITFIELD_BITS_PER_LONG)] >> ((bit) % BITFIELD_BITS_PER_LONG)) & 0x1)

// Initialize static field arrays

vector<string> BaseInputDevice::eventNames =
{
	"EV_SYN",		// 0x00
	"EV_KEY",		// 0x01
	"EV_REL",		// 0x02
	"EV_ABS",		// 0x03
	"EV_MSC",		// 0x04
	"EV_SW",			// 0x05
	"UNKNOWN 0x06",	// 0x06
	"UNKNOWN 0x07",	// 0x07
	"UNKNOWN 0x08",	// 0x08
	"UNKNOWN 0x09",	// 0x09
	"UNKNOWN 0x0A",	// 0x0A
	"UNKNOWN 0x0B",	// 0x0B
	"UNKNOWN 0x0C",	// 0x0C
	"UNKNOWN 0x0D",	// 0x0D
	"UNKNOWN 0x0E",	// 0x0E
	"UNKNOWN 0x0F",	// 0x0F
	"UNKNOWN 0x10",	// 0x10
	"EV_LED",		// 0x11
	"EV_SND",		// 0x12
	"UNKNOWN 0x13",	// 0x13
	"EV_REP",		// 0x14
	"EV_FF",			// 0x15
	"EV_PWR",		// 0x16
	"EV_FF_STATUS",	// 0x17
	"UNKNOWN 0x18",	// 0x18
	"UNKNOWN 0x19",	// 0x19
	"UNKNOWN 0x1A",	// 0x1A
	"UNKNOWN 0x1B",	// 0x1B
	"UNKNOWN 0x1C",	// 0x1C
	"UNKNOWN 0x1D",	// 0x1D
	"UNKNOWN 0x1E",	// 0x1E
	"UNKNOWN 0x1F"	// 0x1F
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

int BaseInputDevice::openDevice(const string& path)
{
	int result = open(path.c_str(), O_RDONLY);

	return result;
}

void BaseInputDevice::closeDevice(int fd)
{
	close(fd);
}

string BaseInputDevice::getDeviceName(const string& path)
{
	string result;

	int fd = open(path.c_str(), O_RDONLY);
	if (fd >= 0)
	{
		result = getDeviceName(fd);

		close(fd);
	}

	return result;
}

string BaseInputDevice::getDeviceName(int fd)
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

InputDeviceTypeEnum BaseInputDevice::getDeviceType(const string& path)
{
	InputDeviceTypeEnum result = InputDeviceTypeEnum::Unknown;

	int fd = open(path.c_str(), O_RDONLY);
	if (fd >= 0)
	{
		result = getDeviceType(fd);

		close(fd);
	}

	return result;
}

InputDeviceTypeEnum BaseInputDevice::getDeviceType(int fd)
{
	InputDeviceTypeEnum result = InputDeviceTypeEnum::Unknown;

	uint32_t bits = getDeviceEventBits(fd);

	// Detect Mouse device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		if (IS_BIT_SET(bits, EV_REL))
		{
			result = InputDeviceTypeEnum::Mouse;
		}
	}

	// Detect Joystick device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		if (IS_BIT_SET(bits, EV_ABS))
		{
			result = InputDeviceTypeEnum::Joystick;
		}
	}

	// Detect Keyboard
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		if (IS_BIT_SET(bits, EV_KEY))
		{
			uint16_t ledBits = getDeviceLEDBits(fd);
			if (ledBits > 0)
			{
				result = InputDeviceTypeEnum::Keyboard;
			}
		}
	}

	return result;
}

uint32_t BaseInputDevice::getDeviceEventBits(int fd)
{
	uint32_t result = 0x00000000;
	unsigned long bit_ev[BITFIELD_LONGS_PER_ARRAY(EV_MAX)];

	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), &bit_ev) >= 0)
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

uint16_t BaseInputDevice::getDeviceLEDBits(int fd)
{
	uint16_t result = 0x0000;
	unsigned long bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];

	if (ioctl(fd, EVIOCGBIT(EV_LED, LED_MAX), &bit_led) >= 0)
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

// ======================= Debug methods ============================

string BaseInputDevice::dumpEventBits(uint32_t value)
{
	static const char* bitNames[] =
	{
		"EV_SYN",		// 0x00
		"EV_KEY",		// 0x01
		"EV_REL",		// 0x02
		"EV_ABS",		// 0x03
		"EV_MSC",		// 0x04
		"EV_SW",			// 0x05
		"UNKNOWN 0x06",	// 0x06
		"UNKNOWN 0x07",	// 0x07
		"UNKNOWN 0x08",	// 0x08
		"UNKNOWN 0x09",	// 0x09
		"UNKNOWN 0x0A",	// 0x0A
		"UNKNOWN 0x0B",	// 0x0B
		"UNKNOWN 0x0C",	// 0x0C
		"UNKNOWN 0x0D",	// 0x0D
		"UNKNOWN 0x0E",	// 0x0E
		"UNKNOWN 0x0F",	// 0x0F
		"UNKNOWN 0x10",	// 0x10
		"EV_LED",		// 0x11
		"EV_SND",		// 0x12
		"UNKNOWN 0x13",	// 0x13
		"EV_REP",		// 0x14
		"EV_FF",			// 0x15
		"EV_PWR",		// 0x16
		"EV_FF_STATUS",	// 0x17
		"UNKNOWN 0x18",	// 0x18
		"UNKNOWN 0x19",	// 0x19
		"UNKNOWN 0x1A",	// 0x1A
		"UNKNOWN 0x1B",	// 0x1B
		"UNKNOWN 0x1C",	// 0x1C
		"UNKNOWN 0x1D",	// 0x1D
		"UNKNOWN 0x1E",	// 0x1E
		"UNKNOWN 0x1F"	// 0x1F
	};

	string result;

	uint8_t idx = 0x1F;
	for (uint32_t i = 1 << 0x1F; i > 0; i >>= 1)
	{
		if ((value & i) && idx < sizeof(bitNames) / sizeof(char*))
		{
			if (result.size() > 0)
				result += ", ";

			result += bitNames[idx];
		}

		idx--;
	}

	if (result.size() == 0)
	{
		result = "None";
	}

	return result;
}

string BaseInputDevice::dumpLEDBits(uint16_t value)
{
	static const char* bitNames[] =
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

	string result;

	uint8_t idx = 0x0F;
	for (uint32_t i = 1 << 0x0F; i > 0; i >>= 1)
	{
		if ((value & i) && idx < sizeof(bitNames) / sizeof(char*))
		{
			if (result.size() > 0)
				result += ", ";

			result += bitNames[idx];
		}

		idx--;
	}

	if (result.size() == 0)
	{
		result = "None";
	}

	return result;
}
