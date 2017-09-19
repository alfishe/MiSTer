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
CoreType FPGACommand::getCoreType()
{
	CoreType result = CoreType::CORE_TYPE_UNKNOWN;

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
		result = CoreType((uint8_t)coretype);

		LOGINFO("Core returned Type=0x%X\n", result);
	}
	else
	{
		LOGERROR("Loaded FPGA core is incompatible with MiSTer platform\n");
	}

	endExecution();

	return result;
}

char* FPGACommand::getCoreName()
{
	static char result[128 + 1];

	// If parallel FPGA data transfer transaction executed - just exit
	if (!startIO())
		return nullptr;

	// Request Identification / Config string from the FPGA core
	sendCommand(UIO_GET_STRING);
	uint8_t byte = connector->transferByte(0);

	// The first char returned will be 0xFF if the core doesn't support
	// config strings. atari 800 returns 0xa4 which is the status byte
	if (!(byte == 0xff || byte == 0xA4))
	{
		unsigned idx = 0;
		result[idx++] = byte;

		while (byte != 0x00 && byte != 0xFF && idx < sizeof(result - 1))
		{
			byte = connector->transferByte(0);

			// Configuration parameters will go next. We don't need them for the name
			if (byte == ';')
			{
				result[idx] = '\0';
				break;
			}

			result[idx++] = byte;
		}

		// Ensure string is NULL-terminated
		if (result[idx] != '\0')
			result[idx] = '\0';
	}

	endIO();

	return result;
}

char* FPGACommand::getCoreConfig()
{
	static char result[128 + 1];

	// If parallel FPGA data transfer transaction executed - just exit
	if (!startIO())
		return nullptr;

	// Request Identification / Config string from the FPGA core
	sendCommand(UIO_GET_STRING);
	uint8_t byte = connector->transferByte(0);

	// The first char returned will be 0xFF if the core doesn't support
	// config strings. atari 800 returns 0xa4 which is the status byte
	if (!(byte == 0xff || byte == 0xA4))
	{
		unsigned idx = 0;

		// Skip core name and start extraction from parameters
		while (byte != 0x00 && byte != 0xFF)
		{
			if (byte == ';')
				break;

			byte = connector->transferByte(0);
		}

		// Extract config string
		while (byte != 0x00 && byte != 0xFF && idx < sizeof(result - 1))
		{
			byte = connector->transferByte(0);

			result[idx++] = byte;
		}

		// Ensure string is NULL-terminated
		if (result[idx] != '\0')
			result[idx] = '\0';
	}

	endIO();

	return result;
}

// OSD commands

/*
 * Starts OSD transaction. No other commands (any type) can be executed to interact with FPGA until transaction finished.
 */
bool FPGACommand::startOSD()
{
	bool result = false;

	if (checkExecution())
	{
		connector->enableOSD();

		result = true;
	}

	return result;
}

/*
 * Ends OSD transaction and unblocks communication with FPGA
 */
void FPGACommand::endOSD()
{
	connector->disableOSD();

	endExecution();
}

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

/*
 * Starts IO transaction. No other commands (any type) can be executed to interact with FPGA until transaction finished.
 */
bool FPGACommand::startIO()
{
	bool result = false;

	if (checkExecution())
	{
		connector->enableIO();

		result = true;
	}

	return result;
}

/*
 * Ends IO transaction and unblocks communication with FPGA
 */
void FPGACommand::endIO()
{
	connector->disableIO();

	endExecution();
}

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

