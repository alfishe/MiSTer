#ifndef IO_INPUT_KEYBOARD_H_
#define IO_INPUT_KEYBOARD_H_

#include <stdint.h>
#include <string>
#include "../../common/consts.h"
#include "baseinputdevice.h"

using namespace std;

class keyboard : public BaseInputDevice
{
protected:
	int fd = INVALID_FILE_DESCRIPTOR;
	string deviceName;
	uint16_t supportedLEDBits = 0x0000;

public:
	keyboard(const string& path);
	virtual ~keyboard();

	const string getDeviceName();
	void getLEDState();
	void setLEDState(uint16_t state);

private:
	static inline int isBitSet(int bit, const uint8_t *array) __attribute__((always_inline))
	{
	    int result = array [bit / 8] & (1 << (bit % 8));

	    return result;
	}
};

#endif /* IO_INPUT_KEYBOARD_H_ */
