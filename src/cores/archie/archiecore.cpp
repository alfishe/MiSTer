#include "archiecore.h"

ArchieCore::ArchieCore()
{
}

ArchieCore::~ArchieCore()
{
}

void ArchieCore::translateKeypress(uint16_t key, bool pressed)
{

}

void ArchieCore::sendKey(uint16_t key, bool pressed)
{

}

void ArchieCore::translateMouseCoordinates(int16_t x, int16_t y, uint8_t buttons)
{
	/*
	archie_debugf("KBD MOUSE X:%d Y:%d B:%d", x, y, b);

	// max values -64 .. 63
	mouse_x += x;
	if (mouse_x >  63) mouse_x = 63;
	if (mouse_x < -64) mouse_x = -64;

	mouse_y -= y;
	if (mouse_y >  63) mouse_y = 63;
	if (mouse_y < -64) mouse_y = -64;

	// don't send anything yet if we are still in reset state
	if (kbd_state <= STATE_RAK2)
	{
		archie_debugf("KBD still in reset");
		return;
	}

	// ignore any mouse movement if mouse is disabled or if nothing to report
	if ((flags & FLAG_MOUSE_ENABLED) && (mouse_x || mouse_y))
	{
		// send asap if no pending byte
		if (kbd_state == STATE_IDLE) {
			archie_kbd_send(STATE_WAIT4ACK1, mouse_x & 0x7f);
			archie_kbd_send(STATE_WAIT4ACK2, mouse_y & 0x7f);
			mouse_x = mouse_y = 0;
		}
	}

	// ignore mouse buttons if key scanning is disabled
	if (flags & FLAG_SCAN_ENABLED)
	{
		static const uint8_t remap[] = { 0, 2, 1 };
		static unsigned char buts = 0;
		uint8_t s;

		// map all three buttons
		for (s = 0; s<3; s++)
		{
			uint8_t mask = (1 << s);
			if ((b&mask) != (buts&mask))
			{
				unsigned char prefix = (b&mask) ? KDDA : KUDA;
				archie_kbd_send(STATE_WAIT4ACK1, prefix | 0x07);
				archie_kbd_send(STATE_WAIT4ACK2, prefix | remap[s]);
			}
		}
		buts = b;
	}
	*/
}
