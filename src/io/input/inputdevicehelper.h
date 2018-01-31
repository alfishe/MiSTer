#ifndef IO_INPUT_INPUTDEVICEHELPER_H_
#define IO_INPUT_INPUTDEVICEHELPER_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <string>
#include <vector>
#include <linux/input.h>

#include "../../common/consts.h"
#include "../../common/types.h"
#include "../../common/helpers/stringhelper.h"

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

class InputDeviceHelper
{
public:
	static const string getDeviceModel(int fd);
	static InputDeviceTypeEnum getDeviceType(int fd);
	static uint32_t getDeviceEventBits(int fd, BitType* bit_ev, size_t length);
	static BitType* getDeviceKeyBits(int fd, BitType* bit_key, size_t length);
	static uint16_t getDeviceLEDBits(int fd, BitType* bit_led, size_t length);

	static inline bool isDescriptorValid(int fd) __attribute__((always_inline))
	{
		bool result = false;

		if (fd != INVALID_FILE_DESCRIPTOR && fcntl(fd, F_GETFD) != -1)
			result = true;

		return result;
	}

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

	// Returns number of set bits for the whole bit set array
	// Note - size is in unsigned long's
	static int bitCount(const BitType* bits, size_t size)
	{
		int result = 0;

		for (size_t i = 0; i < size; i++)
		{
			result += bitCount(bits[i]);
		}

		return result;
	}

	// Returns number of set bits in unsigned 32 bit value
	static inline int bitCount(BitType n) __attribute__((always_inline))
	{
		n = ((0xaaaaaaaa & n) >> 1) + (0x55555555 & n);
		n = ((0xcccccccc & n) >> 2) + (0x33333333 & n);
		n = ((0xf0f0f0f0 & n) >> 4) + (0x0f0f0f0f & n);
		n = ((0xff00ff00 & n) >> 8) + (0x00ff00ff & n);
		n = ((0xffff0000 & n) >> 16) + (0x0000ffff & n);

		return (int)n;
	}
};

#endif /* IO_INPUT_INPUTDEVICEHELPER_H_ */
