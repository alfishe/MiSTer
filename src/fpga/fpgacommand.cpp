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

	// Mutex should be unlocked prior to destroy
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);

	this->connector = nullptr;
	this->fpga = nullptr;
}

/*
 * Requests FPGA core ID
 */
uint8_t FPGACommand::getCoreType()
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

		LOGINFO("Core returned Type=0x%X\n", result);
	}
	else
	{
		LOGERROR("Loaded FPGA core is incompatible with MiSTer platform\n");
	}

	endExecution();

	return result;
}

// OSD commands
void FPGACommand::sendOSDCommand(uint8_t cmd)
{
	if (checkExecution())
	{
		connector->enableOSD();
		sendCommand(cmd);
		connector->disableOSD();

		endExecution();
	}
}

void FPGACommand::sendOSDCommand(uint8_t cmd, uint8_t param)
{
	if (checkExecution())
	{
		connector->enableOSD();
		sendCommand(cmd, param);
		connector->disableOSD();

		endExecution();
	}
}

void FPGACommand::sendOSDCommand(uint8_t cmd, uint16_t param)
{
	if (checkExecution())
	{
		connector->enableOSD();
		sendCommand(cmd, param);
		connector->disableOSD();

		endExecution();
	}
}

void FPGACommand::sendOSDCommand(uint8_t cmd, uint32_t param)
{
	if (checkExecution())
	{
		connector->enableOSD();
		sendCommand(cmd, param);
		connector->disableOSD();

		endExecution();
	}
}

// IO commands
void FPGACommand::sendIOCommand(uint8_t cmd)
{
	if (checkExecution())
	{
		connector->enableIO();
		sendCommand(cmd);
		connector->disableIO();

		endExecution();
	}
}

void FPGACommand::sendIOCommand(uint8_t cmd, uint8_t param)
{
	if (checkExecution())
	{
		connector->enableIO();
		sendCommand(cmd, param);
		connector->disableIO();

		endExecution();
	}
}

void FPGACommand::sendIOCommand(uint8_t cmd, uint16_t param)
{
	if (checkExecution())
	{
		connector->enableIO();
		sendCommand(cmd, param);
		connector->disableIO();

		endExecution();
	}
}

void FPGACommand::sendIOCommand(uint8_t cmd, uint32_t param)
{
	if (checkExecution())
	{
		connector->enableIO();
		sendCommand(cmd, param);
		connector->disableIO();

		endExecution();
	}
}

// IO commands

// Raw commands / parameter level methods
void FPGACommand::sendCommand(uint8_t cmd)
{
	send8(cmd);
}

void FPGACommand::sendCommand(uint8_t cmd, uint8_t param)
{
	send8(cmd);
	send8(param);
}

void FPGACommand::sendCommand(uint8_t cmd, uint16_t param)
{
	send8(cmd);
	send16(param);
}

void FPGACommand::sendCommand(uint8_t cmd, uint32_t param)
{
	send8(cmd);
	send32(param);
}

// Helper methods

