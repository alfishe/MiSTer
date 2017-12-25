#ifndef IO_INPUT_KEYBOARD_H_
#define IO_INPUT_KEYBOARD_H_

#include <stdint.h>
#include <string>
#include "../../common/consts.h"

using namespace std;

class keyboard
{
protected:
	int fd = INVALID_FILE_DESCRIPTOR;
	string deviceName;
	uint16_t supportedLEDMask;

public:
	keyboard(int fd);
	virtual ~keyboard();

	const string getDeviceName();
	bool hasLED();
	void getLEDState();
	void setLEDState(uint8_t state);

private:
	static inline int isBitSet(int bit, const uint8_t *array) __attribute__((always_inline))
	{
	    int result = array [bit / 8] & (1 << (bit % 8));

	    return result;
	}
};

#endif /* IO_INPUT_KEYBOARD_H_ */
