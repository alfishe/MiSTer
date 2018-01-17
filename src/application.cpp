#include "application.h"

#include "common/logger/logger.h"

#include "common/messagetypes.h"
#include "common/events/messagecenter.h"
#include "io/input/devicedetector/devicedetector.h"
#include "io/input/inputmanager.h"

using namespace std;

Application& Application::instance()
{
	static Application instance;

	return instance;
}

Application::~Application()
{

}

void Application::onStart()
{
	// Start input device detector
	DeviceDetector& detector = DeviceDetector::instance();
	detector.init();
	detector.start();

	// Start input manager
	InputManager& inputmgr = InputManager::instance();
	inputmgr.detectDevices();
	inputmgr.startPolling();

	// More initialization
}

void Application::onTerminate()
{
	// Other activities during app termination

	// Stop input device detector
	DeviceDetector& detector = DeviceDetector::instance();
	detector.stop();
	detector.dispose();

	// Stop polling for devices
	InputManager& inputmgr = InputManager::instance();
	inputmgr.stopPolling();
}

// TODO: Remove, test only
void Application::testEvents()
{
	MessageCenter& center = MessageCenter::defaultCenter();

	/*
	// 1. Single add / single remove
	center.addObserver("test", this);
	center.removeObserver("test", this);

	// 2. Multiple add / single remove
	center.addObserver("test", this);
	center.addObserver("test", this);
	center.addObserver("test", this);
	center.removeObserver("test", this);

	// 3. Multiple topics / single remove
	center.addObserver("test1", this);
	center.addObserver("test2", this);
	center.addObserver("test3", this);
	center.removeObserver(this);

	// 4. Random adds and clear
	center.addObserver("test1", this);
	center.addObserver("test2", this);
	center.addObserver("test2", this);
	center.addObserver("test3", this);
	center.addObserver("test3", this);
	center.addObserver("test3", this);
	center.removeObservers();
	*/


	// Full cycle scenario
	center.addObserver("test1", this);
	center.addObserver("test2", this);
	center.addObserver("test3", this);
	center.post("test", this, nullptr);

	center.post("test1", this, new DeviceStatusEvent("Message 11"));
	center.post("test1", this, new DeviceStatusEvent("Message 12"));
	center.post("test1", this, new DeviceStatusEvent("Message 13"));
	center.post("test2", this, new DeviceStatusEvent("Message 21"));
	center.post("test2", this, new DeviceStatusEvent("Message 22"));
	center.post("test2", this, new DeviceStatusEvent("Message 23"));

	sleep(10);

}

// EventObserver delegate
void Application::onMessageEvent(const EventMessageBase& event)
{
	string value;

	if (event.payload != nullptr)
	{
		value = ((DeviceStatusEvent*)event.payload)->device;
	}

	LOGINFO("%s: topic '%s', value '%s'", __PRETTY_FUNCTION__, event.topic.c_str(), value.c_str());
}
