#ifndef IO_INPUT_MOUSE_H_
#define IO_INPUT_MOUSE_H_

#include <string>
#include "baseinputdevice.h"

using namespace std;

class Mouse : public BaseInputDevice
{
public:
	Mouse(const string& name, const string& path);
	virtual ~Mouse() {};
};

#endif /* IO_INPUT_MOUSE_H_ */
