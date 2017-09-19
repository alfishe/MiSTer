#ifndef IO_INPUT_INPUT_H_
#define IO_INPUT_INPUT_H_

#define HID_LED_NUM_LOCK    1
#define HID_LED_CAPS_LOCK   2
#define HID_LED_SCROLL_LOCK 4
#define HID_LED_MASK        7

#define NONE         0xFF
#define LCTRL        0x000100
#define LSHIFT       0x000200
#define LALT         0x000400
#define LGUI         0x000800
#define RCTRL        0x001000
#define RSHIFT       0x002000
#define RALT         0x004000
#define RGUI         0x008000
#define MODMASK      0x00FF00

#define OSD          0x010000  // to be used by OSD, not the core itself
#define OSD_OPEN     0x020000  // OSD key not forwarded to core, but queued in arm controller
#define CAPS_TOGGLE  0x040000  // caps lock toggle behaviour
#define EXT          0x080000
#define EMU_SWITCH_1 0x100000
#define EMU_SWITCH_2 0x200000

#define UPSTROKE     0x400000

#endif /* IO_INPUT_INPUT_H_ */
