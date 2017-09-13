/*
* Copyright (C) 2012 Altera Corporation <www.altera.com>
* All rights reserved.
*
* SPDX-License-Identifier:    BSD-3-Clause
*/

#ifndef FPGA_SOCFPGA_FPGA_MANAGER_H_
#define FPGA_SOCFPGA_FPGA_MANAGER_H_

#include <stdint.h>

// Module instance: fpgamgrregs
// Base address: 0xFF706000 (SOCFPGA_FPGAMGRREGS_ADDRESS)
struct socfpga_fpga_manager
{
	// FPGA Manager Module Registers
	uint32_t	 stat;				// Offset: 0x00 - RW - status register
	uint32_t	 ctrl;				// Offset: 0x04 - RW - control register
	uint32_t dclkcnt;			// Offset: 0x08 - RW - DCLK count register
	uint32_t	 dclkstat;			// Offset: 0x0C - RW - DCLK status register
	uint32_t	 gpo;				// Offset: 0x10 - RW - general-purpose output register
	uint32_t	 gpi;				// Offset: 0x14 - RO - general-purpose input register
	uint32_t	 misci;				// Offset: 0x18 - RO - miscellaneous input register
	// END FPGA Manager Module Registers

	uint32_t	 _pad_0x1c_0x82c[517];

	// Configuration Monitor (MON) Registers
	uint32_t	 gpio_inten;			// Offset: 0x830 - RW - interrupt enable register
	uint32_t	 gpio_intmask;		// Offset: 0x834 - RW - interrupt mask register
	uint32_t	 gpio_inttype_level;	// Offset: 0x838 - RW - interrupt level register
	uint32_t	 gpio_int_polarity;	// Offset: 0x83C - RW - interrupt polarity register
	uint32_t	 gpio_intstatus;		// Offset: 0x840 - RO - interrupt status register
	uint32_t	 gpio_raw_intstatus;	// Offset: 0x844 - RO - raw interrupts status register
	uint32_t	 _pad_0x848;
	uint32_t	 gpio_porta_eoi;		// Offset: 0x84C - WO - clear interrupt register
	uint32_t	 gpio_ext_porta;		// Offset: 0x850 - RO - external Port A register
	uint32_t	 _pad_0x854_0x85c[3];
	uint32_t	 gpio_1s_sync;		// Offset: 0x860 - RW - synchronization level register
	uint32_t	 _pad_0x864_0x868[2];
	uint32_t	 gpio_ver_id_code;	// OffsetL 0x86C - RO - GPIO version register
	uint32_t	 gpio_config_reg2;	// Offset: 0x870 - RO - configuration register 2
	uint32_t	 gpio_config_reg1;	// Offset: 0x874 - RO - configuration register 1
	// END Configuration Monitor (MON) Registers
};

// stat (0x000) Fields and masks
// Cyclone V handbook, Section 4-13
#define FPGAMGRREGS_STAT_MODE_FPGAOFF		0x00		// FPGA Powered Off
#define FPGAMGRREGS_STAT_MODE_RESETPHASE		0x01		// FPGA in Reset Phase
#define FPGAMGRREGS_STAT_MODE_CFGPHASE		0x02		// FPGA in Configuration Phase
#define FPGAMGRREGS_STAT_MODE_INITPHASE		0x03		// FPGA in Initialization Phase. In CVP configuration, this state indicates IO configuration has completed
#define FPGAMGRREGS_STAT_MODE_USERMODE		0x04		// FPGA in User Mode
#define FPGAMGRREGS_STAT_MODE_UNKNOWN		0x05		// FPGA state has not yet been determined. This only occurs briefly after reset

// MSEL has 13 mode documented
// 4 - 16-bit Passive Parallel with Fast Power
// 4 - 16 bit Passive Parallel with Slow Power
// 5 - 32-bit Passive Parallel with Fast Power
//
// TODO: define them

#define FPGAMGRREGS_STAT_MODE_BIT0			0x01
#define FPGAMGRREGS_STAT_MODE_BIT1			0x02
#define FPGAMGRREGS_STAT_MODE_BIT2			0x04
#define FPGAMGRREGS_STAT_MSEL_BIT0			0x08
#define FPGAMGRREGS_STAT_MSEL_BIT1			0x10
#define FPGAMGRREGS_STAT_MSEL_BIT2			0x20
#define FPGAMGRREGS_STAT_MSEL_BIT3			0x40
#define FPGAMGRREGS_STAT_MSEL_BIT4			0x80

#define FPGAMGRREGS_STAT_MODE_MASK			0x07		// bits [2:0]
#define FPGAMGRREGS_STAT_MSEL_MASK			0xF8		// bits [7:3]
#define FPGAMGRREGS_STAT_MSEL_LSB			0x03		// bits [4:3] - only 16-bit related flags

// END stat (0x000) Fields and masks

#define FPGAMGRREGS_CTRL_CFGWDTH_MASK		0x200
#define FPGAMGRREGS_CTRL_AXICFGEN_MASK		0x100
#define FPGAMGRREGS_CTRL_NCONFIGPULL_MASK	0x4
#define FPGAMGRREGS_CTRL_NCE_MASK			0x2
#define FPGAMGRREGS_CTRL_EN_MASK				0x1
#define FPGAMGRREGS_CTRL_CDRATIO_LSB			6

// gpio_ext_porta (0x850) Fields and masks (RO)
// Cyclone V handbook, Section 4-42
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_NS 	0x01		// Value of nSTATUS
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CD 	0x02		// Value of CONF_DONE
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_ID	0x04		// Value of INIT_DONE
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CRC	0x08		// Value of CRC_ERROR
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CCD	0x10		// Value of CVP_CONF_DONE
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_PRR	0x20		// Value of PR_READY
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_PRE	0x40		// Value of PR_ERROR
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_PRD	0x80		// Value of PR_DONE
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_NCP	0x100	// Value of nCONFIG Pin
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_NSP	0x200	// Value of nSTATUS Pin
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CDP	0x400	// Value of CONF_DONE Pin
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_FPO	0x800	// Value of FPGA_POWER_ON

#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CRC_MASK	0x8	// CRC_ERROR mask
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_ID_MASK	0x4	// INIT_DONE mask
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CD_MASK	0x2	// CONF_DONE mask
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_NS_MASK	0x1	// nSTATUS mask
// END gpio_ext_porta (0x850) Fields and masks (RO)

// dclkstat (0x00C) Fields and masks (RW)
#define FPGAMGRREGS_DCLKSTAT_DCNTDONE			0x01	 // dcntdone - set to clear. Set after DCLKCNT transitioned down from 1 to 0
// END dclkstat (0x00C) Fields and masks (RW)

// FPGA CD Ratio Value
#define CDRATIO_x1				0x0
#define CDRATIO_x2				0x1
#define CDRATIO_x4				0x2
#define CDRATIO_x8				0x3

#endif /* FPGA_SOCFPGA_FPGA_MANAGER_H_ */
