#include "baseinputdevice.h"

#include "../../common/logger/logger.h"

#include <iostream>
#include <fstream>
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/helpers/stringhelper.h"

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

// ------------------- Protected methods
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
