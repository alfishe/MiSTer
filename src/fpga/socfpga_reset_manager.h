#ifndef FPGA_SOCFPGA_RESET_MANAGER_H_
#define FPGA_SOCFPGA_RESET_MANAGER_H_

#include <stdint.h>

// Reset mamager
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067769106.html

// Base address: 0xFFD05000 (RSTMGR)
// End address:  0xFFD05FFF
// Size:			 0x00001000
struct socfpga_reset_manager
{
	uint32_t stat;			// Offset: 0x00 - RW - Status register
	uint32_t	 ctrl;			// Offset: 0x04 - RW - Control register
	uint32_t	 counts;			// Offset: 0x08 - RW - Reset cycles count register

	uint32_t	 _pad_0x0C_0x10;

	uint32_t	 mpumodrst;		// Offset: 0x10 - RW - MPU module reset register
	uint32_t	 permodrst;		// Offset: 0x14 - RW - Peripheral module reset register
	uint32_t	 per2modrst;		// Offset: 0x18 - RW - Peripheral 2 module reset register
	uint32_t	 brgmodrst;		// Offset: 0x1C - RW - Brigge module reset register
	uint32_t	 miscmodrst;		// Offset: 0x20 - RW - Miscellaneous module reset register

	uint32_t	 _pad_0x24_0x54[12];

	uint32_t	 tstscratch;		// Offset: 0x54 - RW - Test scratch register
};

// stat (offset 0x00) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/topic/sfo1410067763568.html
#define RSTMGR_STAT_PORVOLTRST 			(1 << 0)		// Built-in POR voltage detector triggered a cold reset (por_voltage_req = 1)
#define RSTMGR_STAT_NPORPINRST 			(1 << 1)		// nPOR pin triggered a cold reset (por_pin_req = 1)
#define RSTMGR_STAT_FPGACOLDRST 			(1 << 2)		// FPGA core triggered a cold reset (f2h_cold_rst_req_n = 1)
#define RSTMGR_STAT_CONFIGIOCOLDRST 		(1 << 3)		// FPGA entered CONFIG_IO mode and a triggered a cold reset
#define RSTMGR_STAT_SWCOLDRST 			(1 << 4)		// Software wrote CTRL.SWCOLDRSTREQ to 1 and triggered a cold reset
#define RSTMGR_STAT_nRSTPINRST 			(1 << 8)		// nRST pin triggered a hardware sequenced warm reset
#define RSTMGR_STAT_FPGAWARMRST 			(1 << 9)		// FPGA core triggered a hardware sequenced warm reset (f2h_warm_rst_req_n = 1)
#define RSTMGR_STAT_SWWARMRST 			(1 << 10)	// Software wrote CTRL.SWWARMRSTREQ to 1 and triggered a hardware sequenced warm reset
#define RSTMGR_STAT_MPUWD0RST 			(1 << 12)	// MPU Watchdog 0 triggered a hardware sequenced warm reset
#define RSTMGR_STAT_MPUWD1RST 			(1 << 13)	// MPU Watchdog 1 triggered a hardware sequenced warm reset
#define RSTMGR_STAT_L4WD0RST 			(1 << 14)	// L4 Watchdog 0 triggered a hardware sequenced warm reset
#define RSTMGR_STAT_L4WD1RST 			(1 << 15)	// L4 Watchdog 1 triggered a hardware sequenced warm reset
#define RSTMGR_STAT_FPGADBGRST 			(1 << 18)	// FPGA triggered debug reset (f2h_dbg_rst_req_n = 1)
#define RSTMGR_STAT_CDBGREQRST 			(1 << 19)	// DAP triggered debug reset
#define RSTMGR_STAT_SDRSELFREFTIMEOUT 	(1 << 24)	// A 1 indicates that Reset Manager's request to the SDRAM Controller Subsystem to put the SDRAM devices into self-refresh mode before starting a hardware sequenced warm reset timed-out and the Reset Manager had to proceed with the warm reset anyway
#define RSTMGR_STAT_FPGAMGRHSTIMEOUT 	(1 << 25)	// A 1 indicates that Reset Manager's request to the FPGA manager to stop driving configuration clock to FPGA CB before starting a hardware sequenced warm reset timed-out and the Reset Manager had to proceed with the warm reset anyway
#define RSTMGR_STAT_SCANHSTIMEOUT 		(1 << 26)	// A 1 indicates that Reset Manager's request to the SCAN manager to stop driving JTAG clock to FPGA CB before starting a hardware sequenced warm reset timed-out and the Reset Manager had to proceed with the warm reset anyway
#define RSTMGR_STAT_FPGAHSTIMEOUT 		(1 << 27)	// A 1 indicates that Reset Manager's handshake request to FPGA before starting a hardware sequenced warm reset timed-out and the Reset Manager had to proceed with the warm reset anyway
#define RSTMGR_STAT_ETRSTALLTIMEOUT 		(1 << 28)	// A 1 indicates that Reset Manager's request to the ETR (Embedded Trace Router) to stall its AXI master port before starting a hardware sequenced warm reset timed-out and the Reset Manager had to proceed with the warm reset anyway

// ctrl (offset 0x04) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067764332.html
#define RSTMGR_CTRL_SWCOLDRSTREQ 		(1 << 0)		// This is a one-shot bit written by software to 1 to trigger a cold reset. It always reads the value 0
#define RSTMGR_CTRL_SWWARMRSTREQ 		(1 << 1)		// This is a one-shot bit written by software to 1 to trigger a hardware sequenced warm reset. It always reads the value 0
#define RSTMGR_CTRL_SDRSELFREFEN 		(1 << 4)		// This field controls whether the contents of SDRAM devices survive a hardware sequenced warm reset. If set to 1, the Reset Manager makes a request to the SDRAM Controller Subsystem to put the SDRAM devices into self-refresh mode before asserting warm reset signals. However, if SDRAM is already in warm reset, Handshake with SDRAM is not performed
#define RSTMGR_CTRL_SDRSELFREFREQ 		(1 << 5)		// Software writes this field 1 to request to the SDRAM Controller Subsystem that it puts the SDRAM devices into self-refresh mode. This is done to preserve SDRAM contents across a software warm reset. Software waits for the SDRSELFREFACK to be 1 and then writes this field to 0. Note that it is possible for the SDRAM Controller Subsystem to never assert SDRSELFREFACK so software should timeout if SDRSELFREFACK is never asserted
#define RSTMGR_CTRL_SDRSELFREQACK 		(1 << 6)		// This is the acknowlege for a SDRAM self-refresh mode request initiated by the SDRSELFREFREQ field. A 1 indicates that the SDRAM Controller Subsystem has put the SDRAM devices into self-refresh mode
#define RSTMGR_CTRL_FPGAMGRHSEN 			(1 << 8)		// Enables a handshake between the Reset Manager and FPGA Manager before a warm reset. The handshake is used to warn the FPGA Manager that a warm reset it coming so it can prepare for it. When the FPGA Manager receives a warm reset handshake, the FPGA Manager drives its output clock to a quiescent state to avoid glitches. If set to 1, the Manager makes a request to the FPGA Managerbefore asserting warm reset signals. However if the FPGA Manager is already in warm reset, the handshake is skipped. If set to 0, the handshake is skipped
#define RSTMGR_CTRL_FPGAMGRHSREQ			(1 << 9)		// Software writes this field 1 to request to the FPGA Manager to idle its output clock. Software waits for the FPGAMGRHSACK to be 1 and then writes this field to 0. Note that it is possible for the FPGA Manager to never assert FPGAMGRHSACK so software should timeout in this case
#define RSTMGR_CTRL_FPGAMGRHSACK			(1 << 10)	// This is the acknowlege (high active) that the FPGA manager has successfully idled its output clock
#define RSTMGR_CTRL_SCANMGRHSEN			(1 << 12)	// Enables a handshake between the Reset Manager and Scan Manager before a warm reset. The handshake is used to warn the Scan Manager that a warm reset it coming so it can prepare for it. When the Scan Manager receives a warm reset handshake, the Scan Manager drives its output clocks to a quiescent state to avoid glitches. If set to 1, the Reset Manager makes a request to the Scan Managerbefore asserting warm reset signals. However if the Scan Manager is already in warm reset, the handshake is skipped. If set to 0, the handshake is skipped
#define RSTMGR_CTRL_SCANMGRHSREQ			(1 << 13)	// Software writes this field 1 to request to the SCAN manager to idle its output clocks. Software waits for the SCANMGRHSACK to be 1 and then writes this field to 0. Note that it is possible for the Scan Manager to never assert SCANMGRHSACK (e.g. its input clock is disabled) so software should timeout in this case
#define RSTMGR_CTRL_SCANMGRHSACK			(1 << 14)	// This is the acknowlege (high active) that the SCAN manager has successfully idled its output clocks
#define RSTMGR_CTRL_FPGAHSEN				(1 << 16)	// This field controls whether to perform handshake with FPGA before asserting warm reset. If set to 1, the Reset Manager makes a request to the FPGAbefore asserting warm reset signals. However if FPGA is already in warm reset state, the handshake is not performed. If set to 0, the handshake is not performed
#define RSTMGR_CTRL_FPGAHSREQ			(1 << 17)	// Software writes this field 1 to initiate handshake request to FPGA . Software waits for the FPGAHSACK to be active and then writes this field to 0. Note that it is possible for the FPGA to never assert FPGAHSACK so software should timeout in this case
#define RSTMGR_CTRL_FPGAHSACK			(1 << 18)	// This is the acknowlege (high active) that the FPGA handshake acknowledge has been received by Reset Manager
#define RSTMGR_CTRL_ETRSTALLEN			(1 << 20)	// This field controls whether the ETR is requested to idle its AXI master interface (i.e. finish outstanding transactions and not initiate any more) to the L3 Interconnect before a warm or debug reset. If set to 1, the Reset Manager makes a request to the ETR to stall its AXI master and waits for it to finish any outstanding AXI transactions before a warm reset of the L3 Interconnect or a debug reset of the ETR. This stalling is required because the debug logic (including the ETR) is reset on a debug reset and the ETR AXI master is connected to the L3 Interconnect which is reset on a warm reset and these resets can happen independently
#define RSTMGR_CTRL_ETRSTALLREQ			(1 << 21)	// Software writes this field 1 to request to the ETR that it stalls its AXI master to the L3 Interconnect. Software waits for the ETRSTALLACK to be 1 and then writes this field to 0. Note that it is possible for the ETR to never assert ETRSTALLACK so software should timeout if ETRSTALLACK is never asserted
#define RSTMGR_CTRL_ETRSTALLACK			(1 << 22)	// This is the acknowlege for a ETR AXI master stall initiated by the ETRSTALLREQ field. A 1 indicates that the ETR has stalled its AXI master
#define RSTMGR_CTRL_ETRSTALLWARMRST		(1 << 23)	// If a warm reset occurs and ETRSTALLEN is 1, hardware sets this bit to 1 to indicate that the stall of the ETR AXI master is pending. Hardware leaves the ETR stalled until software clears this field by writing it with 1. Software must only clear this field when it is ready to have the ETR AXI master start making AXI requests to write trace data

// counts (offset 0x08) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067765299.html
#define RSTMGR_COUNTS_NRSTCNT_MASK		0x00FFFFF00	// Bits [27:8] - The Reset Manager pulls down the nRST pin on a warm reset for the number of cycles specified in this register. A value of 0x0 prevents the Reset Manager from pulling down the nRST pin
#define RSTMGR_COUNTS_WARMRSTCYCLES_MASK	0x0000000FF	// Bits [7:0] - On a warm reset, the Reset Manager releases the reset to the Clock Manager, and then waits for the number of cycles specified in this register before releasing the rest of the hardware controlled resets. Value must be greater than 16

// mpumodrst (offset 0x10) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067765814.html
#define RSTMGR_MPUMODRST_CPU0			(1 << 0)		// Resets Cortex-A9 CPU0 in MPU. When software changes this field from 0 to 1, it triggers the following sequence: 1. CPU0 reset is asserted. cpu0 clkoff is de-asserted 2. after 32 osc1_clk cycles, cpu0 clkoff is asserted. When software changes this field from 1 to 0, it triggers the following sequence: 1.CPU0 reset is de-asserted. 2. after 32 cycles, cpu0 clkoff is de-asserted. Software needs to wait for at least 64 osc1_clk cycles between each change of this field to keep the proper reset/clkoff sequence
#define RSTMGR_MPUMODRST_CPU1			(1 << 1)		// Resets Cortex-A9 CPU1 in MPU. It is reset to 1 on a cold or warm reset. This holds CPU1 in reset until software is ready to release CPU1 from reset by writing 0 to this field. On single-core devices, writes to this field are ignored.On dual-core devices, writes to this field trigger the same sequence as writes to the CPU0 field (except the sequence is performed on CPU1)
#define RSTMGR_MPUMODRST_WDS				(1 << 2)		// Resets both per-CPU Watchdog Reset Status registers in MPU
#define RSTMGR_MPUMODRST_SCUPER			(1 << 3)		// Resets SCU and peripherals. Peripherals consist of the interrupt controller, global timer, both per-CPU private timers, and both per-CPU watchdogs (except for the Watchdog Reset Status registers)
#define RSTMGR_MPUMODRST_L2				(1 << 4)		// Resets L2 cache controller

// permodrst (offset 0x14) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067766547.html
#define RSTMGR_PERMODRST_EMAC0			(1 << 0)		// Resets EMAC0
#define RSTMGR_PERMODRST_EMAC1			(1 << 1)		// Resets EMAC1
#define RSTMGR_PERMODRST_USB0			(1 << 2)		// Resets USB0
#define RSTMGR_PERMODRST_USB1			(1 << 3)		// Resets USB1
#define RSTMGR_PERMODRST_NAND			(1 << 4)		// Resets NAND flash controller
#define RSTMGR_PERMODRST_QSPI			(1 << 5)		// Resets QSPI flash controller
#define RSTMGR_PERMODRST_L4WD0			(1 << 6)		// Resets watchdog 0 connected to L4
#define RSTMGR_PERMODRST_L4WD1			(1 << 7)		// Resets watchdog 1 connected to L4
#define RSTMGR_PERMODRST_OSC1TIMER0		(1 << 8)		// Resets OSC1 timer 0 connected to L4
#define RSTMGR_PERMODRST_OSC1TIMER1		(1 << 9)		// Resets OSC1 timer 1 connected to L4
#define RSTMGR_PERMODRST_SPTIMER0		(1 << 10)	// Resets SP timer 0 connected to L4
#define RSTMGR_PERMODRST_SPTIMER1		(1 << 11)	// Resets SP timer 1 connected to L4
#define RSTMGR_PERMODRST_I2C0			(1 << 12)	// Resets I2C0 controller
#define RSTMGR_PERMODRST_I2C1			(1 << 13)	// Resets I2C1 controller
#define RSTMGR_PERMODRST_I2C2			(1 << 14)	// Resets I2C2 controller
#define RSTMGR_PERMODRST_I2C3			(1 << 15)	// Resets I2C3 controller
#define RSTMGR_PERMODRST_UART0			(1 << 16)	// Resets UART0
#define RSTMGR_PERMODRST_UART1			(1 << 17)	// Resets UART1
#define RSTMGR_PERMODRST_SPIM0			(1 << 18)	// Resets SPIM0 controller
#define RSTMGR_PERMODRST_SPIM1			(1 << 19)	// Resets SPIM1 controller
#define RSTMGR_PERMODRST_SPIS0			(1 << 20)	// Resets SPIS0 controller
#define RSTMGR_PERMODRST_SPIS1			(1 << 21)	// Resets SPIS1 controller
#define RSTMGR_PERMODRST_SDMMC			(1 << 22)	// Resets SD/MMC controller
#define RSTMGR_PERMODRST_CAN0			(1 << 23)	// Resets CAN0 controller. Writes to this field on devices not containing CAN controllers will be ignored
#define RSTMGR_PERMODRST_CAN1			(1 << 24)	// Resets CAN1 controller. Writes to this field on devices not containing CAN controllers will be ignored
#define RSTMGR_PERMODRST_GPIO0			(1 << 25)	// Resets GPIO0
#define RSTMGR_PERMODRST_GPIO1			(1 << 26)	// Resets GPIO1
#define RSTMGR_PERMODRST_GPIO2			(1 << 27)	// Resets GPIO2
#define RSTMGR_PERMODRST_DMA				(1 << 28)	// Resets DMA controller
#define RSTMGR_PERMODRST_SDR				(1 << 29)	// Resets SDRAM Controller Subsystem affected by a warm or cold reset

// per2modrst (offset 0x18) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067767358.html
#define RSTMGR_PER2MODRST_DMAIF0			(1 << 0)		// Resets DMA channel 0 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF1			(1 << 1)		// Resets DMA channel 1 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF2			(1 << 2)		// Resets DMA channel 2 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF3			(1 << 3)		// Resets DMA channel 3 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF4			(1 << 4)		// Resets DMA channel 4 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF5			(1 << 5)		// Resets DMA channel 5 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF6			(1 << 6)		// Resets DMA channel 6 interface adapter between FPGA Fabric and HPS DMA Controller
#define RSTMGR_PER2MODRST_DMAIF7			(1 << 7)		// Resets DMA channel 7 interface adapter between FPGA Fabric and HPS DMA Controller

// brgmodrst (offset 0x1C) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067767967.html
#define RSTMGR_BRGMODRST_HPS2FPGA		(1 << 0)		// Resets HPS2FPGA Bridge
#define RSTMGR_BRGMODRST_LWHPS2FPGA		(1 << 1)		// Resets LWHPS2FPGA Bridge
#define RSTMGR_BRGMODRST_FPGA2HPS		(1 << 2)		// Resets FPGA2HPS Bridge

#define RSTMGR_BRGMODRST_ALL				0x07			// Reset all at once

// miscmodrst (offset 0x20) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/sfo1410067768606.html
#define RSTMGR_MISCMODRST_ROM			(1 << 0)		// Resets Boot ROM
#define RSTMGR_MISCMODRST_OCRAM			(1 << 1)		// Resets On-chip RAM
#define RSTMGR_MISCMODRST_SYSMGR			(1 << 2)		// Resets logic in System Manager that doesn't differentiate between cold and warm resets
#define RSTMGR_MISCMODRST_SYSMGRCOLD		(1 << 3)		// Resets logic in System Manager that is only reset by a cold reset (ignores warm reset)
#define RSTMGR_MISCMODRST_FPGAMGR		(1 << 4)		// Resets FPGA Manager
#define RSTMGR_MISCMODRST_ACPIDMAP		(1 << 5)		// Resets ACP ID Mapper
#define RSTMGR_MISCMODRST_S2F			(1 << 6)		// Resets logic in FPGA core that doesn't differentiate between HPS cold and warm resets (h2f_rst_n = 1)
#define RSTMGR_MISCMODRST_S2FCOLD		(1 << 7)		// Resets logic in FPGA core that is only reset by a cold reset (ignores warm reset) (h2f_cold_rst_n = 1)
#define RSTMGR_MISCMODRST_NRSTPIN		(1 << 8)		// Pulls nRST pin low
#define RSTMGR_MISCMODRST_TIMESTAMPCOLD	(1 << 9)		// Resets debug timestamp to 0 (cold reset only)
#define RSTMGR_MISCMODRST_CLKMGRCOLD		(1 << 10)	// Resets Clock Manager (cold reset only)
#define RSTMGR_MISCMODRST_SCANMGR		(1 << 11)	// Resets Scan Manager
#define RSTMGR_MISCMODRST_FRZCTRLCOLD	(1 << 12)	// Resets Freeze Controller in System Manager (cold reset only)
#define RSTMGR_MISCMODRST_SYSDBG			(1 << 13)	// Resets logic that spans the system and debug domains
#define RSTMGR_MISCMODRST_DBG			(1 << 14)	// Resets logic located only in the debug domain
#define RSTMGR_MISCMODRST_TAPCOLD		(1 << 15)	// Resets portion of DAP JTAG TAP controller no reset by a debug probe reset (i.e. nTRST pin). Cold reset only
#define RSTMGR_MISCMODRST_SDRCOLD		(1 << 16)	// Resets logic in SDRAM Controller Subsystem affected only by a cold reset

// tstscratch (offset 0x54) Fields and masks
// Details: https://www.altera.com/hps/cyclone-v/index.html#topic/suc1416598823950.html
#define RSTMGR_TSTSCRATCH_FIELD0_MASK	0xFFFFFFFFF	// Bits [31:0] - This field can be read and written as a test scratch register without affecting the reset manager function. Note that this register is not affected by a warm reset and can be used for storing pieces of information that must survive a warm reset



// -------------------------------------------------------------------------------------------------------------
// What's below derived from Altera/U-Boot sources and looks inconsistent

#if defined(CONFIG_SOCFPGA_VIRTUAL_TARGET)
#define RSTMGR_CTRL_SWWARMRSTREQ_LSB 2
#else
#define RSTMGR_CTRL_SWWARMRSTREQ_LSB 1
#endif

/*
* Define a reset identifier, from which a permodrst bank ID
* and reset ID can be extracted using the subsequent macros
* RSTMGR_RESET() and RSTMGR_BANK().
*/
#define RSTMGR_BANK_OFFSET	8
#define RSTMGR_BANK_MASK	0x7
#define RSTMGR_RESET_OFFSET	0
#define RSTMGR_RESET_MASK	0x1f
#define RSTMGR_DEFINE(_bank, _offset)		\
	((_bank) << RSTMGR_BANK_OFFSET) | ((_offset) << RSTMGR_RESET_OFFSET)

/* Extract reset ID from the reset identifier. */
#define RSTMGR_RESET(_reset)			\
	(((_reset) >> RSTMGR_RESET_OFFSET) & RSTMGR_RESET_MASK)

/* Extract bank ID from the reset identifier. */
#define RSTMGR_BANK(_reset)			\
	(((_reset) >> RSTMGR_BANK_OFFSET) & RSTMGR_BANK_MASK)

/*
* SocFPGA Cyclone V/Arria V reset IDs, bank mapping is as follows:
* 0 ... mpumodrst
* 1 ... permodrst
* 2 ... per2modrst
* 3 ... brgmodrst
* 4 ... miscmodrst
*/
#define RSTMGR_EMAC0			RSTMGR_DEFINE(1, 0)
#define RSTMGR_EMAC1			RSTMGR_DEFINE(1, 1)
#define RSTMGR_NAND			RSTMGR_DEFINE(1, 4)
#define RSTMGR_QSPI			RSTMGR_DEFINE(1, 5)
#define RSTMGR_L4WD0			RSTMGR_DEFINE(1, 6)
#define RSTMGR_OSC1TIMER0	RSTMGR_DEFINE(1, 8)
#define RSTMGR_UART0			RSTMGR_DEFINE(1, 16)
#define RSTMGR_SPIM0			RSTMGR_DEFINE(1, 18)
#define RSTMGR_SPIM1			RSTMGR_DEFINE(1, 19)
#define RSTMGR_SDMMC			RSTMGR_DEFINE(1, 22)
#define RSTMGR_DMA			RSTMGR_DEFINE(1, 28)
#define RSTMGR_SDR			RSTMGR_DEFINE(1, 29)

/* Create a human-readable reference to SoCFPGA reset. */
#define SOCFPGA_RESET(_name)	RSTMGR_##_name

#endif /* FPGA_SOCFPGA_RESET_MANAGER_H_ */
