#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>
#include <mutex>
#include <string>
#include "../../common/events/events.h"
#include "../../common/events/messagecenter.h"
#include "inputpoll/inputpoller.h"
#include "input.h"

using namespace std;

class InputManager : public EventObserver
{
public:
	// Fields
	InputDeviceMap m_inputDevices;

	InputDeviceMap m_keyboards;
	InputDeviceMap m_mouses;
	InputDeviceMap m_joysticks;

	mutex m_mutexDevices;

public:
	// Singleton instance
	static InputManager& instance();
	InputManager(InputManager&&) = delete;					// Disable move constructor (C++11 feature)
	InputManager(const InputManager& that) = delete; 			// Disable copy constructor (C++11 feature)
	InputManager& operator =(InputManager const&) = delete;	// Disable assignment operator (C++11 feature)
	virtual ~InputManager();

	void reset();
	InputDeviceMap& detectDevices();
	void startPolling();
	void stopPolling();

	bool isDeviceTypeAllowed(InputDeviceTypeEnum type);

	bool resolveDevice(const string& name, InputDevice& inputDevice);

protected:
	void addInputDevice(InputDevice& device);
	void removeInputDevice(const string& name);

public:
	// Debug methods
	string dumpDevicesMap();
	static string dump(InputDevice& inputDevice);

private:
	InputManager(); // Disallow direct instances creation

// EventObserver delegates
public:
	void onMessageEvent(const MessageEvent& event);
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
