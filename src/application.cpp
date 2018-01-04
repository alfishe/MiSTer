#include "application.h"

#include "common/logger/logger.h"
#include "common/events/messagecenter.h"
#include "io/input/devicedetector.h"

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

	// More initialization

	// TODO: test only
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
	LOGINFO("%s", __PRETTY_FUNCTION__);
}
