#ifndef IO_INPUT_INPUT_H_
#define IO_INPUT_INPUT_H_

#include <string>
#include <vector>
#include "../../3rdparty/betterenums/enum.h"
#include "../../common/consts.h"
#include "../../common/types.h"

using namespace std;

// Declare enum using Better-enums macro (allows to operate with enum names, indexes and size in runtime)
BETTER_ENUM(InputDeviceTypeEnum, uint8_t,
	Unknown = 0,
	Keyboard,
	Mouse,
	Joystick
)

/*
enum InputDeviceType
{
	Unknown = 0,
	Keyboard,
	Mouse,
	Joystick
};
*/

struct InputDevice
{
	// Better-enums workaround (declare proxy enum within struct/class)
	typedef InputDeviceTypeEnum InputDeviceType;

	int fd = INVALID_FILE_DESCRIPTOR;
	uint32_t eventBits;

	int index = -1;
	string path;
	VIDPID deviceID;
	InputDeviceType type = InputDeviceType::Unknown;
	string name;
};

typedef vector<InputDevice> InputDeviceVector;

// Linux device paths related to input
#define LINUX_DEVICE_INPUT "/dev/input"
#define LINUX_INPUT_DEVICE "/sys/class/input/event%d/device/"
#define LINUX_INPUT_DEVICE_ID "/sys/class/input/event%d/device/id/"
#define LINUX_INPUT_DEVICE_VID "/sys/class/input/event%d/device/id/vendor"
#define LINUX_INPUT_DEVICE_PID "/sys/class/input/event%d/device/id/product"

#define REGEX_INPUT_DEVICE_INDEX "event(\\d+)"

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
