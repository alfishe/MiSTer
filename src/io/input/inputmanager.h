#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

class InputManager
{
public:
	InputManager();
	InputManager(const InputManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~InputManager();
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
