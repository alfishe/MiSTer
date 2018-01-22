#include "inputmanager.h"

#include "../../common/logger/logger.h"

#include <algorithm>
#include <ostream>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <sys/sysinfo.h>

#include "input.h"
#include "baseinputdevice.h"
#include "mouse.h"
#include "joystick.h"
#include "keyboard.h"
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/consts.h"
#include "../../common/messagetypes.h"
#include "../../common/file/path/path.h"
#include "../../common/file/scandir/scandir.h"
#include "../../common/file/filemanager.h"
#include "../../common/helpers/collectionhelper.h"


InputManager& InputManager::instance()
{
	static InputManager instance;

	return instance;
}

InputManager::InputManager()
{
	TRACE("InputManager()");

	// Create input poller instance
	InputPoller& poller = InputPoller::instance();
	poller.init();

	// Subscribe for device status events
	MessageCenter& center = MessageCenter::defaultCenter();
	center.addObserver(EVENT_DEVICE_INSERTED, this);
	center.addObserver(EVENT_DEVICE_REMOVED, this);
}

InputManager::~InputManager()
{
	TRACE("~InputManager()");

	// Unsubscribe from all notifications
	MessageCenter& center = MessageCenter::defaultCenter();
	center.removeObserver(this);

	// Reset information about all registered input devices
	reset();

	// Ensure that poller is stopped
	InputPoller::instance().dispose();
}

BaseInputDevice* InputManager::findInputDeviceByName(const string& name)
{
	BaseInputDevice* result = nullptr;

	if (key_exists(m_inputDevices, name))
	{
		result = m_inputDevices[name];
	}

	return result;
}

/// Clear all internal information about devices
///
void InputManager::reset()
{
	TRACE("%s", __PRETTY_FUNCTION__);

	// Detach poller from events from all devices listening
	InputPoller::instance().reset();

	// Prevent access from multiple threads
	lock_guard<mutex> lock(m_mutexDevices);

	// Clear all collections
	m_keyboards.clear();
	m_mouses.clear();
	m_joysticks.clear();

	// Free up allocated memory for each device
	for (auto it = m_inputDevices.begin(); it != m_inputDevices.end(); it++)
	{
		BaseInputDevice* device = it->second;
		delete device;
	}

	m_inputDevices.clear();
}

BaseInputDeviceMap& InputManager::detectDevices()
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
				BaseInputDevice* inputDevice = resolveDevice(entry.name);
				if (inputDevice != nullptr)
				{
					// Register device in correspondent collections
					addInputDevice(inputDevice);
				}
			}
		);

		// Info logging
		LOGINFO(dumpDevicesMap().c_str());
		// -Info logging
	}
	else
	{
		LOGINFO("No input devices availablee\n");
	}

	// Free up ScanDir buffers
	scan.dispose();

	return m_inputDevices;
}

void InputManager::startPolling()
{
	InputPoller& poller = InputPoller::instance();

	// Add all already discovered devices to polling list
	for (auto it = m_inputDevices.begin(); it != m_inputDevices.end(); it++)
	{
		poller.addInputDevice(*(it->second));
	}

	poller.start();
}

void InputManager::stopPolling()
{
	InputPoller::instance().stop();
}

BaseInputDevice* InputManager::resolveDevice(const string& name)
{
	BaseInputDevice* result = nullptr;

	string path = Path::combine(LINUX_DEVICE_INPUT, name).toString();

	if (filemanager::isFileExist(path.c_str()))
	{
		// Query device for properties
		result = new BaseInputDevice(name, path);
		result->init();

		if (!isDeviceTypeAllowed(result->type))
		{
			LOGWARN("Unknown device with name '%s' detected on path '%s'", result->model.c_str(), result->path.c_str());

			delete result;
			result = nullptr;
		}
	}
	else
	{
		LOGWARN("%s: input device with path '%s' does not exist", __PRETTY_FUNCTION__, path.c_str());
	}

	return result;
}

// Helper methods
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

// Runnable delegate
void InputManager::onMessageEvent(const EventMessageBase& event)
{
	string eventName = event.topic;

	if (event.payload == nullptr)
	{
	  LOGWARN("%s: notification with name '%s' contains no expected parameter value", __PRETTY_FUNCTION__, eventName.c_str());
	  return;
	}

	// Extract device name from the payload
	DeviceStatusEvent* payload = (DeviceStatusEvent*)event.payload;
	string name = payload->device;

	TRACE("%s: notification name: '%s' with value '%s'", __PRETTY_FUNCTION__, eventName.c_str(), name.c_str());

	if (eventName == EVENT_DEVICE_INSERTED)
	{
		BaseInputDevice* inputDevice = resolveDevice(name);
		if (inputDevice != nullptr)
		{
			LOGINFO("New input device detected:\n%s", dump(inputDevice).c_str());

			// Register device in correspondent collections
			addInputDevice(inputDevice);

			// Notify poller about changes
			InputPoller::instance().addInputDevice(*inputDevice);
		}
	}
	else if (eventName == EVENT_DEVICE_REMOVED)
	{
		if (key_exists(m_inputDevices, name))
		{
			BaseInputDevice* inputDevice = m_inputDevices[name];

			// Notify poller about changes
			InputPoller::instance().removeInputDevice(*inputDevice);
		}

		// Unregister from all collections
		removeInputDevice(name);
	}
}

// ======================= Protected methods =================================
void InputManager::addInputDevice(BaseInputDevice* inputDevice)
{
	BaseInputDevice& device = *inputDevice;

	string name = device.name;
	string deviceModel = device.model;
	int fd = device.fd;

	TRACE("%s: adding input device '%s' with name '%s'...", __PRETTY_FUNCTION__, device.model.c_str(), device.name.c_str());

	// Protect from simultaneous multithread access
	lock_guard<mutex> lock(m_mutexDevices);

	// Register in device-type specific collection
	switch (device.type)
	{
		case InputDeviceTypeEnum::Mouse:
			m_mouses.insert({name, inputDevice});
			break;
		case InputDeviceTypeEnum::Joystick:
			m_joysticks.insert({name, inputDevice});
			break;
		case InputDeviceTypeEnum::Keyboard:
			m_keyboards.insert({name, inputDevice});
			break;
		default:
			break;
	}

	// Register in global collection
	m_inputDevices.insert( {name, inputDevice} );

	TRACE("OK, added");
}

void InputManager::removeInputDevice(const string& name)
{
	TRACE("%s: removing input device with name '%s'...", __PRETTY_FUNCTION__, name.c_str());

	if (key_exists(m_inputDevices, name))
	{
		// Protect from simultaneous multithread access
		lock_guard<mutex> lock(m_mutexDevices);

		BaseInputDevice* inputDevice = m_inputDevices[name];

		switch (inputDevice->type)
		{
			case InputDeviceTypeEnum::Mouse:
				m_mouses.erase(inputDevice->name);
				break;
			case InputDeviceTypeEnum::Joystick:
				m_joysticks.erase(inputDevice->name);
				break;
			case InputDeviceTypeEnum::Keyboard:
				m_keyboards.erase(inputDevice->name);
				break;
			default:
				break;
		}

		m_inputDevices.erase(name);

		// Free up memory
		delete inputDevice;

		TRACE("OK, removed");
	}
	else
	{
		LOGWARN("%s: input device with name '%s' not registered. Unable to remove.", __PRETTY_FUNCTION__, name.c_str());
	}
}

// ======================= Debug methods =====================================
string InputManager::dumpDevicesMap()
{
	stringstream ss;

	ss << tfm::format("Available input devices:  %d\n", m_inputDevices.size());

	for_each(m_inputDevices.begin(), m_inputDevices.end(),
			[&](BaseInputDevicePair pair)
		{
			ss << dump(pair.second);
			ss << '\n';
		}
	);

	/*
	for(auto it = m_inputDevices.begin(); it != m_inputDevices.end(); it++)
	{
		ss << dump(it->second);
		ss << '\n';
	}
	*/

	ss << "\n";

	return ss.str();
}

string InputManager::dump(BaseInputDevice* inputDevice)
{
	BaseInputDevice& device = *inputDevice;

	stringstream ss;

	ss << tfm::format("  name: '%s'", device.name.c_str());
	ss << "\n";
	ss << tfm::format("  path: '%s'", device.path.c_str());
	ss << "\n";
	ss << tfm::format("  index: %d", device.index);
	ss << "\n";
	ss << tfm::format("  fd: 0x%x", device.fd);
	ss << "\n";
	ss << tfm::format("  model: '%s'", device.model.c_str());
	ss << "\n";
	ss << tfm::format("  VID: 0x%04x, PID: 0x%04x", device.deviceID.vid, device.deviceID.pid);
	ss << "\n";
	ss << tfm::format("  type: 0x%x ('%s')", device.type._to_integral(), device.type._to_string());
	ss << "\n";

	switch (device.type)
	{
		case InputDeviceTypeEnum::Mouse:
			break;
		case InputDeviceTypeEnum::Joystick:
			break;
		case InputDeviceTypeEnum::Keyboard:
			{
				Keyboard device(device.name, device.path);

				device.openDevice();
				uint16_t ledBits = device.getDeviceLEDBits();
				device.closeDevice();

				ss << tfm::format("  LED bits: 0x%04x | %s", ledBits, BaseInputDevice::dumpLEDBits(ledBits).c_str());
				ss << "\n";
			}
			break;
		default:
			break;
	}

	return ss.str();
}
