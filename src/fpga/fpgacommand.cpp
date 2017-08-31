#include "fpgacommand.h"

#include "../common/logger/logger.h"

#include "fpgaconnector.h"
#include "fpgadevice.h"

FPGACommand::FPGACommand(FPGAConnector *connector)
{
	TRACE("FPGACommand(FPGAConnector *connector)");

	this->connector = connector;
	this->fpga = connector->fpga;
}

FPGACommand::~FPGACommand()
{
	TRACE("~FPGACommand()");

	this->connector = nullptr;
	this->fpga = nullptr;
}

/*
 * Requests FPGA core ID
 */
uint8_t FPGACommand::getCoreID()
{
	uint8_t result = 0;

	// Each command should have this check as a first call
	if (!checkExecution())
		return result;

	// Reset gpo[31] - that triggers returning magic number (0x5CA623) from FPGA side in gpi[31:8] and core ID in gpi[7:0]
	uint32_t gpo = (fpga->gpo_read() & 0x7FFFFFFF);
	fpga->gpo_write(gpo);

	// Retrieve value from gpi
	uint32_t coretype = fpga->gpi_read();
	gpo |= 0x80000000;
	fpga->gpo_write(gpo);

	if ((coretype >> 8) == MISTER_CORE_MAGIC_NUMBER)
	{
		result = (uint8_t)coretype;

		LOGINFO("Core returned ID=0x%X\n", result);
	}
	else
	{
		LOGERROR("Loaded FPGA core is incompatible with MiSTer platform\n");
	}

	return result;
}
