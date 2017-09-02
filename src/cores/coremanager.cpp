#include "coremanager.h"

#include "../common/logger/logger.h"

#include <limits.h>
#include <stdio.h>
#include "../common/consts.h"
#include "../fpga/fpgadevice.h"

CoreManager& CoreManager::instance()
{
	static CoreManager instance;

	return instance;
}

bool CoreManager::loadCore(const char* filename)
{
	bool result = false;

	FPGADevice& device = FPGADevice::instance();

	// Load core bitstream into FPGA from file
	if (device.load_rbf(filename))
	{
		// Determine core type

		// Instantiate correspondent

		result = true;

		LOGINFO("Core %s successfully loaded\n", filename);
	}
	else
	{
		LOGERROR("Unable to load core file: %s\n", filename);
	}

	return result;
}
