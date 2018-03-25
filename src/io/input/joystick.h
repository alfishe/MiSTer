#ifndef IO_INPUT_JOYSTICK_H_
#define IO_INPUT_JOYSTICK_H_
#include <string>
#include "baseinputdevice.h"

using namespace std;

class Joystick: public BaseInputDevice
{
public:
	Joystick(const string& name, const string& path);
	virtual ~Joystick() {};
};

#endif /* IO_INPUT_JOYSTICK_H_ */
