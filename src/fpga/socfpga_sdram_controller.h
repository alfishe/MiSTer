#ifndef FPGA_SOCFPGA_SDRAM_CONTROLLER_H_
#define FPGA_SOCFPGA_SDRAM_CONTROLLER_H_

#include <stdint.h>

// Module instance: sdr
// Base address: 0xFFC20000 (SOCFPGA_SDR_ADDRESS)
// End address:  0xFFC3FFFF
// Size:			 0x00020000
struct socfpga_sdram_controller
{
	uint32_t	__pad_0x0000_0x4FFF[0x1400];

	uint32_t ctrlcfg;			// 0x5000
	uint32_t dramtiming1;		// 0x5004
	uint32_t dramtiming2;		// 0x5008
	uint32_t dramtiming3;		// 0x500C
	uint32_t dramtiming4;		// 0x5010
	uint32_t lowpwrtiming;		// 0x5014
	uint32_t dramodt;			// 0x5018

	uint32_t	__pad_0x5020_0x502B[3];

	uint32_t dramaddrw;			// 0x502C
	uint32_t dramifwidth;		// 0x5030

	uint32_t __pad_0x5034_0x5037;

	uint32_t dramsts;			// 0x5038
	uint32_t dramintr;			// 0x503C
	uint32_t sbecount;			// 0x5040
	uint32_t dbecount;			// 0x5044
	uint32_t erraddr;			// 0x5048
	uint32_t dropcount;			// 0x504C
	uint32_t	 dropaddr;			// 0x5050
	uint32_t lowpwreq;			// 0x5054
	uint32_t lowpwrack;			// 0x5058
	uint32_t staticcfg;			// 0x505C
	uint32_t ctrlwidth;			// 0x5060

	uint32_t __pad_0x5064_0x507B[6];

	uint32_t portcfg;			// 0x507C
	uint32_t fpgaportrst;		// 0x5080

	uint32_t __pad_0x5084_0x508B[2];

	uint32_t protportdefault;	// 0x508C
	uint32_t protruleaddr;		// 0x5090
	uint32_t protruleid;			// 0x5094
	uint32_t protruledata;		// 0x5098
	uint32_t protrulerdwr;		// 0x509C

	uint32_t __pad_0x50A0_0x50AB[3];

	uint32_t mppriority;			// 0x50AC

	uint32_t mpweight_0_4;		// 0x50B0
	uint32_t mpweight_1_4;		// 0x50B4
	uint32_t mpweight_2_4;		// 0x50B8
	uint32_t mpweight_3_4;		// 0x50BC

	uint32_t __pad_0x50C0_0x50BF[8];

	uint32_t remappriority;		// 0x50E0
};

#endif /* FPGA_SOCFPGA_SDRAM_CONTROLLER_H_ */
