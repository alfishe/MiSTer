#include "fpgadevice.h"

#include "../common/logger/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fpgaconnector.h"
#include "fpgacommand.h"
#include "../common/consts.h"
#include "../common/addresses.h"
#include "socfpga_reset_manager.h"
#include "socfpga_fpga_manager.h"
#include "socfpga_nic301.h"
#include "socfpga_system_manager.h"
#include "../common/file/filemanager.h"
#include "../common/system/sysmanager.h"

FPGADevice& FPGADevice::instance()
{
	static FPGADevice instance;

	return instance;
}

FPGADevice::FPGADevice()
{
	TRACE("FPGADevice()");

	connector = new FPGAConnector(this);
	command = new FPGACommand(connector);

	init();
}

FPGADevice::~FPGADevice()
{
	TRACE("~FPGADevice()");

	if (command != nullptr)
	{
		delete command;
		command = nullptr;
	}

	if (connector != nullptr)
	{
		delete connector;
		connector = nullptr;
	}

	if (map_base != INVALID_ADDRESS_UINT32 && fdFPGAMemory != -1)
	{
		munmap(map_base, FPGA_REG_SIZE);
		map_base = INVALID_ADDRESS_UINT32;
	}

	if (fdFPGAMemory != -1)
	{
		close(fdFPGAMemory);
		fdFPGAMemory = INVALID_FILE_DESCRIPTOR;
	}
}


bool FPGADevice::init()
{
	bool result = false;

	// Map FPGA addresses into application (Linux process) address space
	if ((fdFPGAMemory = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
	{
		map_base = (uint32_t *)mmap(nullptr, FPGA_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdFPGAMemory, FPGA_REG_BASE);
		if (map_base != (uint32_t *)MAP_FAILED)
		{
			isInitialized = true;

			// Info logging
			LOGINFO("FPGA address space mapped into the process successfully\n");
		}
		else
		{
			LOGERROR("Unable to mmap(/dev/mem)\n");
			LOGSYSTEMERROR();

			close(fdFPGAMemory);
			fdFPGAMemory = -1;
		}
	}
	else
	{
		LOGERROR("Unable to access memory via /dev/mem. Probably program need to be started as 'sudo MiSTer'\n");
	}

	return result;
}

/*
 * Sends signal to FPGA fabric to reset
 */
void FPGADevice::reboot(bool cold)
{
	core_reset(true);

	if (cold)
		writel(0, &reset_regs->tstscratch);

	writel(2, &reset_regs->ctrl);

	// Stay in frozen state until the whole device rebooted from FPGA side
	while (1);
}

void FPGADevice::core_reset(bool reset)
{
	// Read state for bits 30 and 31 (buttons
	uint32_t gpo = gpo_read() & ~0xC0000000;

	// Set bit 30 if reset is requested, and bit 31 if not
	gpo_write(reset ? gpo | 0x40000000 : gpo | 0x80000000);
}

// FPGA load
bool FPGADevice::load_rbf(const char *name)
{
	bool result = false;

	// Should be sufficient if file is located not too deep in folder hierarchy.
	// Otherwise PATH_MAX (4096) should be used.
	int filepath_len = NAME_MAX * 2;
	char filepath[filepath_len];

	// Info logging
	LOGINFO("Loading RBF file: %s...  ", name);

	// Create full core .rbf filepath (assuming it's in root of data volume)
	snprintf(filepath, filepath_len, "%s/%s", sysmanager::getDataRootDir(), name);

#ifdef REBOOT_ON_RBF_LOAD
	// TODO: re-check whether it's still needed
	do_bridge(0);
	result = save_core_name(name);
#else
	// Do loading .RBF file
	if (filemanager::isFileExist(filepath))
	{
		// Info logging
		LOGINFO("Found\n");

		uint64_t filesize = filemanager::getFileSize(filepath);
		if (filesize > 0)
		{
			// Info logging
			LOGINFO("FPGA bitstream size: %llu bytes\n", filesize);

			void* buffer = malloc(filesize);
			if (buffer != nullptr)
			{
				if (filemanager::readFileIntoMemory(filepath, (uint8_t *)buffer, filesize))
				{
					if (program(buffer, filesize))
					{
						result = true;

						LOGINFO("FPGA successfully programmed with '%s' file\n", filepath);
					}
					else
					{
						LOGERROR("Unable to program FPGA with '%s' file\n", filepath);
					}
				}
				else
				{
					LOGERROR("Unable to read FPGA bitstream file: %s\n", filepath);
				}

				free(buffer);
			}
			else
			{
				LOGERROR("Unable to allocate %llu bytes\n", filesize);
			}
		}
		else
		{
			// Info logging
			LOGINFO("File has zero size\n");
		}

		// Trigger application restart to follow changes in FPGA
		//sysmanager::restartApplication();
	}
	else
	{
		// Info logging
		LOGWARN("File '%s' not found\n", filepath);
	}
#endif

	// Info logging
	if (result)
	{
		LOGINFO("OK");
	}
	else
	{
		LOGINFO("FAILED");
	}

	return result;
}

bool FPGADevice::program(const void* rbf_data, uint32_t rbf_size)
{
	bool result = false;

	// Validation checks
	if (rbf_data == nullptr || rbf_size == 0)
	{
		LOGERROR("FPGA: Unable to program with empty data.\n");
		return result;
	}

	if ((uint32_t)rbf_data & 0x3)
	{
		LOGERROR("FPGA: Unaligned data, needs to be aligned to 4-bytes (32-bit) boundary.\n");
		return result;
	}

	// References:
	// Altera Cyclone C Hard Processor System Technical Reference Manual:
	//		Booting and Configuration - http://www.altera.com/literature/hb/cyclone-v/cv_5400A.pdf
	//		FPGA Manager (Section 4) - http://www.altera.com/literature/hb/cyclone-v/cv_54013.pdf
	//		Figure 7-1: Configuration Sequence for Cyclone V Devices - http://www.altera.com/literature/hb/cyclone-v/cv_52007.pdf

	// Step 1: Initialize FPGA manager
	result = fpgamanager_init_programming();

	if (result)
	{
		// Step 2: Write RBF data to FPGA manager
		fpgamanager_program_write(rbf_data, rbf_size);

		// Step 3: Verify FPGA configured successfully
		result = fpgamanager_program_poll_cd();

		if (result)
		{
			// Step 4: Verify FPGA started initialization from RBF data
			result = fpgamanager_program_poll_initphase();

			if (result)
			{
				// Step 5: Verify FPGA returned back to user mode with new bitstream
				result = fpgamgr_program_poll_usermode();

				if (result)
				{
					LOGINFO("FPGA successfully loaded from bitstream file and configured\n");
				}
				else
				{
					LOGERROR("FPGA didn't enter USER mode\n");
				}
			}
			else
			{
				LOGERROR("FPGA didn't enter INIT phase\n");
			}
		}
		else
		{
			LOGERROR("FPGA didn't enter CONFIG DONE state\n");
		}
	}
	else
	{
		LOGERROR("FPGA programming mode initialization failed\n");
	}

	return result;
}

void FPGADevice::do_bridge(bool enable)
{
	if (enable)
	{
		writel(0x00003FFF, (void*)(SOCFPGA_SDR_ADDRESS + 0x5080));
		writel(0x00000000, &reset_regs->brg_mod_reset);
		writel(0x00000019, &nic301_regs->remap);
	}
	else
	{
		writel(0, &sysmgr_regs->fpgaintfgrp_module);
		writel(0, (void*)(SOCFPGA_SDR_ADDRESS + 0x5080));
		writel(7, &reset_regs->brg_mod_reset);
		writel(1, &nic301_regs->remap);
	}
}


// Indicators
void FPGADevice::set_led(bool on)
{
	uint32_t gpo = gpo_read();
	gpo_write(on ? gpo | 0x20000000 : gpo & ~0x20000000);
}

int FPGADevice::get_buttons_state()
{
	int result = 0;

	gpo_write(gpo_read() | 0x80000000);

	int gpi = gpi_read();


	if (gpi >= 0)
	{
		// Mask bits 29 and 30 (buttons state)
		result = (gpi >> 29) & 0b00000011;
	}
	else
	{
		// FPGA is not in user mode. Ignore the data;
	}

	return result;
}

// Helper methods

void FPGADevice::gpo_write(uint32_t value)
{
	// Store new value in caching variable
	this->gpo_caching_copy = value;

	// Write value into memory-mapped register
	writel(value, &fpgamgr_regs->gpo);
}

uint32_t FPGADevice::gpo_read()
{
	// Return cached copy of gpo register to save time
	return this->gpo_caching_copy; //readl((void*)(SOCFPGA_MGR_ADDRESS + 0x10));
}

uint32_t FPGADevice::gpi_read()
{
	uint32_t result = readl(&fpgamgr_regs->gpi);

	return result;
}

void FPGADevice::core_write(uint32_t offset, uint32_t value)
{
	if (offset <= MAX_FPGA_OFFSET)
	{
		// Write 32-bit value using aligned offset address
		writel(value, (void*)(SOCFPGA_LWFPGASLAVES_ADDRESS + (offset & ~3)));
	}
}

uint32_t FPGADevice::core_read(uint32_t offset)
{
	uint32_t result = 0;

	if (offset <= MAX_FPGA_OFFSET)
	{
		// Read 32-bit value using aligned offset address
		result = readl((void*)(SOCFPGA_LWFPGASLAVES_ADDRESS + (offset & ~3)));
	}

	return result;
}

// FPGA manager helpers

/*
 * Get the FPGA mode (stat -> [2:0] mode)
 */
uint32_t FPGADevice::fpgamanager_get_mode()
{
	uint32_t val = readl(&fpgamgr_regs->stat);
	val &= FPGAMGRREGS_STAT_MODE_MASK;

	return val;
}

/*
 * Set CD (Clock Divisor) ratio
 */
void FPGADevice::fpgamanager_set_cd_ratio(uint32_t ratio)
{
	clrsetbits_le32(
		&fpgamgr_regs->ctrl,
		0x3 << FPGAMGRREGS_CTRL_CDRATIO_LSB,
		(ratio & 0x3) << FPGAMGRREGS_CTRL_CDRATIO_LSB);
}

bool FPGADevice::fpgamanager_dclkcnt_set(uint32_t cnt)
{
	bool result = false;
	uint32_t i;

	// Clear done bit if set
	if (readl(&fpgamgr_regs->dclkstat))
		writel(FPGAMGRREGS_DCLKSTAT_DCNTDONE, &fpgamgr_regs->dclkstat);

	// Request DCLK pulses
	writel(cnt, &fpgamgr_regs->dclkcnt);

	// Wait till the dclkcnt set is done
	for (i = 0; i < FPGA_TIMEOUT_COUNT; i++)
	{
		if (!readl(&fpgamgr_regs->dclkstat))
			continue;

		writel(FPGAMGRREGS_DCLKSTAT_DCNTDONE, &fpgamgr_regs->dclkstat);
		result = true;
		break;
	}

	return result;
}

bool FPGADevice::fpgamanager_init_programming()
{
	bool result = false;

	uint32_t msel;
	uint32_t i;

	// Get the MSEL bits
	msel = readl(&fpgamgr_regs->stat);
	msel &= FPGAMGRREGS_STAT_MSEL_MASK;
	msel >>= FPGAMGRREGS_STAT_MSEL_LSB;

	if (msel & 0x8) // MSEL[3] = 1
	{
		// Cfg width = 32 bit
		setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_CFGWDTH_MASK);

		// Set clock ratio
		uint8_t masked_bits = msel & 0x03;
		switch (masked_bits)
		{
			case 0x00:
				// MSEL[1:0] = 0, CD Ratio = 1
				fpgamanager_set_cd_ratio(CDRATIO_x1);
				break;
			case 0x01:
				// MSEL[1:0] = 1, CD Ratio = 4
				fpgamanager_set_cd_ratio(CDRATIO_x4);
				break;
			case 0x02:
				// MSEL[1:0] = 2, CD Ratio = 8
				fpgamanager_set_cd_ratio(CDRATIO_x8);
				break;
		}
	}
	else  // MSEL[3] = 0
	{
		// Cfg width = 16 bit
		clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_CFGWDTH_MASK);

		// Set clock ratio
		uint8_t masked_bits = msel & 0x03;
		switch (masked_bits)
		{
			case 0x00:
				// MSEL[1:0] = 0, CD Ratio = 1
				fpgamanager_set_cd_ratio(CDRATIO_x1);
				break;
			case 0x01:
				// MSEL[1:0] = 1, CD Ratio = 2
				fpgamanager_set_cd_ratio(CDRATIO_x2);
				break;
			case 0x02:
				// MSEL[1:0] = 2, CD Ratio = 4
				fpgamanager_set_cd_ratio(CDRATIO_x4);
				break;
		}
	}

	// Enable FPGA Manager configuration
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCE_MASK);

	// Enable FPGA Manager drive over configuration line
	setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_EN_MASK);

	// Put FPGA into reset phase
	setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCONFIGPULL_MASK);

	// Wait when FPGA enters reset phase (with timeout)
	for (i = 0; i < FPGA_TIMEOUT_COUNT; i++)
	{
		if (fpgamanager_get_mode() == FPGAMGRREGS_STAT_MODE_RESETPHASE)
			break;
	}

	// If successfully entered into reset mode
	if (fpgamanager_get_mode() == FPGAMGRREGS_STAT_MODE_RESETPHASE)
	{
		// Release FPGA from reset phase, entering configuration mode
		clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCONFIGPULL_MASK);

		// Wait until FPGA enters configuration mode (with timeout)
		for (i = 0; i < FPGA_TIMEOUT_COUNT; i++)
		{
			if (fpgamanager_get_mode() == FPGAMGRREGS_STAT_MODE_CFGPHASE)
				break;
		}

		if (fpgamanager_get_mode() == FPGAMGRREGS_STAT_MODE_CFGPHASE)
		{
			// Clear all interrupts in CB Monitor
			writel(0x0FFF, &fpgamgr_regs->gpio_porta_eoi);

			// Enable AXI configuration
			setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_AXICFGEN_MASK);

			result = true;
		}
		else
		{
			LOGERROR("FPGA: Could not enter into configuration mode\n");
		}
	}
	else
	{
		LOGERROR("FPGA: Could not enter the reset mode\n");
	}

	return result;
}

/*
 * Write the RBF data to FPGA Manager
 */
void FPGADevice::fpgamanager_program_write(const void *rbf_data, uint32_t rbf_size)
{
	uint32_t src = (uint32_t)rbf_data;
	uint32_t dst = (uint32_t)MAP_ADDR(SOCFPGA_FPGAMGRDATA_ADDRESS);

	// Number of loops for 32-byte long copy operations (8 ARM registers involved)
	uint32_t loops32 = rbf_size / 32;

	// Number of loops for 4-byte long copy cycles + trailing bytes
	uint32_t loops4 = DIV_ROUND_UP(rbf_size % 32, 4);

	// Write .rbf data to FPGA Manager address block in two steps:
	// First: fast copy using 32-byte blocks
	// Second: 4-byte blocks copy for the rest of content
	// Node, it's highly undesirable to use r7 in GCC embedded asm. that's why u-boot-derived code modified to use r8
	__asm volatile
	(
		"1:	ldmia %0!,{r0-r6, r8}   \n" // Loop 1
		"	stmia %1!,{r0-r6, r8}   \n"
		"	sub	  %1, #32       \n"
		"	subs  %2, #1        \n"
 		"	bne   1b            \n" // End of Loop 1
		"	cmp   %3, #0        \n"
		"	beq   3f            \n"
		"2:	ldr	  %2, [%0], #4  \n" // Loop 2
		"	str   %2, [%1]      \n"
		"	subs  %3, #1        \n"
		"	bne   2b            \n" // End of Loop 2
		"3:	nop                 \n"
		: "+r"(src), "+r"(dst), "+r"(loops32), "+r"(loops4) :
		: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r8", "cc"
	);
}

/*
 * Ensure the FPGA finished configuration
 */
bool FPGADevice::fpgamanager_program_poll_cd()
{
	bool result = false;

	const uint32_t success_mask = FPGAMGRREGS_MON_GPIO_EXT_PORTA_CD;
	const uint32_t error_mask = FPGAMGRREGS_MON_GPIO_EXT_PORTA_NS;

	uint32_t reg;
	uint32_t success;
	uint32_t error;
	uint32_t i;

	// Wait until FPGA configuration completes (with timeout)
	for (i = 0; i < FPGA_TIMEOUT_COUNT; i++)
	{
		reg = readl(&fpgamgr_regs->gpio_ext_porta);
		success = reg & success_mask;
		error = reg & error_mask;

		if (success)
		{
			// CONF_DONE released high - means success
			result = true;
			break;
		}

		if (!error)
		{
			// nSTATUS pulled low - Configuration error happened
			LOGERROR("FPGA: error during configuring. nSTATUS set to 0");
			break;
		}
	}

	if (result && i < FPGA_TIMEOUT_COUNT)
	{
		// FPGA reconfiguration fit into time slot. Everything works as expected.

		// Disable AXI configuration
		clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_AXICFGEN_MASK);

		result = true;
	}
	else
	{
		printf("FPGA: Timeout waiting for programming and reconfiguration finished\n");
	}

	return result;
}

bool FPGADevice::fpgamanager_program_poll_initphase()
{
	bool result = false;

	uint32_t reg;

	// Additional clocks for the CB to enter initialization phase
	fpgamanager_dclkcnt_set(0x4);

	//  Wait until FPGA enters init phase or user mode
	for (uint32_t i = 0; i < FPGA_TIMEOUT_COUNT; i++)
	{
		reg = fpgamanager_get_mode();

		if (reg == FPGAMGRREGS_STAT_MODE_INITPHASE || reg == FPGAMGRREGS_STAT_MODE_USERMODE)
		{
			result = true;
			break;
		}
	}

	return result;
}

/*
 * Ensure the FPGA entered user mode
 */
bool FPGADevice::fpgamgr_program_poll_usermode()
{
	bool result;

	// Additional clocks for the CB to exit initialization phase
	fpgamanager_dclkcnt_set(0x5000);


	// Wait until FPGA enters user mode (with timeout)
	for (uint32_t i = 0; i < FPGA_TIMEOUT_COUNT; i++)
	{
		if (fpgamanager_get_mode() == FPGAMGRREGS_STAT_MODE_USERMODE)
		{
			result = true;
			break;
		}
	}

	if (!result)
	{
		LOGERROR("FPGA: didn't entered user mode within waiting period");
	}

	// To release FPGA Manager drive over configuration line
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_EN_MASK);

	return result;
}
