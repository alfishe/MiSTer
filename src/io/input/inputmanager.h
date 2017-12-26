#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>
#include "input.h"

using namespace std;

class InputManager
{
protected:
	// Fields
	InputDeviceVector inputDevices;

	list<int> keyboards;
	list<int> mouses;
	list<int> joysticks;

public:
	// Singleton instance
	static InputManager& instance();

	InputManager(const InputManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~InputManager() {};

	InputDeviceVector detectDevices();

	// Debug methods
	static string dump(InputDeviceVector& inputDevices);

private:
	InputManager() {}; // Disallow direct instances creation
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
