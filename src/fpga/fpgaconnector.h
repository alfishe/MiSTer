#ifndef FPGA_FPGACONNECTOR_H_
#define FPGA_FPGACONNECTOR_H_

#include <stdint.h>
#include "fpgadevice.h"

#define SSPI_STROBE  (1<<17)
#define SSPI_ACK     SSPI_STROBE

#define SSPI_FPGA_EN (1<<18)
#define SSPI_OSD_EN  (1<<19)
#define SSPI_IO_EN   (1<<20)
#define SSPI_DM_EN   (1<<21)

#define SWAPW(a) ((((a)<<8)&0xff00)|(((a)>>8)&0x00ff))

/*
 * Provides HPS<-> FPGA data transfer via 32-bit gpo and gpi registers
 */
class FPGAConnector
{
private:
	// Fields
	FPGADevice *fpga;

public:
	FPGAConnector(FPGADevice *fpga);
	virtual ~FPGAConnector();

	// Drive control flags methods
	void enableFPGA();
	void disableFPGA();

	void enableOSD();
	void disableOSD();

	void enableIO();
	void disableIO();

	void enableDMode();
	void disableDMode();

	// Low-level data transfer methods
	void read(uint8_t *addr, uint16_t len, bool use16bit);
	void write(uint8_t *addr, uint16_t len, bool use16bit);
	uint8_t transferByte(uint8_t byte);
	uint16_t transferWord(uint16_t word);

protected:
	// Helper methods
	inline void enableByMask(uint32_t mask) __attribute__((always_inline))
	{
		uint32_t gpo = fpga->gpo_read() | 0x80000000;
		fpga->gpo_write(gpo | mask);
	}

	inline void disableByMask(uint32_t mask) __attribute__((always_inline))
	{
		uint32_t gpo = fpga->gpo_read() | 0x80000000;
		fpga->gpo_write(gpo & ~mask);
	}
};

#endif /* FPGA_FPGACONNECTOR_H_ */
