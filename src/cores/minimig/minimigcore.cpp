#include "minimigcore.h"

#include "../../common/logger/logger.h"

#include "../../io/input/input.h"
#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgaconnector.h"
#include "../../fpga/fpgacommand.h"

MinimigCore::MinimigCore()
{
}

MinimigCore::~MinimigCore()
{
}

void MinimigCore::translateKeypress(uint16_t key, bool pressed)
{

}

void MinimigCore::sendKey(uint16_t key, bool pressed)
{
	uint32_t code = getAmigaKeyCode(key);
	if (code == NONE)
		return;

	if (code & CAPS_TOGGLE)
	{
		if (pressed)
		{
			// send alternating make and break codes for caps lock
			/*
			if (caps_lock_toggle)
				code |= 0x80;
			caps_lock_toggle = !caps_lock_toggle;
			set_kbd_led(HID_LED_CAPS_LOCK, caps_lock_toggle);
			*/
		}
	}
	else
	{
		// amiga has "break" marker in msb
		if (!pressed)
			code |= 0x80;
	}

	code &= 0xff;

	// Send to FPGA as fast as possible
	/*
	if (CheckTimer(kbd_timer) && (kbd_fifo_w == kbd_fifo_r))
	{
		kbd_fifo_minimig_send(code);
	}
	else
	{
		kbd_fifo_enqueue(code);
	}
	*/
}

void MinimigCore::translateMouseCoordinates(int16_t x, int16_t y, uint8_t buttons)
{

}

// Minimig specific methods
uint32_t MinimigCore::getAmigaKeyCode(uint16_t key)
{

}
