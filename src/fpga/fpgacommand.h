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
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
	FPGACommand(FPGAConnector *connector);
	FPGACommand(const FPGACommand& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~FPGACommand();

	uint8_t getCoreType();

	// OSD commands
	bool startOSD();
	void endOSD();
	void sendOSDCommand(uint8_t cmd);
	void sendOSDCommand(uint8_t cmd, uint8_t param);
	void sendOSDCommand(uint8_t cmd, uint16_t param);
	void sendOSDCommand(uint8_t cmd, uint32_t param);

	// IO commands
	void sendIOCommand(uint8_t cmd);
	void sendIOCommand(uint8_t cmd, uint8_t param);
	void sendIOCommand(uint8_t cmd, uint16_t param);
	void sendIOCommand(uint8_t cmd, uint32_t param);

	// Raw commands / parameter level methods
	void sendCommand(uint8_t cmd);
	void sendCommand(uint8_t cmd, uint8_t param);
	void sendCommand(uint8_t cmd, uint16_t param);
	void sendCommand(uint8_t cmd, uint32_t param);

protected:
	// Helper methods
	__inline bool checkExecution() __attribute__((always_inline))
	{
		bool result = true;

		if (connector == nullptr || fpga == nullptr || !fpga->isInitialized)
		{
			LOGERROR("FPGA device/connector infrastructure was not not initialized properly\n");

			result = false;
		}
		else
		{
			if (pthread_mutex_trylock(&mutex) != 0)
			{
				LOGERROR("Unable to execute FPGA command when other command is still in progress\n");

				result = false;
			}
		}

		return result;
	}

	__inline void endExecution()  __attribute__((always_inline))
	{
		pthread_mutex_unlock(&mutex);
	}

	__inline void send8(uint8_t value) __attribute__((always_inline))
	{
		connector->transferByte(value);
	}

	/*
	 * Send 2 bytes value (MSB)
	 */
	__inline void send16(uint16_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}

	/*
	 * Send 3 bytes value (MSB)
	 */
	__inline void send24(uint32_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 16);
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}

	/*
	 * Send 4 bytes value (MSB)
	 */
	__inline void send32(uint32_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 24);
		connector->transferByte(value >> 16);
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}
};

#endif /* FPGA_FPGACOMMAND_H_ */
