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
	// Start message center
	MessageCenter& center = MessageCenter::defaultCenter();

	// Start input device detector
	DeviceDetector& detector = DeviceDetector::instance();
	detector.init();
	detector.start();

	// More initialization
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
