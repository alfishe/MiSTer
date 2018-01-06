#include "application.h"

#include "common/logger/logger.h"
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
	// Start message center (needs to be started as earlier as possible)
	MessageCenter& center = MessageCenter::defaultCenter();

	// Start input device detector
	DeviceDetector& detector = DeviceDetector::instance();
	detector.init();
	detector.start();

	// Start input manager
	InputManager& inputmgr = InputManager::instance();
	inputmgr.detectDevices();

	// More initialization

	// TODO: test only
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

	/*
	// Full cycle scenario
	center.addObserver("test1", this);
	center.addObserver("test2", this);
	center.addObserver("test3", this);
	center.post("test", this, nullptr);
	center.post("test1", this, (void *)"Message 11");
	center.post("test1", this, (void *)"Message 12");
	center.post("test1", this, (void *)"Message 13");
	center.post("test2", this, (void *)"Message 21");
	center.post("test2", this, (void *)"Message 22");
	center.post("test2", this, (void *)"Message 23");

	sleep(10);
	*/

	// -test only
}

void Application::onTerminate()
{
	// Other activities during app termination

	// Stop input device detector
	DeviceDetector& detector = DeviceDetector::instance();
	detector.stop();
	detector.dispose();

	// Dispose message center resources
	MessageCenter& center = MessageCenter::defaultCenter();
	center.dispose();
}

void Application::onMessageEvent(MessageEvent event)
{
	LOGINFO("%s: topic '%s' msg '%s'", __PRETTY_FUNCTION__, event.name.c_str(), event.param);
}
