#include "fpgaconnector.h"

#include "../common/logger/logger.h"

#include "fpgadevice.h"

FPGAConnector::FPGAConnector(FPGADevice *fpga)
{
	this->fpga = fpga;
}

FPGAConnector::~FPGAConnector()
{
}

void FPGAConnector::enableFPGA()
{
	enableByMask(SSPI_FPGA_EN);
}

void FPGAConnector::disableFPGA()
{
	disableByMask(SSPI_FPGA_EN);
}

void FPGAConnector::enableOSD()
{
	enableByMask(SSPI_OSD_EN);
}

void FPGAConnector::disableOSD()
{
	disableByMask(SSPI_OSD_EN);
}

void FPGAConnector::enableIO()
{
	enableByMask(SSPI_IO_EN);
}

void FPGAConnector::disableIO()
{
	disableByMask(SSPI_IO_EN);
}

void FPGAConnector::enableDMode()
{
	enableByMask(SSPI_DM_EN);
}

void FPGAConnector::disableDMode()
{
	disableByMask(SSPI_DM_EN);
}

// Low-level data transfer methods
void FPGAConnector::read(uint8_t *addr, uint16_t len, bool use16bit)
{
	// Each read byte requires fake write beforehand
	// So initiating zero writes we're getting data from FPGA in return

	if (use16bit)
	{
		uint16_t *addr16bit = (uint16_t*)addr;
		uint16_t len16bit = len >> 1;
		bool oddLen = len & 1;

		// Transfer even number of bytes by word transfers
		while (len16bit--)
			*(addr16bit++) = transferWord(0);

		// Last odd byte transferred separately if needed
		if (oddLen)
			*((uint8_t*)addr16bit) = transferByte(0);
	}
	else
	{
		while (len--)
			*(addr++) = transferByte(0);
	}
}

void FPGAConnector::write(uint8_t *addr, uint16_t len, bool use16bit)
{
	if (use16bit)
	{
		uint16_t *addr16bit = (uint16_t*)addr;
		uint16_t len16bit = len >> 1;
		bool oddLen = len & 1;

		// Transfer even number of bytes by word transfers
		while (len16bit--)
			transferWord(*addr16bit++);

		// Last odd byte transferred separately if needed
		if (oddLen)
			transferByte(*((uint8_t*)addr16bit));
	}
	else
	{
		while (len--)
			transferByte(*addr++);
	}
}

/*
 * Since gpo/gpi have 16 data bits - reuse transferWord logic
 */
uint8_t FPGAConnector::transferByte(uint8_t byte)
{
	uint8_t result = transferWord((uint16_t)byte);

	return result;
}

// TODO: investigate if timeouts needed
uint16_t FPGAConnector::transferWord(uint16_t word)
{
	uint16_t result = 0;

	// Read current gpo value, put 16 bit value into data block (lower 16 bits) and reset STROBE bit in control block (upper 16 bits)
	uint32_t gpo = (fpga->gpo_read() & ~(0xFFFF | SSPI_STROBE)) | word;

	// Step 1: reset strobe bit for FPGA
	fpga->gpo_write(gpo);

	// Step 2: set strobe bit (positive edge indicates data transfer start for FPGA)
	fpga->gpo_write(gpo | SSPI_STROBE);

	// Step 3: poll gpi until ACK bit set
	do
	{
		result = fpga->gpi_read();
		if (result < 0)
		{
			LOGERROR("GPI[31]==1. FPGA is uninitialized?\n");
			result = 0;
			break;
		}
	}
	while (!(result & SSPI_ACK));

	return result;
}

