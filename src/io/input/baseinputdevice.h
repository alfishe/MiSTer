#ifndef IO_INPUT_BASEINPUTDEVICE_H_
#define IO_INPUT_BASEINPUTDEVICE_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <linux/input.h>

#include "../../common/consts.h"
#include "../../common/types.h"
#include "input.h"

using namespace std;

typedef unsigned long BitType;

/*
 * Macros for reading ioctl bit fields.
 */
#define BITFIELD_BITS_PER_LONG      (sizeof(unsigned long) * 8)
#define BITFIELD_LONGS_PER_ARRAY(x) ((((x) - 1) / BITFIELD_BITS_PER_LONG) + 1)
#define BITFIELD_TEST(bit, array)   ((array[((bit) / BITFIELD_BITS_PER_LONG)] >> ((bit) % BITFIELD_BITS_PER_LONG)) & 0x1)
#define INPUT_BITCOUNT	(sizeof(BitType) * 8)
#define INPUT_BITS(n)	((n / INPUT_BITCOUNT) + 1)

class BaseInputDevice: public InputDevice
{
protected:
	static vector<string> eventNames;
	static vector<string> ledNames;

	// Ioctl input-related bit arrays
	unsigned long bit_ev[BITFIELD_LONGS_PER_ARRAY(EV_MAX)];
	unsigned long bit_key[BITFIELD_LONGS_PER_ARRAY(KEY_MAX)];
	unsigned long bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];

public:
	string path;
	int fd = INVALID_FILE_DESCRIPTOR;

	string name;
	InputDeviceTypeEnum type = InputDeviceTypeEnum::Unknown;

public:
	BaseInputDevice(const string& path);
	virtual ~BaseInputDevice();

	int openDevice();
	int openDeviceWrite();
	void closeDevice();

	bool init();
	const string getDeviceName();
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

	static string dumpEventBits(uint32_t value);
	static string dumpLEDBits(uint16_t value);

protected:
	static const string getDeviceNameQuery();
	static const string getDeviceBusTypeQuery();
	static const string getDeviceVendorQuery();
	static const string getDeviceProductQuery();

protected:
	static inline bool isBitSet(const BitType* bits, unsigned bit) __attribute__((always_inline))
	{
		bool result = bool(bits[bit / INPUT_BITCOUNT] & (1ul << (bit % INPUT_BITCOUNT)));

		return result;
	}

	static inline bool hasEventType(const BitType* bits, unsigned type) __attribute__((always_inline))
	{
		bool result  = isBitSet(bits, type);

		return result;
	}

	static inline bool hasEventCode(const BitType* bits, unsigned code) __attribute__((always_inline))
	{
		bool result = isBitSet(bits, code);

		return result;
	}
};

#endif /* IO_INPUT_BASEINPUTDEVICE_H_ */
