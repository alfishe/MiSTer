#include "coremanager.h"

#include "../common/logger/logger.h"

#include "../fpga/fpgadevice.h"

CoreManager& CoreManager::instance()
{
	static CoreManager instance;

	return instance;
}

bool CoreManager::loadCore(char* filename)
{
	bool result = false;

	FPGADevice device = FPGADevice::instance();

	// Load core bitstream into FPGA from file
	if (device.load_rbf(filename))
	{
		// Determine core type

		result = true;

		LOGINFO("Core %s successfully loaded", filename);
	}
	else
	{
		LOGERROR("Unable to load core file: %s", filename);
	}

	return result;
}
