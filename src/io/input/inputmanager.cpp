#include "inputmanager.h"

#include "../../common/logger/logger.h"

#include <algorithm>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <sys/sysinfo.h>

#include "input.h"
#include "baseinputdevice.h"
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/consts.h"
#include "../../common/file/path/path.h"
#include "../../common/file/scandir/scandir.h"

InputManager& InputManager::instance()
{
	static InputManager instance;

	return instance;
}

InputDeviceVector InputManager::detectDevices()
{
	InputDeviceVector result;

	// Scan for available input device using path pattern:  /dev/input/event<N>
	ScanDir scan;
	scan.scanFolder(LINUX_DEVICE_INPUT, ScanDir::getInputDevicesFilter(), ScanDir::getAlphaSort());
	auto devices = scan.getScanResults();

	if (devices.size() > 0)
	{
		for_each(devices.begin(), devices.end(),
			[&](DirectoryEntry& entry)
			{
				string path = Path::combine(LINUX_DEVICE_INPUT, entry.name).toString();

				int fd = BaseInputDevice::openDevice(path);
				string name = BaseInputDevice::getDeviceName(fd);
				uint32_t eventBits = BaseInputDevice::getDeviceEventBits(fd);
				InputDeviceTypeEnum type = BaseInputDevice::getDeviceType(fd);
				BaseInputDevice::closeDevice(fd);

				InputDevice inputDevice;
				inputDevice.path = path;
				inputDevice.name = name;
				inputDevice.eventBits = eventBits;
				inputDevice.type = type;

				result.push_back(inputDevice);
			}
		);

		// Info logging
		LOGINFO(dump(result).c_str());
		// -Info logging
	}
	else
	{
		LOGINFO("No input devices availablee\n");
	}

	// Store result in a field
	this->inputDevices = result;

	// Free up ScanDir buffers
	scan.dispose();

	return result;
}

string InputManager::dump(InputDeviceVector& inputDevices)
{
	string result = tfm::format("Available input devices:  %d\n", inputDevices.size());

	for_each(inputDevices.begin(), inputDevices.end(),
		[&](InputDevice& inputDevice)
		{
			result += tfm::format("path: '%s'", inputDevice.path.c_str());
			result += "\n";
			result += tfm::format("name: '%s'", inputDevice.name.c_str());
			result += "\n";
			result += tfm::format("type: 0x%x ('%s')", inputDevice.type._to_integral(), inputDevice.type._to_string());
			result += "\n";

			switch (inputDevice.type)
			{
				case InputDeviceTypeEnum::Mouse:
					break;
				case InputDeviceTypeEnum::Joystick:
					break;
				case InputDeviceTypeEnum::Keyboard:
					{
						int fd = BaseInputDevice::openDevice(inputDevice.path);
						uint16_t ledBits = BaseInputDevice::getDeviceLEDBits(fd);
						BaseInputDevice::closeDevice(fd);

						result += tfm::format("LED bits: 0x%04x | %s", ledBits, BaseInputDevice::dumpLEDBits(ledBits).c_str());
						result += "\n";
					}
					break;
				default:
					break;
			}

			result += "\n";
		}
	);

	return result;
}
