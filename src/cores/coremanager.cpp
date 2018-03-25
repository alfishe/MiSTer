#include "coremanager.h"

#include "../common/logger/logger.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../common/consts.h"
#include "../common/messagetypes.h"
#include "../common/events/messagecenter.h"
#include "../fpga/fpgadevice.h"
#include "../fpga/fpgacommand.h"

CoreManager& CoreManager::instance()
{
	static CoreManager instance;

	return instance;
}

bool CoreManager::loadCore(const string& filename)
{
	bool result = false;

	FPGADevice& device = FPGADevice::instance();

	// Load core bitstream into FPGA from file
	if (device.load_rbf(filename))
	{
		// Determine core type
		FPGACommand& command = *device.command;
		CoreType coreType = command.getCoreType();
		LOGINFO("Core name: %s", command.getCoreName().c_str());
		LOGINFO("Core config: %s", command.getCoreConfig().c_str());
		LOGINFO("%s", command.getVideoMode().c_str());

		// Instantiate correspondent adapter(s) in ARM code

		// Notify that core was started successfully
		MessageCenter& center = MessageCenter::instance();
		MessagePayloadBase* payload = new CoreStartedEvent(filename); // Allocating here. Will be destroyed automatically by Message Center queue
		center.post(EVENT_CORE_STARTED, this, payload);

		result = true;

		LOGINFO("Core %s successfully loaded", filename.c_str());
	}
	else
	{
		LOGERROR("Unable to load core file: %s", filename.c_str());
	}

	return result;
}

CoreType CoreManager::getCoreType()
{
	FPGADevice& device = FPGADevice::instance();
	FPGACommand& command = *device.command;

	CoreType result = command.getCoreType();

	return result;
}

const string CoreManager::getCoreName()
{
	FPGADevice& device = FPGADevice::instance();
	FPGACommand& command = *device.command;

	string result(command.getCoreName());

	return result;
}

bool CoreManager::isMenuCore()
{
	bool result = false;

	if (strncmp(getCoreName().c_str(), "MENU", 4) == 0)
	{
		result = true;
	}

	return result;
}

ICoreInterface* CoreManager::getCurrentCore()
{
	if (currentCore == nullptr)
	{
		CoreType type = getCoreType();
		const string name = getCoreName();
	}

	return currentCore;
}
