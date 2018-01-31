#ifndef IO_INPUT_KEYBOARD_H_
#define IO_INPUT_KEYBOARD_H_

#include <stdint.h>
#include <map>
#include <string>
#include "../../common/consts.h"
#include "baseinputdevice.h"

using namespace std;

typedef map<uint16_t, bool> KeyStateMap;

class Keyboard : public BaseInputDevice
{
	friend class InputManager;

protected:
	unsigned long bit_key_state[BITFIELD_LONGS_PER_ARRAY(KEY_MAX)];
	unsigned long bit_led[BITFIELD_LONGS_PER_ARRAY(LED_MAX)];
	uint16_t supportedLEDBits = 0x0000;

	KeyStateMap m_keysState;

public:
	Keyboard(const string& name, const string& path);
	virtual ~Keyboard();

	// Common control methods
	void reset();

	// Key operations
	bool isKeyPressed(uint16_t key);
	bool isKeyPressed(unsigned long* keyBits, uint16_t key);
	bool pollKeys();
	int getPressedKeysCount();

	// LED operations
	uint16_t getLEDState();
	void setLEDState(uint16_t ledMask, bool on);

	// Key state operations
	void setKeyState(uint16_t key, bool state);
	bool getKeyState(uint16_t key);

	// Debug methods
	string dumpKeyBits();
	static string dumpKeyBits(unsigned long* keyBits);

protected:
	void makeLEDEvent(struct input_event* event, uint16_t ledMask, bool on);
};

#endif /* IO_INPUT_KEYBOARD_H_ */
