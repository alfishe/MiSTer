#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>
#include "../../common/events/events.h"
#include "../../common/events/messagecenter.h"
#include "input.h"

using namespace std;

class InputManager : public EventObserver
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
	InputManager(InputManager&&) = delete;			// Move constructor disabled (C++11 feature)
	InputManager(const InputManager& that) = delete; // Disable copy constructor (C++11 feature)
	InputManager& operator =(InputManager const&) = delete;		// Assignment disabled (C++11 feature)
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
	InputManager(); // Disallow direct instances creation

// EventObserver delegates
public:
	void onMessageEvent(MessageEvent event);
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
