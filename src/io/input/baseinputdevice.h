#ifndef IO_INPUT_BASEINPUTDEVICE_H_
#define IO_INPUT_BASEINPUTDEVICE_H_

#include <stdint.h>
#include <string>
#include "../../common/consts.h"
#include "../../common/types.h"

using namespace std;

class BaseInputDevice
{
public:
	static string getInputDeviceName(int deviceIndex);
	static VIDPID getInputDeviceVIDPID(int deviceIndex);

	static string readDeviceStringValue(const string& deviceName);
	static uint16_t readDeviceHexValue(const string& deviceName);

protected:
	static const string getDeviceNameQuery();
	static const string getDeviceBusTypeQuery();
	static const string getDeviceVendorQuery();
	static const string getDeviceProductQuery();
};

#endif /* IO_INPUT_BASEINPUTDEVICE_H_ */
