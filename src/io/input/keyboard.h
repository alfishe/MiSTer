#ifndef IO_INPUT_KEYBOARD_H_
#define IO_INPUT_KEYBOARD_H_

#include <stdint.h>
#include <string>
#include "../../common/consts.h"
#include "baseinputdevice.h"

using namespace std;

class Keyboard : public BaseInputDevice
{
protected:
	unsigned long bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];
	uint16_t supportedLEDBits = 0x0000;

public:
	Keyboard(const string& path);
	virtual ~Keyboard();

	uint16_t getLEDState();
	void setLEDState(uint16_t ledMask, bool on);

protected:
	void makeLEDEvent(struct input_event* event, uint16_t ledMask, bool on);
};

#endif /* IO_INPUT_KEYBOARD_H_ */
