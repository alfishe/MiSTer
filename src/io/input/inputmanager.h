#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>
#include "input.h"

using namespace std;

class InputManager
{
public:
	// Fields
	InputDeviceVector inputDevices;

	InputDeviceVector keyboards;
	InputDeviceVector mouses;
	InputDeviceVector joysticks;

public:
	// Singleton instance
	static InputManager& instance();

	InputManager(const InputManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~InputManager();

	void reset();
	InputDeviceVector& detectDevices();
	bool isDeviceTypeAllowed(InputDeviceTypeEnum type);

protected:
	void addInputDevice(InputDevice& device);

public:
	// Debug methods
	static string dump(InputDeviceVector& inputDevices);

private:
	InputManager() {}; // Disallow direct instances creation
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
