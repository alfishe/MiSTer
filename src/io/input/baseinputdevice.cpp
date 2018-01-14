#include "baseinputdevice.h"

#include "../../common/logger/logger.h"

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "../../3rdparty/tinyformat/tinyformat.h"
#include "../../common/helpers/displayhelper.h"
#include "../../common/helpers/stringhelper.h"
#include "../../common/file/path/path.h"

#define IS_BIT_SET(var, pos) ((var) & (1 << (pos)))

// Check if correspondent <bit> set in a bitset
#define test_bit(bit, bitset) (bitset [bit / 8] & (1 << (bit % 8)))
#define NBITS(x) ((((x) - 1) / LONG_BIT) + 1)

// Initialize static field arrays

vector<string> BaseInputDevice::eventNames =
{
	"EV_SYN",		// 0x00
	"EV_KEY",		// 0x01
	"EV_REL",		// 0x02
	"EV_ABS",		// 0x03
	"EV_MSC",		// 0x04
	"EV_SW",			// 0x05
	"UNKNOWN 0x06",	// 0x06
	"UNKNOWN 0x07",	// 0x07
	"UNKNOWN 0x08",	// 0x08
	"UNKNOWN 0x09",	// 0x09
	"UNKNOWN 0x0A",	// 0x0A
	"UNKNOWN 0x0B",	// 0x0B
	"UNKNOWN 0x0C",	// 0x0C
	"UNKNOWN 0x0D",	// 0x0D
	"UNKNOWN 0x0E",	// 0x0E
	"UNKNOWN 0x0F",	// 0x0F
	"UNKNOWN 0x10",	// 0x10
	"EV_LED",		// 0x11
	"EV_SND",		// 0x12
	"UNKNOWN 0x13",	// 0x13
	"EV_REP",		// 0x14
	"EV_FF",			// 0x15
	"EV_PWR",		// 0x16
	"EV_FF_STATUS",	// 0x17
	"UNKNOWN 0x18",	// 0x18
	"UNKNOWN 0x19",	// 0x19
	"UNKNOWN 0x1A",	// 0x1A
	"UNKNOWN 0x1B",	// 0x1B
	"UNKNOWN 0x1C",	// 0x1C
	"UNKNOWN 0x1D",	// 0x1D
	"UNKNOWN 0x1E",	// 0x1E
	"UNKNOWN 0x1F"	// 0x1F
};

vector<string> BaseInputDevice::ledNames =
{
	"LED_NUML",			// 0x00
	"LED_CAPSL",			// 0x01
	"LED_SCROLLL",		// 0x02
	"LED_COMPOSE",		// 0x03
	"LED_KANA",			// 0x04
	"LED_SLEEP",			// 0x05
	"LED_SUSPEND",		// 0x06
	"LED_MUTE",			// 0x07
	"LED_MISC",			// 0x08
	"LED_MAIL",			// 0x09
	"LED_CHARGING",		// 0x0A
	"UNDEFINED 0x0B",	// 0x0B
	"UNDEFINED 0x0C",	// 0x0C
	"UNDEFINED 0x0D",	// 0x0D
	"UNDEFINED 0x0E",	// 0x0E
	"UNDEFINED 0x0F"		// 0x0F
};

vector<string> BaseInputDevice::synNames =
{
	"SYN_REPORT",		// 0x00
	"SYN_CONFIG",		// 0x01
	"SYN_MT_REPORT",		// 0x02
	"SYN_DROPPED	",		// 0x03
};

vector<string> BaseInputDevice::mscNames =
{
	"MSC_SERIAL",		// 0x00
	"MSC_PULSELED",		// 0x01
	"MSC_GESTURE",		// 0x02
	"MSC_RAW",			// 0x03
	"MSC_SCAN",			// 0x04
	"MSC_TIMESTAMP",		// 0x05
	"UNKNOWN_0x06",		// 0x06
};

vector<string> BaseInputDevice::keyNames =
{
	"KEY_RESERVED",			// 0x00
	"KEY_ESC",				// 0x01
	"KEY_1",					// 0x02
	"KEY_2",					// 0x03
	"KEY_3",					// 0x04
	"KEY_4",					// 0x05
	"KEY_5",					// 0x06
	"KEY_6",					// 0x07
	"KEY_7",					// 0x08
	"KEY_8",					// 0x09
	"KEY_9",					// 0x0A
	"KEY_0",					// 0x0B
	"KEY_MINUS",				// 0x0C
	"KEY_EQUAL",				// 0x0D
	"KEY_BACKSPACE",			// 0x0E
	"KEY_TAB",				// 0x0F
	"KEY_Q",					// 0x10
	"KEY_W",					// 0x11
	"KEY_E",					// 0x12
	"KEY_R",					// 0x13
	"KEY_T",					// 0x14
	"KEY_Y",					// 0x15
	"KEY_U",					// 0x16
	"KEY_I",					// 0x17
	"KEY_O",					// 0x18
	"KEY_P",					// 0x19
	"KEY_LEFTBRACE",			// 0x1A
	"KEY_RIGHTBRACE",		// 0x1B
	"KEY_ENTER",				// 0x1C
	"KEY_LEFTCTRL",			// 0x1D
	"KEY_A",					// 0x1E
	"KEY_S",					// 0x1F
	"KEY_D",					// 0x20
	"KEY_F",					// 0x21
	"KEY_G",					// 0x22
	"KEY_H",					// 0x23
	"KEY_J",					// 0x24
	"KEY_K",					// 0x25
	"KEY_L",					// 0x26
	"KEY_SEMICOLON",			// 0x27
	"KEY_APOSTROPHE",		// 0x28
	"KEY_GRAVE",				// 0x29
	"KEY_LEFTSHIFT",			// 0x2A
	"KEY_BACKSLASH",			// 0x2B
	"KEY_Z",					// 0x2C
	"KEY_X",					// 0x2D
	"KEY_C",					// 0x2E
	"KEY_V",					// 0x2F
	"KEY_B",					// 0x30
	"KEY_N",					// 0x31
	"KEY_M",					// 0x32
	"KEY_COMMA",				// 0x33
	"KEY_DOT",				// 0x34
	"KEY_SLASH",				// 0x35
	"KEY_RIGHTSHIFT",		// 0x36
	"KEY_KPASTERISK",		// 0x37
	"KEY_LEFTALT",			// 0x38
	"KEY_SPACE",				// 0x39
	"KEY_CAPSLOCK",			// 0x3A
	"KEY_F1",				// 0x3B
	"KEY_F2",				// 0x3C
	"KEY_F3",				// 0x3D
	"KEY_F4",				// 0x3E
	"KEY_F5",				// 0x3F
	"KEY_F6",				// 0x40
	"KEY_F7",				// 0x41
	"KEY_F8",				// 0x42
	"KEY_F9",				// 0x43
	"KEY_F10",				// 0x44
	"KEY_NUMLOCK",			// 0x45
	"KEY_SCROLLLOCK",		// 0x46
	"KEY_KP7",				// 0x47
	"KEY_KP8",				// 0x48
	"KEY_KP9",				// 0x49
	"KEY_KPMINUS",			// 0x4A
	"KEY_KP4",				// 0x4B
	"KEY_KP5",				// 0x4C
	"KEY_KP6",				// 0x4D
	"KEY_KPPLUS",			// 0x4E
	"KEY_KP1",				// 0x4F
	"KEY_KP2",				// 0x50
	"KEY_KP3",				// 0x51
	"KEY_KP0",				// 0x52
	"KEY_KPDOT",				// 0x53
	"UNKNOWN_0x54",			// 0x54
	"KEY_ZENKAKUHANKAKU",	// 0x55
	"KEY_102ND",				// 0x56
	"KEY_F11",				// 0x57
	"KEY_F12",				// 0x58
	"KEY_RO",				// 0x59
	"KEY_KATAKANA",			// 0x5A
	"KEY_HIRAGANA",			// 0x5B
	"KEY_HENKAN",			// 0x5C
	"KEY_KATAKANAHIRAGANA", 	// 0x5D
	"KEY_MUHENKAN",			// 0x5E
	"KEY_KPJPCOMMA",			// 0x5F
	"KEY_KPENTER",			// 0x60
	"KEY_RIGHTCTRL",			// 0x61
	"KEY_KPSLASH",			// 0x62
	"KEY_SYSRQ",				// 0x63
	"KEY_RIGHTALT",			// 0x64
	"KEY_LINEFEED",			// 0x65
	"KEY_HOME",				// 0x66
	"KEY_UP",				// 0x67
	"KEY_PAGEUP",			// 0x68
	"KEY_LEFT",				// 0x69
	"KEY_RIGHT",				// 0x6A
	"KEY_END",				// 0x6B
	"KEY_DOWN",				// 0x6C
	"KEY_PAGEDOWN",			// 0x6D
	"KEY_INSERT",			// 0x6E
	"KEY_DELETE",			// 0x6F
	"KEY_MACRO",				// 0x70
	"KEY_MUTE",				// 0x71
	"KEY_VOLUMEDOWN",		// 0x72
	"KEY_VOLUMEUP",			// 0x73
	"KEY_POWER",				// 0x74
	"KEY_KPEQUAL",			// 0x75
	"KEY_KPPLUSMINUS",		// 0x76
	"KEY_PAUSE",				// 0x77
	"KEY_SCALE",				// 0x78
	"KEY_KPCOMMA",			// 0x79
	"KEY_HANGUEL",			// 0x7A
	"KEY_HANJA",				// 0x7B
	"KEY_YEN",				// 0x7C
	"KEY_LEFTMETA",			// 0x7D
	"KEY_RIGHTMETA",			// 0x7E
	"KEY_COMPOSE",			// 0x7F

	"KEY_STOP",				// 0x80
	"KEY_AGAIN",				// 0x81
	"KEY_PROPS",				// 0x82
	"KEY_UNDO",				// 0x83
	"KEY_FRONT",				// 0x84
	"KEY_COPY",				// 0x85
	"KEY_OPEN",				// 0x86
	"KEY_PASTE",				// 0x87
	"KEY_FIND",				// 0x88
	"KEY_CUT",				// 0x89
	"KEY_HELP",				// 0x8A
	"KEY_MENU",				// 0x8B
	"KEY_CALC",				// 0x8C
	"KEY_SETUP",				// 0x8D
	"KEY_SLEEP",				// 0x8E
	"KEY_WAKEUP",			// 0x8F
	"KEY_FILE",				// 0x90
	"KEY_SENDFILE",			// 0x91
	"KEY_DELETEFILE",		// 0x92
	"KEY_XFER",				// 0x93
	"KEY_PROG1",				// 0x94
	"KEY_PROG2",				// 0x95
	"KEY_WWW",				// 0x96
	"KEY_MSDOS",				// 0x97
	"KEY_COFFEE",			// 0x98
	"KEY_ROTATE_DISPLAY",	// 0x99
	"KEY_CYCLEWINDOWS",		// 0x9A
	"KEY_MAIL",				// 0x9B
	"KEY_BOOKMARKS",			// 0x9C
	"KEY_COMPUTER",			// 0x9D
	"KEY_BACK",				// 0x9E
	"KEY_FORWARD",			// 0x9F
	"KEY_CLOSECD",			// 0xA0
	"KEY_EJECTCD",			// 0xA1
	"KEY_EJECTCLOSECD",		// 0xA2
	"KEY_NEXTSONG",			// 0xA3
	"KEY_PLAYPAUSE",			// 0xA4
	"KEY_PREVIOUSSONG",		// 0xA5
	"KEY_STOPCD",			// 0xA6
	"KEY_RECORD",			// 0xA7
	"KEY_REWIND",			// 0xA8
	"KEY_PHONE",				// 0xA9
	"KEY_ISO",				// 0xAA
	"KEY_CONFIG",			// 0xAB
	"KEY_HOMEPAGE",			// 0xAC
	"KEY_REFRESH",			// 0xAD
	"KEY_EXIT",				// 0xAE
	"KEY_MOVE",				// 0xAF
	"KEY_EDIT",				// 0xB0
	"KEY_SCROLLUP",			// 0xB1
	"KEY_SCROLLDOWN",		// 0xB2
	"KEY_KPLEFTPAREN",		// 0xB3
	"KEY_KPRIGHTPAREN",		// 0xB4
	"KEY_NEW",				// 0xB5
	"KEY_REDO",				// 0xB6
	"KEY_F13",				// 0xB7
	"KEY_F14",				// 0xB8
	"KEY_F15",				// 0xB9
	"KEY_F16",				// 0xBA
	"KEY_F17",				// 0xBB
	"KEY_F18",				// 0xBC
	"KEY_F19",				// 0xBD
	"KEY_F20",				// 0xBE
	"KEY_F21",				// 0xBF
	"KEY_F22",				// 0xC0
	"KEY_F23",				// 0xC1
	"KEY_F24",				// 0xC2
	"UNKNOWN_0xC3",			// 0xC3
	"UNKNOWN_0xC4",			// 0xC4
	"UNKNOWN_0xC5",			// 0xC5
	"UNKNOWN_0xC6",			// 0xC6
	"UNKNOWN_0xC7",			// 0xC7
	"KEY_PLAYCD",			// 0xC8
	"KEY_PAUSECD",			// 0xC9
	"KEY_PROG3",				// 0xCA
	"KEY_PROG4",				// 0xCB
	"KEY_DASHBOARD",			// 0xCC
	"KEY_SUSPEND",			// 0xCD
	"KEY_CLOSE",				// 0xCE
	"KEY_PLAY",				// 0xCF
	"KEY_FASTFORWARD",		// 0xD0
	"KEY_BASSBOOST",			// 0xD1
	"KEY_PRINT",				// 0xD2
	"KEY_HP",				// 0xD3
	"KEY_CAMERA",			// 0xD4
	"KEY_SOUND",				// 0xD5
	"KEY_QUESTION",			// 0xD6
	"KEY_EMAIL",				// 0xD7
	"KEY_CHAT",				// 0xD8
	"KEY_SEARCH",			// 0xD9
	"KEY_CONNECT",			// 0xDA
	"KEY_FINANCE",			// 0xDB
	"KEY_SPORT",				// 0xDC
	"KEY_SHOP",				// 0xDD
	"KEY_ALTERASE",			// 0xDE
	"KEY_CANCEL	",			// 0xDF
	"KEY_BRIGHTNESSDOWN",	// 0xE0
	"KEY_BRIGHTNESSUP",		// 0xE1
	"KEY_MEDIA",				// 0xE2
	"KEY_SWITCHVIDEOMODE",	// 0xE3
	"KEY_KBDILLUMTOGGLE",	// 0xE4
	"KEY_KBDILLUMDOWN",		// 0xE5
	"KEY_KBDILLUMUP",		// 0xE6
	"KEY_SEND",				// 0xE7
	"KEY_REPLY",				// 0xE8
	"KEY_FORWARDMAIL",		// 0xE9
	"KEY_SAVE",				// 0xEA
	"KEY_DOCUMENTS",			// 0xEB
	"KEY_BATTERY",			// 0xEC
	"KEY_BLUETOOTH",			// 0xED
	"KEY_WLAN",				// 0xEE
	"KEY_UWB",				// 0xEF
	"KEY_UNKNOWN",			// 0xF0
	"KEY_VIDEO_NEXT",		// 0xF1
	"KEY_VIDEO_PREV",		// 0xF2
	"KEY_BRIGHTNESS_CYCLE",	// 0xF3
	"KEY_BRIGHTNESS_AUTO",	// 0xF4
	"KEY_DISPLAY_OFF",		// 0xF5
	"KEY_WWAN",				// 0xF6
	"KEY_RFKILL",			// 0xF7
	"KEY_MICMUTE",			// 0xF8
	"UNKNOWN_0xF9",			// 0xF9
	"UNKNOWN_0xFA",			// 0xFA
	"UNKNOWN_0xFB",			// 0xFB
	"UNKNOWN_0xFC",			// 0xFC
	"UNKNOWN_0xFD",			// 0xFD
	"UNKNOWN_0xFE",			// 0xFE
	"RESERVED_AT",			// 0xFF

	"BTN_0", 				// 0x0100
	"BTN_1", 				// 0x0101
	"BTN_2", 				// 0x0102
	"BTN_3", 				// 0x0103
	"BTN_4", 				// 0x0104
	"BTN_5", 				// 0x0105
	"BTN_6", 				// 0x0106
	"BTN_7", 				// 0x0107
	"BTN_8", 				// 0x0108
	"BTN_9", 				// 0x0109
	"UNKNOWN_0x010A",		// 0x010A
	"UNKNOWN_0x010B",		// 0x010B
	"UNKNOWN_0x010C",		// 0x010C
	"UNKNOWN_0x010D",		// 0x010D
	"UNKNOWN_0x010E",		// 0x010E
	"UNKNOWN_0x010F",		// 0x010F
	"BTN_LEFT", 				// 0x0110
	"BTN_RIGHT", 			// 0x0111
	"BTN_MIDDLE", 			// 0x0112
	"BTN_SIDE", 				// 0x0113
	"BTN_EXTRA", 			// 0x0114
	"BTN_FORWARD", 			// 0x0115
	"BTN_BACK", 				// 0x0116
	"BTN_TASK", 				// 0x0117
	"UNKNOWN_0x0118",		// 0x0118
	"UNKNOWN_0x0119",		// 0x0119
	"UNKNOWN_0x011A",		// 0x011A
	"UNKNOWN_0x011B",		// 0x011B
	"UNKNOWN_0x011C",		// 0x011C
	"UNKNOWN_0x011D",		// 0x011D
	"UNKNOWN_0x011E",		// 0x011E
	"UNKNOWN_0x011F",		// 0x011F
	"BTN_TRIGGER", 			// 0x0120
	"BTN_THUMB", 			// 0x0121
	"BTN_THUMB2", 			// 0x0122
	"BTN_TOP", 				// 0x0123
	"BTN_TOP2", 				// 0x0124
	"BTN_PINKIE", 			// 0x0125
	"BTN_BASE", 				// 0x0126
	"BTN_BASE2", 			// 0x0127
	"BTN_BASE3", 			// 0x0128
	"BTN_BASE4", 			// 0x0129
	"BTN_BASE5", 			// 0x012A
	"BTN_BASE6", 			// 0x012B
	"UNKNOWN_0x012C",		// 0x012C
	"UNKNOWN_0x012D",		// 0x012D
	"UNKNOWN_0x012E",		// 0x012E
	"BTN_DEAD", 				// 0x012F
	"BTN_A", 				// 0x0130
	"BTN_B", 				// 0x0131
	"BTN_C", 				// 0x0132
	"BTN_X", 				// 0x0133
	"BTN_Y", 				// 0x0134
	"BTN_Z", 				// 0x0135
	"BTN_TL", 				// 0x0136
	"BTN_TR", 				// 0x0137
	"BTN_TL2", 				// 0x0138
	"BTN_TR2", 				// 0x0139
	"BTN_SELECT", 			// 0x013A
	"BTN_START", 			// 0x013B
	"BTN_MODE", 				// 0x013C
	"BTN_THUMBL", 			// 0x013D
	"BTN_THUMBR", 			// 0x013E
	"UNKNOWN_0x013F",		// 0x013F
	"BTN_TOOL_PEN", 			// 0x0140
	"BTN_TOOL_RUBBER", 		// 0x0141
	"BTN_TOOL_BRUSH", 		// 0x0142
	"BTN_TOOL_PENCIL", 		// 0x0143
	"BTN_TOOL_AIRBRUSH", 	// 0x0144
	"BTN_TOOL_FINGER", 		// 0x0145
	"BTN_TOOL_MOUSE", 		// 0x0146
	"BTN_TOOL_LENS", 		// 0x0147
	"BTN_TOOL_QUINTTAP", 	// 0x0148
	"UNKNOWN_0x0149",		// 0x0149
	"BTN_TOUCH", 			// 0x014A
	"BTN_STYLUS", 			// 0x014B
	"BTN_STYLUS2", 			// 0x014C
	"BTN_TOOL_DOUBLETAP", 	// 0x014D
	"BTN_TOOL_TRIPLETAP", 	// 0x014E
	"BTN_TOOL_QUADTAP", 		// 0x014F
	"BTN_WHEEL", 			// 0x0150
	"BTN_GEAR_UP", 			// 0x0151
};

vector<string> BaseInputDevice::relNames =
{
	"REL_X",			// 0x00
	"REL_Y",			// 0x01
	"REL_Z",			// 0x02
	"REL_RX",		// 0x03
	"REL_RY",		// 0x04
	"REL_RZ",		// 0x05
	"REL_HWHEEL",	// 0x06
	"REL_DIAL",		// 0x07
	"REL_WHEEL",		// 0x08
	"REL_MISC",		// 0x09
	"UNKNOWN_0x0A",	// 0x0A
	"UNKNOWN_0x0B",	// 0x0B
	"UNKNOWN_0x0C",	// 0x0C
	"UNKNOWN_0x0D",	// 0x0D
	"UNKNOWN_0x0E",	// 0x0E
	"UNKNOWN_0x0F",	// 0x0F
};

vector<string> BaseInputDevice::absNames =
{
	"ABS_X",					// 0x00
	"ABS_Y",					// 0x01
	"ABS_Z",					// 0x02
	"ABS_RX",				// 0x03
	"ABS_RY",				// 0x04
	"ABS_RZ",				// 0x05
	"ABS_THROTTLE",			// 0x06
	"ABS_RUDDER",			// 0x07
	"ABS_WHEEL",				// 0x08
	"ABS_GAS",				// 0x09
	"ABS_BRAKE",				// 0x0A
	"ABS_HAT0X",				// 0x10
	"ABS_HAT0Y",				// 0x11
	"ABS_HAT1X",				// 0x12
	"ABS_HAT1Y",				// 0x13
	"ABS_HAT2X",				// 0x14
	"ABS_HAT2Y",				// 0x15
	"ABS_HAT3X",				// 0x16
	"ABS_HAT3Y",				// 0x17
	"ABS_PRESSUR",			// 0x18
	"ABS_DISTANC",			// 0x19
	"ABS_TILT_X",			// 0x1A
	"ABS_TILT_Y",			// 0x1B
	"ABS_TOOL_WIDTH",		// 0x1C
	"UNKNOWN_0x1D",			// 0x1D
	"UNKNOWN_0x1E",			// 0x1E
	"UNKNOWN_0x1F",			// 0x1F
	"ABS_VOLUME",			// 0x20
	"UNKNOWN_0x21",			// 0x21
	"UNKNOWN_0x22",			// 0x22
	"UNKNOWN_0x23",			// 0x23
	"UNKNOWN_0x24",			// 0x24
	"UNKNOWN_0x25",			// 0x25
	"UNKNOWN_0x26",			// 0x26
	"UNKNOWN_0x27",			// 0x27
	"ABS_MISC",				// 0x28
	"UNKNOWN_0x29",			// 0x29
	"UNKNOWN_0x2A",			// 0x2A
	"UNKNOWN_0x2B",			// 0x2B
	"UNKNOWN_0x2C",			// 0x2C
	"UNKNOWN_0x2D",			// 0x2D
	"UNKNOWN_0x2E",			// 0x2E
	"ABS_MT_SLOT",			// 0x2F
	"ABS_MT_TOUCH_MAJOR",	// 0x30
	"ABS_MT_TOUCH_MINOR",	// 0x31
	"ABS_MT_WIDTH_MAJOR",	// 0x32
	"ABS_MT_WIDTH_MINOR",	// 0x33
	"ABS_MT_ORIENTATION",	// 0x34
	"ABS_MT_POSITION_X",		// 0x35
	"ABS_MT_POSITION_Y",		// 0x36
	"ABS_MT_TOOL_TYPE",		// 0x37
	"ABS_MT_BLOB_ID",		// 0x38
	"ABS_MT_TRACKING_ID",	// 0x39
	"ABS_MT_PRESSURE",		// 0x3A
	"ABS_MT_DISTANCE",		// 0x3B
	"ABS_MT_TOOL_X",			// 0x3C
	"ABS_MT_TOOL_Y",			// 0x3D
	"UNKNOWN_0x3E",			// 0x3E
	"UNKNOWN_0x3F",			// 0x3F
};

// -Initialize static field arrays

BaseInputDevice::BaseInputDevice(const string& event, const string& path)
{
	TRACE("BaseInputDevice()");

	this->name = string(event);
	this->path = string(path);
}

BaseInputDevice::~BaseInputDevice()
{
	TRACE("~BaseInputDevice()");

	closeDevice();
}

int BaseInputDevice::openDevice()
{
	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		closeDevice();
	}

	fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);

	return fd;
}

int BaseInputDevice::openDeviceWrite()
{
	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		closeDevice();
	}

	fd = open(path.c_str(), O_RDWR | O_NONBLOCK);

	return fd;
}

void BaseInputDevice::closeDevice()
{
	if (fd != INVALID_FILE_DESCRIPTOR)
	{
		close(fd);

		fd = INVALID_FILE_DESCRIPTOR;
	}
}

bool BaseInputDevice::init()
{
	bool result = true;

	openDevice();

	// 1. Get input device index
	index = getDeviceIndex();

	// 2. Retrieve device name
	model = getDeviceName();

	// 3. Get device type
	type = getDeviceType();

	// 4. Get device USB VID / PID (if available)
	deviceID = getDeviceVIDPID();

	closeDevice();

	return result;
}

const string BaseInputDevice::getDeviceName()
{
	static const int EVENT_BUFFER_SIZE = 256;

	string result;
	result.resize(EVENT_BUFFER_SIZE);

	int res = ioctl(fd, EVIOCGNAME(EVENT_BUFFER_SIZE), result.c_str());
	if (res >= 0 && res <= EVENT_BUFFER_SIZE)
	{
		result.resize(res);
	}

	return result;
}

const int BaseInputDevice::getDeviceIndex()
{
	int result = -1;

	if (path.size() > 0)
	{
		result = StringHelper::getIntegerRegex(path, REGEX_INPUT_DEVICE_INDEX);
	}

	return result;
}

VIDPID BaseInputDevice::getDeviceVIDPID()
{
	VIDPID result;

	if (index != -1)
	{
		// Retrieve input device VID and PID using access to devices:
		// '/sys/class/input/event%d/device/id/vendor'
		// '/sys/class/input/event%d/device/id/product'
		string vidDeviceName = tfm::format(LINUX_INPUT_DEVICE_VID, index);
		string pidDeviceName = tfm::format(LINUX_INPUT_DEVICE_PID, index);

		result.vid = readDeviceHexValue(vidDeviceName);
		result.pid = readDeviceHexValue(pidDeviceName);
	}

	return result;
}

InputDeviceTypeEnum BaseInputDevice::getDeviceType()
{
	InputDeviceTypeEnum result = InputDeviceTypeEnum::Unknown;

	getDeviceEventBits();
	getDeviceKeyBits();

	// Detect Mouse device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// If device supports relative coordinates and supports at least left button - most likely it's a mouse
		if (hasEventType(bit_ev, EV_REL) &&
			hasEventType(bit_ev, EV_KEY) && hasEventCode(bit_key, BTN_LEFT)
			)
		{
			result = InputDeviceTypeEnum::Mouse;
		}
	}

	// Detect Joystick device
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// If device supports absolute coordinates and has gamepad button or joystick button - most likely it's a joystick
		if (hasEventType(bit_ev, EV_ABS) &&
			hasEventType(bit_ev, EV_KEY) &&
			(hasEventCode(bit_key, BTN_GAMEPAD) || hasEventCode(bit_key, BTN_JOYSTICK))
			)
		{
			result = InputDeviceTypeEnum::Joystick;
		}
	}

	// Detect Keyboard
	if (result == +InputDeviceTypeEnum::Unknown)
	{
		// Keyboard needs to have at least few letter buttons
		// TODO: Numpads detection (keyboard HID but have only numeric keys)
		if (hasEventType(bit_ev, EV_KEY) &&
			hasEventCode(bit_key, KEY_A) &&
			hasEventCode(bit_key, KEY_Z)
			)
		{
			uint16_t ledBits = getDeviceLEDBits();
			if (ledBits > 0)
			{
				result = InputDeviceTypeEnum::Keyboard;
			}
		}
	}

	return result;
}

uint32_t BaseInputDevice::getDeviceEventBits()
{
	uint32_t result = 0x00000000;

	if (ioctl(fd, EVIOCGBIT(0, sizeof(bit_ev)), &bit_ev) >= 0)
	{
		result = (uint32_t)bit_ev[0];

		//TRACE("Device event bits: 0x%08x | %s", result, DisplayHelper::formatBits(result).c_str());
		//TRACE("Event bits: %s", dumpEventBits(result).c_str());
		//TRACE("");
	}
	else
	{
		LOGERROR("Unable to retrieve event bits for device");
	}

	return result;
}

uint16_t BaseInputDevice::getDeviceLEDBits()
{
	uint16_t result = 0x0000;

	if (ioctl(fd, EVIOCGBIT(EV_LED, sizeof(bit_led)), &bit_led) >= 0)
	{
		result = (uint16_t)bit_led[0];

		//TRACE("Device LED bits: 0x%04x | %s", result, DisplayHelper::formatBits(result).c_str());
		//TRACE("LED bits: %s", dumpLEDBits(result).c_str());
		//TRACE("");
	}
	else
	{
		LOGERROR("Unable to retrieve LED bits for device");
	}

	return result;
}

const BitType* BaseInputDevice::getDeviceKeyBits()
{
	if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(bit_key)), &bit_key) >= 0)
	{
	}
	else
	{
		LOGERROR("Unable to retrieve Key bits for device");
	}

	return bit_key;
}

string BaseInputDevice::printDeviceInfo(int fd)
{
	string result;

	uint8_t type;
	uint16_t code;
	uint32_t version;
	uint16_t id[4];
	char name[256] = "Unknown";
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	unsigned long state[KEY_CNT] = {0};

	// Retrieve device version information
	if (ioctl(fd, EVIOCGVERSION, &version))
	{
		LOGERROR("printDeviceInfo: can't get version");
		return result;
	}
	string strVersion = tfm::format("Input driver version is %d.%d.%d\n",
									version >> 16,
									(version >> 8) & 0xff,
									version & 0xff);

	// Retrieve IDs (Bus, Vendor - VID, Product - PID, Version)
	ioctl(fd, EVIOCGID, id);
	string strIDs = tfm::format("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
								id[ID_BUS],
								id[ID_VENDOR],
								id[ID_PRODUCT],
								id[ID_VERSION]);

	// Retrieve device name
	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	string strName = tfm::format("Input device name: \"%s\"\n", name);

	// Retrieve information about supported events
	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	string strEvents = "Supported events:\n";

	int stateval;
	bool have_state;
	for (type = 0; type < EV_MAX; type++)
	{
		if (test_bit(type, bit[0]) && type != EV_REP)
		{
			strEvents.append(tfm::format("  Event type 0x%X (uint16)\n", type));

			have_state = getDeviceState(fd, type, state, sizeof(state));
			if (type != EV_SYN)
			{
				ioctl(fd, EVIOCGBIT(type, KEY_MAX), bit[type]);

				for (code = 0; code < KEY_MAX; code++)
				{
					if (test_bit(code, bit[type]))
					{
						if (have_state)
						{
							stateval = test_bit(code, state);
							//strEvents.append("    Event code %d (%s) state %d\n", code, codename(type, code), stateval);
						}
						else
						{
							//strEvents.append("    Event code %d (%s)\n", code, codename(type, code));
						}

						//if (type == EV_ABS)
						//	print_absdata(fd, code);
					}
				}
			}
		}
	}

	return result;
}

bool BaseInputDevice::getDeviceState(int fd, uint8_t type, unsigned long *array, size_t size)
{
	bool result = false;

	int rc = 0;
	switch (type)
	{
		case EV_LED:
			rc = ioctl(fd, EVIOCGLED(size), array);
			break;
		case EV_SND:
			rc = ioctl(fd, EVIOCGSND(size), array);
			break;
		case EV_SW:
			rc = ioctl(fd, EVIOCGSW(size), array);
			break;
		case EV_KEY:
			/* intentionally not printing the value for EV_KEY, let the
			 * repeat handle this */
			break;
	}

	if (rc >= 0)
	{
		result = true;
	}

	return result;
}

// ======================= Device information methods ============================

string BaseInputDevice::getInputDeviceName(int deviceIndex)
{
	// Retrieve device name using access to device:
	// '/sys/class/input/event%d/device/name'
	string query = getDeviceNameQuery();
	string nameDeviceName = tfm::format(query, deviceIndex);
	string result = readDeviceStringValue(nameDeviceName);

	// Cleaning up from non-printable characters
	result = StringHelper::cleanup(result);

	return result;
}

VIDPID BaseInputDevice::getInputDeviceVIDPID(int deviceIndex)
{
	VIDPID result;

	// Retrieve input device VID and PID using access to devices:
	// '/sys/class/input/event%d/device/id/vendor'
	// '/sys/class/input/event%d/device/id/product'
	string vidDeviceName = tfm::format(LINUX_INPUT_DEVICE_VID, deviceIndex);
	string pidDeviceName = tfm::format(LINUX_INPUT_DEVICE_PID, deviceIndex);

	result.vid = readDeviceHexValue(vidDeviceName);
	result.pid = readDeviceHexValue(pidDeviceName);

	return result;
}

string BaseInputDevice::readDeviceStringValue(const string& deviceName)
{
	string result;

	ifstream inputFile;

	inputFile.open(deviceName.c_str());
	if (inputFile.is_open())
	{
		// Read the whole line (including spaces/tabs/etc. symbols into string variable)
		// If only first word is needed then 'inputFile >> result;' will serve well
		getline(inputFile, result);

		inputFile.close();
	}

	return result;
}

uint16_t BaseInputDevice::readDeviceHexValue(const string& deviceName)
{
	uint16_t result = 0x0000;

	ifstream inputFile;

	inputFile.open(deviceName.c_str());
	if (inputFile.is_open())
	{
		inputFile >> result;

		inputFile.close();
	}

	return result;
}

// ------------------- Protected methods ---------------------------------------

// Returns formatter for the input device name
// '/sys/class/input/event%d/device/name'
const string BaseInputDevice::getDeviceNameQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE, "name");

	return result;
}

const string BaseInputDevice::getDeviceBusTypeQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "bustype");

	return result;
}

const string BaseInputDevice::getDeviceVendorQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "vendor");

	return result;
}

const string BaseInputDevice::getDeviceProductQuery()
{
	string result = tfm::format("%s%s", LINUX_INPUT_DEVICE_ID, "product");

	return result;
}

// ======================= Debug methods ============================

string BaseInputDevice::dumpEventBits(uint32_t value)
{
	string result;

	uint8_t idx = 0x1F;
	for (uint32_t i = 1 << 0x1F; i > 0; i >>= 1)
	{
		if ((value & i) && idx < eventNames.size())
		{
			if (result.size() > 0)
				result += ", ";

			result += eventNames[idx];
		}

		idx--;
	}

	if (result.size() == 0)
	{
		result = "None";
	}

	return result;
}

string BaseInputDevice::dumpLEDBits(uint16_t value)
{
	string result;

	uint8_t idx = 0x0F;
	for (uint32_t i = 1 << 0x0F; i > 0; i >>= 1)
	{
		if ((value & i) && idx < ledNames.size())
		{
			if (result.size() > 0)
				result += ", ";

			result += ledNames[idx];
		}

		idx--;
	}

	if (result.size() == 0)
	{
		result = "None";
	}

	return result;
}

string BaseInputDevice::dumpLEDType(uint16_t value)
{
	string result;

	if (value < ledNames.size())
	{
		result = string(ledNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}

string BaseInputDevice::dumpEventType(uint16_t value)
{
	string result;

	if (value < eventNames.size())
	{
		result = string(eventNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}

string BaseInputDevice::dumpSynType(uint16_t value)
{
	string result;

	if (value < synNames.size())
	{
		result = string(synNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}

string BaseInputDevice::dumpMscType(uint16_t value)
{
	string result;

	if (value < mscNames.size())
	{
		result = string(mscNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}

string BaseInputDevice::dumpKey(uint16_t value)
{
	string result;

	if (value < keyNames.size())
	{
		result = string(keyNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%04x", value);
	}

	return result;
}

string BaseInputDevice::dumpRelType(uint16_t value)
{
	string result;

	if (value < relNames.size())
	{
		result = string(relNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}

string BaseInputDevice::dumpAbsType(uint16_t value)
{
	string result;

	if (value < absNames.size())
	{
		result = string(absNames[value]);
	}
	else
	{
		result = tfm::format("UNKNOWN 0x%x", value);
	}

	return result;
}
