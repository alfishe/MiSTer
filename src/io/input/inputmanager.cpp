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

InputManager::~InputManager()
{
	TRACE("~InputManager()");

	reset();
}

/// Clear all internal information about devices
///
void InputManager::reset()
{
	keyboards.clear();
	mouses.clear();
	joysticks.clear();
	inputDevices.clear();
}

InputDeviceVector& InputManager::detectDevices()
{
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

				BaseInputDevice device(path);
				int fd = device.openDevice();
				string name = device.getDeviceName();
				uint32_t eventBits = device.getDeviceEventBits();
				InputDeviceTypeEnum type = device.getDeviceType();
				device.closeDevice();

				if (isDeviceTypeAllowed(type))
				{
					InputDevice inputDevice;
					inputDevice.path = path;
					inputDevice.name = name;
					inputDevice.eventBits = eventBits;
					inputDevice.type = type;

					// Register device in correspondent collections
					addInputDevice(inputDevice);
				}
				else
				{
					LOGWARN("Unknown device with name '%s' detected on path '%s'", name.c_str(), path.c_str());
				}
			}
		);

		// Info logging
		LOGINFO(dump(this->inputDevices).c_str());
		// -Info logging
	}
	else
	{
		LOGINFO("No input devices availablee\n");
	}

	// Free up ScanDir buffers
	scan.dispose();

	return this->inputDevices;
}

bool InputManager::isDeviceTypeAllowed(InputDeviceTypeEnum type)
{
	bool result = false;

	// Allow Mouse, Joystick, Keyboard types to be detected
	if (type != +InputDeviceTypeEnum::Unknown)
	{
		result = true;
	}

	return result;
}

// ======================= Protected methods =================================
void InputManager::addInputDevice(InputDevice& device)
{
	// Register in device-type specific collection
	switch (device.type)
	{
		case InputDeviceTypeEnum::Mouse:
			mouses.push_back(device);
			break;
		case InputDeviceTypeEnum::Joystick:
			joysticks.push_back(device);
			break;
		case InputDeviceTypeEnum::Keyboard:
			keyboards.push_back(device);
			break;
		default:
			break;
	}

	// Register in global collection
	inputDevices.push_back(device);
}

// ======================= Debug methods =====================================

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
						BaseInputDevice device(inputDevice.path);

						int fd = device.openDevice();
						uint16_t ledBits = device.getDeviceLEDBits();
						device.closeDevice();

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
