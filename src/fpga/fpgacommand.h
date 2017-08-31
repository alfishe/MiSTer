#ifndef FPGA_FPGACOMMAND_H_
#define FPGA_FPGACOMMAND_H_

#include "../common/logger/logger.h"

#include <stdint.h>
#include <pthread.h>

#include "fpgadevice.h"
#include "fpgaconnector.h"

// Forward declaration. Header included from fpgacommand.cpp
class FPGAConnector;
class FPGADevice;

class FPGACommand
{
protected:
	// Physical transfers done using FPGAConnector and FPGADevice
	FPGAConnector *connector = nullptr;
	FPGADevice *fpga = nullptr;

	// Ensure that command scope untouched during the whole execution with mutex guard
	pthread_mutex_t mutex;

public:
	FPGACommand(FPGAConnector *connector);
	FPGACommand(const FPGACommand& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~FPGACommand();

	uint8_t getCoreID();

protected:
	// Helper methods
	__inline bool checkExecution()
	{
		bool result = true;

		if (connector == nullptr || fpga == nullptr || !fpga->isInitialized)
		{
			LOGERROR("FPGA device/connector infrastructure was not not initialized properly\n");

			result = false;
		}

		return result;
	}
};

#endif /* FPGA_FPGACOMMAND_H_ */
