#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>

using namespace std;

class InputManager
{
protected:
	// Fields
	list<int> keyboards;
	list<int> mouses;
	list<int> joysticks;

public:
	static InputManager& instance();
	InputManager(const InputManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~InputManager();

	void detectDevices();

private:
	InputManager() {}; // Disallow direct instances creation
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
