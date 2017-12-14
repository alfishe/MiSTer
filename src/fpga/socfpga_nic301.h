#ifndef FPGA_SOCFPGA_NIC301_H_
#define FPGA_SOCFPGA_NIC301_H_

#include <stdint.h>

// ARM AMBA Network Interconnect (NIC-301)
// See details on L3 (NIC-301) GPV Registers Address Map page: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410068252941.html
// See details: https://static.docs.arm.com/ddi0397/g/DDI0397G_amba_network_interconnect_nic301_r2p1_trm.pdf

// Module instance: sdr
// Base address: 0xFF800000 (L3REGS)
// End address:  0xFF8FFFFF
// Size:			 0x00010000
struct nic301_registers
{
	uint32_t		remap;					// Offset: 0x00 - WO - remap

	/* Security Register Group */
	uint32_t		_pad_0x4_0x8[1];

	uint32_t		l4main;					// Offset: 0x08 - WO - L4 main peripherals security
	uint32_t		l4sp;					// Offset: 0x0C - WO - L4 SP peripherals security
	uint32_t		l4mp;					// Offset: 0x10 - WO - L4 MP peripherals security
	uint32_t		l4osc1;					// Offset: 0x14 - WO - L4 OSC1 peripherals security
	uint32_t		l4spim;					// Offset: 0x18 - WO - L4 SPIM peripherals security
	uint32_t		stm;						// Offset: 0x1C - WO - STM peripherals security
	uint32_t		lwhps2fpgaregs;			// Offset: 0x20 - WO - LWHPS2FPGA AXI bridge registers peripheral security

	uint32_t		_pad_0x24_0x28[1];

	uint32_t		usb1;					// Offset: 0x28 - WO - USB1 registers peripheral security
	uint32_t		nanddata;				// Offset: 0x2C - WO - NAND Flash controller data peripheral security

	uint32_t		_pad_0x30_0x80[20];

	uint32_t		usb0;					// Offset: 0x80 - WO - USB0 registers peripheral security
	uint32_t		nandregs;				// Offset: 0x84 - WO - NAND Flash controller registers peripheral security
	uint32_t		qspidata;				// Offset: 0x88 - WO - QSPI Flash controller data peripheral security
	uint32_t		fpgamgrdata;				// Offset: 0x8C - WO - FPGA Manager data peripheral security
	uint32_t		hps2fpgaregs;			// Offset: 0x90 - WO - HPS2FPGA AXI bridge registers peripheral security
	uint32_t		acp;						// Offset: 0x94 - WO - MPU ACP peripheral security
	uint32_t		rom;						// Offset: 0x98 - WO - ROM peripheral security
	uint32_t		ocram;					// Offset: 0x9C - WO - On-chip RAM peripheral security
	uint32_t		sdrdata;					// Offset: 0xA0 - WO - SDRAM data peripheral security

	uint32_t		_pad_0xa4_0x1fd0[1995];

	/* ID Register Group */
	uint32_t		periph_id_4;				// Offset: 0x1FD0 - RO - Peripheral ID4 register

	uint32_t		_pad_0x1fd4_0x1fe0[3];

	uint32_t		periph_id_0;				// Offset: 0x1FE0 - RO - Peripheral ID0 register
	uint32_t		periph_id_1;				// Offset: 0x1FE4 - RO - Peripheral ID1 register
	uint32_t		periph_id_2;				// Offset: 0x1FE8 - RO - Peripheral ID2 register
	uint32_t		periph_id_3;				// Offset: 0x1FEC - RO - Peripheral ID3 register
	uint32_t		comp_id_0;				// Offset: 0x1FF0 - RO - Component ID0 register
	uint32_t		comp_id_1;				// Offset: 0x1FF4 - RO - Component ID1 register
	uint32_t		comp_id_2;				// Offset: 0x1FF8 - RO - Component ID2 register
	uint32_t		comp_id_3;				// Offset: 0x1FFC - RO - Component ID3 register

	uint32_t		_pad_0x2000_0x2008[2];

	/* L4 MAIN */
	uint32_t		l4main_fn_mod_bm_iss;	// Offset: 0x2008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x200c_0x3008[1023];

	/* L4 SP */
	uint32_t		l4sp_fn_mod_bm_iss;		// Offset: 0x3008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x300c_0x4008[1023];

	/* L4 MP */
	uint32_t		l4mp_fn_mod_bm_iss;		// Offset: 0x4008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x400c_0x5008[1023];

	/* L4 OSC1 */
	uint32_t		l4osc_fn_mod_bm_iss;		// Offset: 0x5008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x500c_0x6008[1023];

	/* L4 SPIM */
	uint32_t 	l4spim_fn_mod_bm_iss;	// Offset: 0x6008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x600c_0x7008[1023];

	/* STM */
	uint32_t		stm_fn_mod_bm_iss;		// Offset: 0x7008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x700c_0x7108[63];

	uint32_t		stm_fn_mod;				// Offset: 0x7108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x710c_0x8008[959];

	/* LWHPS2FPGA */
	uint32_t		lwhps2fpga_fn_mod_bm_iss;// Offset: 0x8008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x800c_0x8108[63];

	uint32_t		lwhps2fpga_fn_mod;		// Offset: 0x8108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x810c_0xa008[1983];

	/* USB1 */
	uint32_t		usb1_fn_mod_bm_iss;		// Offset: 0xA008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0xa00c_0xa044[14];

	uint32_t		usb1_ahb_cntl;			// Offset: 0xA044 - RW - AHB control register

	uint32_t		_pad_0xa048_0xb008[1008];

	/* NANDDATA */
	uint32_t		nanddata_fn_mod_bm_iss;	// Offset: 0xB100 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0xb00c_0xb108[63];

	uint32_t		nanddata_fn_mod;			// Offset: 0xB108 - RW - Issuing functionality modification register

	uint32_t		_pad_0xb10c_0x20008[21439];

	/* USB0 */
	uint32_t		usb0_fn_mod_bm_iss;		// Offset: 0x20008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2000c_0x20044[14];

	uint32_t		usb0_ahb_cntl;			// Offset: 0x20044 - RW - AHB control register

	uint32_t		_pad_0x20048_0x21008[1008];

	/* NANDREGS */
	uint32_t		nandregs_fn_mod_bm_iss;	// Offset: 0x21008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2100c_0x21108[63];

	uint32_t		nandregs_fn_mod;			// Offset: 0x21108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2110c_0x22008[959];

	/* QSPIDATA */
	uint32_t		qspidata_fn_mod_bm_iss;	// Offset: 0x22008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2200c_0x22044[14];

	uint32_t		qspidata_ahb_cntl;		// Offset: 0x22044 - RW - Issuing functionality modification register

	uint32_t		_pad_0x22048_0x23008[1008];

	/* FPGAMGRDATA */
	uint32_t		fpgamgrdata_fn_mod_bm_iss;// Offset: 0x23008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2300c_0x23040[13];

	uint32_t		fpgamgrdata_wr_tidemark;	// Offset: 0x23040 - RW - Write tidemark

	uint32_t		_pad_0x23044_0x23108[49];

	uint32_t		fn_mod;					// Offset: 0x23108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2310c_0x24008[959];

	/* HPS2FPGA */
	uint32_t		hps2fpga_fn_mod_bm_iss;	// Offset: 0x24008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2400c_0x24040[13];

	uint32_t		hps2fpga_wr_tidemark;	// Offset: 0x24040 - RW - Write tidemark

	uint32_t		_pad_0x24044_0x24108[49];

	uint32_t		hps2fpga_fn_mod;			// Offset: 0x24108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2410c_0x25008[959];

	/* ACP */
	uint32_t		acp_fn_mod_bm_iss;		// Offset: 0x25008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2500c_0x25108[63];

	uint32_t		acp_fn_mod;				// Offset: 0x25108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2510c_0x26008[959];

	/* Boot ROM */
	uint32_t		bootrom_fn_mod_bm_iss;	// Offset: 0x26008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2600c_0x26108[63];

	uint32_t		bootrom_fn_mod;			// Offset: 0x26108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2610c_0x27008[959];

	/* On-chip RAM */
	uint32_t		ocram_fn_mod_bm_iss;		// Offset: 0x27008 - RW - Bus matrix issuing functionality modification register

	uint32_t		_pad_0x2700c_0x27040[13];

	uint32_t		ocram_wr_tidemark;		// Offset: 0x27040 - RW - Write tidemark

	uint32_t		_pad_0x27044_0x27108[49];

	uint32_t		ocram_fn_mod;			// Offser: 0x27108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x2710c_0x42024[27590];

	/* DAP */
	uint32_t		dap_fn_mod2;				// Offset: 0x42024 - RW - Functionality modification 2 register
	uint32_t		dap_fn_mod_ahb;			// Offset: 0x42028 - RW - Functionality modification AHB register

	uint32_t		_pad_0x4202c_0x42100[53];

	uint32_t		dap_read_qos;			// Offset: 0x42100 - RW - Read channel QoS value
	uint32_t		dap_write_qos;			// Offset: 0x42104 - RW - Write channel QoS value
	uint32_t		dap_fn_mod;				// Offset: 0x42108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4210c_0x43100[1021];

	/* MPU */
	uint32_t		mpu_read_qos;			// Offset: 0x43100 - RW - Read channel QoS value
	uint32_t		mpu_write_qos;			// Offset: 0x43104 - RW - Write channel QoS value
	uint32_t		mpu_fn_mod;				// Offset: 0x43108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4310c_0x44028[967];

	/* SDMMC */
	uint32_t		sdmmc_fn_mod_ahb;		// Offset: 0x44028 - RW - Functionality modification AHB register

	uint32_t		_pad_0x4402c_0x44100[53];

	uint32_t		sdmmc_read_qos;			// Offset: 0x44100 - RW - Read channel QoS value
	uint32_t		sdmmc_write_qos;			// Offset: 0x44104 - RW - Write channel QoS value
	uint32_t		sdmmc_fn_mod;			// Offset: 0x44108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4410c_0x45100[1021];

	/* DMA */
	uint32_t		dma_read_qos;			// Offset: 0x45100 - RW - Read channel QoS value
	uint32_t		dma_write_qos;			// Offset: 0x45104 - RW - Write channel QoS value
	uint32_t		dma_fn_mod;				// Offset: 0x45108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4510c_0x46040[973];

	/* FPGA2HPS */
	uint32_t		fpga2hps_wr_tidemark;	// Offset: 0x46040 - RW - Write tidemark

	uint32_t		_pad_0x46044_0x46100[47];

	uint32_t		fpga2hps_read_qos;		// Offset: 0x46100 - RW - Read channel QoS value
	uint32_t		fpga2hps_write_qos;		// Offset: 0x46104 - RW - Write channel QoS value
	uint32_t		fpga2hps_fn_mod;			// Offset: 0x46108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4610c_0x47100[1021];

	/* ETR */
	uint32_t		etr_read_qos;			// Offset: 0x47100 - RW - Read channel QoS value
	uint32_t		etr_write_qos;			// Offset: 0x47104 - RW - Write channel QoS value
	uint32_t		etr_fn_mod;				// Offset: 0x47108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4710c_0x48100[1021];

	/* EMAC0 */
	uint32_t		emac0_read_qos;			// Offset: 0x48100 - RW - Read channel QoS value
	uint32_t		emac0_write_qos;			// Offset: 0x48104 - RW - Write channel QoS value
	uint32_t		emac0_fn_mod;			// Offset: 0x48108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4810c_0x49100[1021];

	/* EMAC1 */
	uint32_t		emac1_read_qos;			// Offset: 0x49100 - RW - Read channel QoS value
	uint32_t		emac1_write_qos;			// Offset: 0x49104 - RW - Write channel QoS value
	uint32_t		emac1_fn_mod;			// Offset: 0x49108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4910c_0x4a028[967];

	/* USB0 */
	uint32_t		usb0_fn_mod_ahb;			// Offset: 0x4a028 - RW - AHB control register

	uint32_t		_pad_0x4a02c_0x4a100[53];

	uint32_t		usb0_read_qos;			// Offset: 0x4A100 - RW - Read channel QoS value
	uint32_t		usb0_write_qos;			// Offset: 0x4A104 - RW - Write channel QoS value
	uint32_t		usb0_fn_mod;				// Offset: 0x4A108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4a10c_0x4b100[1021];

	/* NAND */
	uint32_t		nand_read_qos;			// Offset: 0x4B100 - RW - Read channel QoS value
	uint32_t		nand_write_qos;			// Offset: 0x4B104 - RW - Write channel QoS value
	uint32_t		nand_fn_mod;				// Offset: 0x4B108 - RW - Issuing functionality modification register

	uint32_t		_pad_0x4b10c_0x4c028[967];

	/* USB1 */
	uint32_t		usb1_fn_mod_ahb;			// Offset: 0x4c028 - RW - AHB control register

	uint32_t		_pad_0x4c02c_0x4c100[53];

	uint32_t		usb1_read_qos;			// Offset: 0x4C100 - RW - Read channel QoS value
	uint32_t		usb1_write_qos;			// Offset: 0x4C104 - RW - Write channel QoS value
	uint32_t		usb1_fn_mod;				// Offset: 0x4C108 - RW - Issuing functionality modification register
};

#endif /* FPGA_SOCFPGA_NIC301_H_ */
