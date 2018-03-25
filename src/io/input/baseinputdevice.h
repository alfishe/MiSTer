#ifndef IO_INPUT_BASEINPUTDEVICE_H_
#define IO_INPUT_BASEINPUTDEVICE_H_

#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <linux/input.h>

#include "../../common/consts.h"
#include "../../common/types.h"
#include "../../common/helpers/stringhelper.h"
#include "input.h"
#include "inputdevicehelper.h"

using namespace std;

class BaseInputDevice: public InputDevice
{
protected:
	static vector<string> eventNames;
	static vector<string> ledNames;
	static vector<string> synNames;
	static vector<string> mscNames;
	static vector<string> keyNames;
	static vector<string> relNames;
	static vector<string> absNames;

	// Ioctl input-related bit arrays
	unsigned long bit_ev[BITFIELD_LONGS_PER_ARRAY(EV_MAX)];
	unsigned long bit_key[BITFIELD_LONGS_PER_ARRAY(KEY_MAX)];
	unsigned long bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];

public:
	// Input device fields are located in a base InputDevice class

public:
	BaseInputDevice();
	BaseInputDevice(const string& name, const string& path);
	virtual ~BaseInputDevice();

	int openDevice();
	int openDeviceWrite();
	void closeDevice();

	bool init();
	const string getDeviceModel();
	const int getDeviceIndex();
	VIDPID getDeviceVIDPID();
	InputDeviceTypeEnum getDeviceType();

	static uint8_t getDeviceBus(int fd);

	uint32_t getDeviceEventBits();
	uint16_t getDeviceLEDBits();
	const BitType* getDeviceKeyBits();

	static string getInputDeviceName(int deviceIndex);
	static VIDPID getInputDeviceVIDPID(int deviceIndex);

	static string readDeviceStringValue(const string& deviceName);
	static uint16_t readDeviceHexValue(const string& deviceName);

	static string printDeviceInfo(int fd);
	static bool getDeviceState(int fd, uint8_t type, unsigned long *array, size_t size);

	// Debug methods
	static string dumpEventBits(uint32_t value);
	static string dumpLEDBits(uint16_t value);
	static string dumpLEDType(uint16_t value);
	static string dumpEventType(uint16_t value);
	static string dumpSynType(uint16_t value);
	static string dumpMscType(uint16_t value);
	static string dumpKey(uint16_t value);
	static string dumpRelType(uint16_t value);
	static string dumpAbsType(uint16_t value);

protected:
	static const string getDeviceNameQuery();
	static const string getDeviceBusTypeQuery();
	static const string getDeviceVendorQuery();
	static const string getDeviceProductQuery();
};

typedef map<string, BaseInputDevice*> BaseInputDeviceMap;
typedef pair<string, BaseInputDevice*> BaseInputDevicePair;
typedef map<int, BaseInputDevice*> FDBaseInputDeviceMap;

#endif /* IO_INPUT_BASEINPUTDEVICE_H_ */
