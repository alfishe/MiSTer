#ifndef FPGA_SOCFPGA_SDRAM_CONTROLLER_H_
#define FPGA_SOCFPGA_SDRAM_CONTROLLER_H_

#include <stdint.h>

// See more on Cyclone V HPS Memory Map documentation page: https://www.altera.com/hps/cyclone-v/hps.html

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

// fpgaportrst (0x5080) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1411577376106.html
// FPGAPORTRST (0xFFC25080), used bits [13:0]
// This register should be written to with a 1 to enable the selected FPGA port to exit reset.
// Writing a bit to a zero will stretch the port reset until the register is written.
// Bits [3:0] - Read data ports
// Bits [7:4] - Write data ports
// Bits [13:0] - Command ports
//    Bit 8 - Command port 0
//    Bit 9 - Command port 1
//    Bit 10 - Command port 2
//    Bit 11 - Command port 3
//    Bit 12 - Command port 4
//    Bit 13 - Command port 5
#define SDR_FPGAPORTRST_PORTRSTN_MASK 0x00003FFF	// Bits [13:0] - This register should be written to with a 1 to enable the selected FPGA port to exit reset. Writing a bit to a zero will stretch the port reset until the register is written. Read data ports are connected to bits 3:0, with read data port 0 at bit 0 to read data port 3 at bit 3. Write data ports 0 to 3 are mapped to 4 to 7, with write data port 0 connected to bit 4 to write data port 3 at bit 7. Command ports are connected to bits 8 to 13, with command port 0 at bit 8 to command port 5 at bit 13. Expected usage would be to set all the bits at the same time but setting some bits to a zero and others to a one is supported

#endif /* FPGA_SOCFPGA_SDRAM_CONTROLLER_H_ */
