#ifndef FPGA_SOCFPGA_SYSTEM_MANAGER_H_
#define FPGA_SOCFPGA_SYSTEM_MANAGER_H_

#include <stdint.h>

// System mamager
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067999349.html

// Base address: 0xFFD08000 (SYSMGR)
// End address:  0xFFD0C000
// Size:			 0x00004000
struct socfpga_system_manager
{
	// System Manager Module
	uint32_t	 siliconid1;				// Offset: 0x00 - RO - Silicon ID1 Register
	uint32_t	 siliconid2;				// Offset: 0x04 - RO - Silicon ID2 Register

	uint32_t	 _pad_0x08_0x0f[2];

	uint32_t	 wddbg;					// Offset: 0x10 - RW - L4 Watchdog Debug Register
	uint32_t	 bootinfo;				// Offset: 0x14 - RO - Boot Info Register
	uint32_t	 hpsinfo;				// Offset: 0x18 - RO - HPS Info Register
	uint32_t	 parityinj;				// Offset: 0x1C - RW - Parity Fail Injection Register

	// FPGA Interface Group
	uint32_t	 fpgaintf_gbl;			// Offset: 0x20 - RW - Global Disable Register
	uint32_t	 fpgaintf_indiv;			// Offset: 0x24 - RW - Individual Disable Register
	uint32_t	 fpgaintf_module;		// Offset: 0x28 - RW - Module Disable Register

	uint32_t	 _pad_0x2c_0x2f;

	// Scan Manager Group
	uint32_t	 scanmgr_ctrl;			// Offset: 0x30 - RW - Scan Manager Control Register

	uint32_t	 _pad_0x34_0x3f[3];

	// Freeze Control Group
	uint32_t	 vioctrl;				// Offset: 0x40 - RW - VIO Control Register

	uint32_t	 _pad_0x44_0x4f[3];

	uint32_t frzctrl_hioctrl;		// Offset: 0x50 - RW - HIO Control Register
	uint32_t	 frzctrl_src;			// Offset: 0x54 - RW - Source Register
	uint32_t	 frzctrl_hwctrl;			// Offset: 0x58 - RW - Hardware Control Register

	uint32_t	 _pad_0x5c_0x5f;

	// EMAC Group
	uint32_t emac_ctrl;				// Offset: 0x60 - RW - Control Register
	uint32_t	 emac_l3master;			// Offset: 0x64 - RW - EMAC L3 Master AxCACHE Register

	uint32_t	 _pad_0x68_0x6f[2];

	// DMA Controller Group
	uint32_t	 dma_ctrl;				// Offset: 0x70 - RW - Control Register
	uint32_t	 dma_persecurity;		// Offset: 0x74 - RW - Peripheral Security Register

	uint32_t	 _pad_0x78_0x7f[2];

	// Preloader (initial software) Group
	uint32_t	 iswgrp_handoff[8];		// Offset: 0x80 - RW - Preloader to OS Handoff Information

	uint32_t	 _pad_0xa0_0xbf[8];

	// Boot ROM Code Register Group
	uint32_t	 romcode_ctrl;			// Offset: 0xC0 - RW - Control Register
	uint32_t	 romcode_cpu1startaddr;	// Offset: 0xC4 - RW - CPU1 Start Address Register
	uint32_t	 romcode_initswstate;	// Offset: 0xC8 - RW - Preloader (initial software) State Register
	uint32_t	 romcode_initswlastld;	// Offset: 0xCC - RW - Preloader (initial software) Last Image Loaded Register
	uint32_t	 romcode_bootromswstate;	// Offset: 0xD0 - RW - Boot ROM Software State Register

	uint32_t		_pad_0xd4_0xdf[3];

	// Warm Boot from On-Chip RAM Group
	uint32_t	 warmram_enable;			// Offset: 0xE0 - RW - Enable Register
	uint32_t	 warmram_datastart;		// Offset: 0xE4 - RW - Data Start Register
	uint32_t	 warmram_length;			// Offset: 0xE8 - RW - Length Register
	uint32_t	 warmram_execution;		// Offset: 0xEC - RW - Execution Register
	uint32_t	 warmram_crc;			// Offset: 0xF0 - RW - Expected CRC Register

	uint32_t	 _pad_0xf4_0xff[3];

	// Boot ROM Hardware Register Group
	uint32_t	 romhw_ctrl;				// Offset: 0x100 - RW - Boot ROM Hardware Control Register

	uint32_t	 _pad_0x104_0x107;

	// SDMMC Controller Group
	uint32_t	 sdmmc_ctrl;				// Offset: 0x108 - RW - Control Register
	uint32_t	 sdmmc_l3master;			// Offset: 0x10C - RW - SD/MMC L3 Master HPROT Register

	// NAND Flash Controller Register Group
	uint32_t	 nand_bootstrap;			// Offset: 0x110 - RW - Bootstrap Control Register
	uint32_t	 nand_l3master;			// Offset: 0x114 - RW - NAND L3 Master AxCACHE Register

	// USB Controller Group
	uint32_t		usb_l3master;		// Offset: 0x118 - RW - USB L3 Master HPROT Register

	uint32_t		_pad_0x11c_0x13f[9];

	// ECC Management Register Group
	uint32_t		ecc_l2;				// Offset: 0x140 - RW - L2 Data RAM ECC Enable Register
	uint32_t		ecc_ocram;			// Offset: 0x144 - RW - On-chip RAM ECC Enable Register
	uint32_t		ecc_usb0;			// Offset: 0x148 - RW - USB0 RAM ECC Enable Register
	uint32_t		ecc_usb1;			// Offset: 0x14C - RW - USB1 RAM ECC Enable Register
	uint32_t		ecc_emac0;			// Offset: 0x150 - RW - EMAC0 RAM ECC Enable Register
	uint32_t		ecc_emac1;			// Offset: 0x154 - RW - EMAC1 RAM ECC Enable Register
	uint32_t		ecc_dma;				// Offset: 0x158 - RW - DMA RAM ECC Enable Register
	uint32_t		ecc_can0;			// Offset: 0x15C - RW - CAN0 RAM ECC Enable Register
	uint32_t		ecc_can1;			// Offset: 0x160 - RW - CAN1 RAM ECC Enable Register
	uint32_t		ecc_nand;			// Offset: 0x164 - RW - NAND RAM ECC Enable Register
	uint32_t		ecc_qspi;			// Offset: 0x168 - RW - QSPI RAM ECC Enable Register
	uint32_t		ecc_sdmmc;			// Offset: 0x16C - RW - SDMMC RAM ECC Enable Register

	uint32_t		_pad_0x170_0x3ff[164];

	// Pin Mux Control Group
	uint32_t		emacio[20];			// Offset: 0x400 - RW - EMAC mux selection registers
	uint32_t		flashio[12];			// Offset: 0x450 - RW - SDMMC mux selection registers
	uint32_t		generalio[28];		// Offset: 0x480 - RW - General IO mux selection registers

	uint32_t		_pad_0x4f0_0x4ff[4];

	uint32_t		mixed1io[22];		// Offset: 0x500 - RW - Mixed IO 1 mux selection registers
	uint32_t		mixed2io[8];			// Offset: 0x558 - RW - Mixed IO 2 mux selection registers
	uint32_t		gplinmux[23];		// Offset: 0x570 - RW - GPIO / LoanIO input mux selection registers
	uint32_t		gplmux[71];			// Offset: 0x5D4 - RW - GPIO / LoanIO output mux selection registers
	uint32_t		nandusefpga;			// Offset: 0x6F0 - RW - Select source for NAND signals (HPS Pins or FPGA Interface)

	uint32_t		_pad_0x6f4_0x6f7;

	uint32_t		rgmii1usefpga;		// Offset: 0x6F8 - RW - Select source for RGMII1 signals (HPS Pins or FPGA Interface)

	uint32_t		_pad_0x6fc_0x703[2];

	uint32_t		i2c0usefpga;			// Offset: 0x704 - RW - Select source for I2C0 signals (HPS Pins or FPGA Interface)
	uint32_t		sdmmcusefpga;		// Offset: 0x708 - RW - Recent manuals missing this signal. Seems to be Select source for SD/MMC

	uint32_t		_pad_0x70c_0x713[2];

	uint32_t		rgmii0usefpga;		// Offset: 0x714 - RW - Select source for RGMII0 signals (HPS Pins or FPGA Interface)

	uint32_t		_pad_0x718_0x723[3];

	uint32_t		i2c3usefpga;			// Offset: 0x724 - RW - Select source for I2C3 signals (HPS Pins or FPGA Interface)
	uint32_t		i2c2usefpga;			// Offset: 0x728 - RW - Select source for I2C2 signals (HPS Pins or FPGA Interface)
	uint32_t		i2c1usefpga;			// Offset: 0x72C - RW - Select source for I2C1 signals (HPS Pins or FPGA Interface)
	uint32_t		spim1usefpga;		// Offset: 0x730 - RW - Select source for SPIM1 signals (HPS Pins or FPGA Interface)

	uint32_t		_pad_0x734_0x737;

	uint32_t		spim0usefpga;		// Offset: 0x738 - RW - Select source for SPIM0 signals (HPS Pins or FPGA Interface)
};

// silicon1 (0x000) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067843378.html
#define SYSMGR_SILICON1_REV_MASK 0x0000FFFF	// Bits [15:0] - Silicon revision number
#define SYSMGR_SILICON2_ID_MASK 0xFFFF0000	// Bits [31:16] - Silicon ID

// silicon1 (0x004) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067845000.html
#define SYSMGR_SILICON2_RSV_MASK 0xFFFFFFFF	// Bits [31:0] - Reserved for future use

// wddbg (0x010) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067845624.html
#define SYSMGR_WDDBG_MODE_0_MASK 0x00000003	// Bits [1:0] - Controls behavior of L4 watchdog when CPUs in debug mode. Field array index matches L4 watchdog index
#define SYSMGR_WDDBG_MODE_1 MASK 0x0000000C	// Bits [3:2] - Controls behavior of L4 watchdog when CPUs in debug mode. Field array index matches L4 watchdog index

// bootinfo (0x014) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067846092.html
#define SYSMGR_BOOTINFO_BSEL_MASK 0x00000007	// Bits [2:0] - The boot select field specifies the boot source. It is read by the Boot ROM code on a cold or warm reset to determine the boot source. The HPS BSEL pins value are sampled upon deassertion of cold reset
#define SYSMGR_BOOTINFO_CSEL_MASK 0x00000018	// Bits [4:3] - The clock select field specifies clock information for booting. The clock select encoding is a function of the CSEL value. The clock select field is read by the Boot ROM code on a cold or warm reset when booting from a flash device to get information about how to setup the HPS clocking to boot from the specified clock device. The encoding of the clock select field is specified by the enum associated with this field. The HPS CSEL pins value are sampled upon deassertion of cold reset
#define SYSMGR_BOOTINFO_PINBSEL_MASK 0x000000E0	// Bits [7:5] - Specifies the sampled value of the HPS BSEL pins. The value of HPS BSEL pins are sampled upon deassertion of cold reset
#define SYSMGR_BOOTINFO_PINCSEL_MASK 0x00000300	// Bits [9:8] - Specifies the sampled value of the HPS CSEL pins. The value of HPS CSEL pins are sampled upon deassertion of cold reset

// hpsinfo (0x018) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067846591.html

// parityinj (0x01C) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067847075.html

// fpgaintf_gbl (0x020) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067848042.html

// fpgaintf_indiv (0x024) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067848573.html

// fpgaintf_module (0x028) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067849150.html
#define SYSMGR_FPGAINTF_MODULE_EMAC_0 (1 << 2)	// Used to disable signals from the FPGA fabric to the EMAC modules that could potentially interfere with their normal operation. The array index corresponds to the EMAC module instance
#define SYSMGR_FPGAINTF_MODULE_EMAC_1 (1 << 3)	// Used to disable signals from the FPGA fabric to the EMAC modules that could potentially interfere with their normal operation. The array index corresponds to the EMAC module instance













#define SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGPINMUX	(1 << 0)
#define SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO	(1 << 1)
#define SYSMGR_ECC_OCRAM_EN	(1 << 0)
#define SYSMGR_ECC_OCRAM_SERR	(1 << 3)
#define SYSMGR_ECC_OCRAM_DERR	(1 << 4)
#define SYSMGR_FPGAINTF_USEFPGA	0x1
#define SYSMGR_FPGAINTF_SPIM0	(1 << 0)
#define SYSMGR_FPGAINTF_SPIM1	(1 << 1)
#define SYSMGR_FPGAINTF_EMAC0	(1 << 2)
#define SYSMGR_FPGAINTF_EMAC1	(1 << 3)
#define SYSMGR_FPGAINTF_NAND	(1 << 4)
#define SYSMGR_FPGAINTF_SDMMC	(1 << 5)

#if defined(CONFIG_TARGET_SOCFPGA_GEN5)
#define SYSMGR_SDMMC_SMPLSEL_SHIFT	3
#else
#define SYSMGR_SDMMC_SMPLSEL_SHIFT	4
#endif

#define SYSMGR_SDMMC_DRVSEL_SHIFT	0

/* EMAC Group Bit definitions */
#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_GMII_MII	0x0
#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RGMII		0x1
#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RMII		0x2

#define SYSMGR_EMACGRP_CTRL_PHYSEL0_LSB			0
#define SYSMGR_EMACGRP_CTRL_PHYSEL1_LSB			2
#define SYSMGR_EMACGRP_CTRL_PHYSEL_MASK			0x3



#endif /* FPGA_SOCFPGA_SYSTEM_MANAGER_H_ */
