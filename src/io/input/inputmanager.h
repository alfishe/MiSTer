#ifndef IO_INPUT_INPUTMANAGER_H_
#define IO_INPUT_INPUTMANAGER_H_

#include <list>
#include <mutex>
#include <string>
#include "../../common/events/events.h"
#include "../../common/events/messagecenter.h"
#include "inputpoll/inputpoller.h"
#include "input.h"
#include "baseinputdevice.h"

using namespace std;

class InputManager : public EventObserver
{
public:
	// Fields
	BaseInputDeviceMap m_inputDevices;

	BaseInputDeviceMap m_keyboards;
	BaseInputDeviceMap m_mouses;
	BaseInputDeviceMap m_joysticks;

	mutex m_mutexDevices;

public:
	// Singleton instance
	static InputManager& instance();
	InputManager(InputManager&&) = delete;					// Disable move constructor (C++11 feature)
	InputManager(const InputManager& that) = delete; 			// Disable copy constructor (C++11 feature)
	InputManager& operator =(InputManager const&) = delete;	// Disable assignment operator (C++11 feature)
	virtual ~InputManager();

	// Get/find methods
	BaseInputDevice* findInputDeviceByName(const string& name);

	// Control methods
	void reset();
	BaseInputDeviceMap& detectDevices();
	void startPolling();
	void stopPolling();

	bool isDeviceTypeAllowed(InputDeviceTypeEnum type);

	BaseInputDevice* resolveDevice(const string& name);

protected:
	void addInputDevice(BaseInputDevice* device);
	void removeInputDevice(const string& name);

public:
	// Debug methods
	string dumpDevicesMap();
	static string dump(BaseInputDevice* inputDevice);

// EventObserver delegates
public:
	void onMessageEvent(const EventMessageBase& event);

private:
	InputManager();	// Disable explicit object creation (only singleton instace allowed)
};

#endif /* IO_INPUT_INPUTMANAGER_H_ */
