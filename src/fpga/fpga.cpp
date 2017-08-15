#include "fpga.h"
#include "../common/logger/logger.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../common/consts.h"
#include "../common/addresses.h"
#include "socfpga_reset_manager.h"
#include "socfpga_nic301.h"
#include "socfpga_system_manager.h"
#include "../common/file/filemanager.h"
#include "../common/system/sysmanager.h"

fpga& fpga::instance()
{
	static fpga instance;

	return instance;
}

fpga::fpga()
{
}

fpga::~fpga()
{
	if (map_base != INVALID_ADDRESS_UINT32 && mem_fd != -1)
	{
		munmap(map_base, FPGA_REG_SIZE);
		map_base = INVALID_ADDRESS_UINT32;
	}

	if (mem_fd != -1)
	{
		close(mem_fd);
	}
}


bool fpga::init()
{
	bool result = false;

	// Map FPGA addresses into application (Linux process) address space
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) != -1)
	{
		map_base = (uint32_t *)mmap(nullptr, FPGA_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, FPGA_REG_BASE);
		if (map_base != (uint32_t *)INVALID_ADDRESS)
		{
			// Info logging
			LOGINFO("FPGA address space mapped into the process successfully");
		}
		else
		{
			LOGERROR("Unable to mmap(/dev/mem)\n");
			close(mem_fd);
			mem_fd = -1;
		}
	}

	return result;
}

/*
 * Sends signal to FPGA fabric to reset
 */
void fpga::reboot(bool cold)
{
	core_reset(true);

	if (cold)
		writel(0, &reset_regs->tstscratch);

	writel(2, &reset_regs->ctrl);

	// Stay in frozen state until the whole device rebooted from FPGA side
	while (1);
}

void fpga::core_reset(bool reset)
{
	// Read state for bits 30 and 31 (buttons
	uint32_t gpo = fpga_gpo_read() & ~0xC0000000;

	// Set bit 30 if reset is requested, and bit 31 if not
	fpga_gpo_write(reset ? gpo | 0x40000000 : gpo | 0x80000000);
}

// FPGA load
bool fpga::load_rbf(const char *name)
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
		LOGINFO("Found");

		uint64_t filesize = filemanager::getFileSize(filepath);
		if (filesize > 0)
		{
			// Info logging
			LOGINFO("\nFPGA bitstream size: %llu bytes\n", filesize);

			void* buffer = malloc(filesize);
			if (buffer != nullptr)
			{
				if (filemanager::readFileIntoMemory(filepath, buffer, filesize))
				{

				}
				else
				{
					LOGERROR("Unable to read FPGA bitstream file: %s", filepath);
				}

				free(buffer);
			}
			else
			{
				LOGERROR("Unable to allocate %llu bytes", filesize);
			}
		}
		else
		{
			// Info logging
			LOGINFO("\nFile has zero size");
		}

		// Trigger application restart to follow changes in FPGA
		sysmanager::restartApplication();
	}
	else
	{
		// Info logging
		LOGINFO("Not found");
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

void fpga::do_bridge(bool enable)
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
void fpga::set_led(bool on)
{
	uint32_t gpo = fpga_gpo_read();
	fpga_gpo_write(on ? gpo | 0x20000000 : gpo & ~0x20000000);
}

int fpga::get_buttons_state()
{
	int result = 0;

	fpga_gpo_write(fpga_gpo_read() | 0x80000000);

	int gpi = fpga_gpi_read();


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

void fpga::fpga_gpo_write(uint32_t value)
{
	// Store new value in caching variable
	this->gpo_caching_copy = value;

	// Write value into memory-mapped register
	writel(value, (void*)(SOCFPGA_MGR_ADDRESS + 0x10));
}

uint32_t fpga::fpga_gpo_read()
{
	// Return cached copy of gpo register to save time
	return this->gpo_caching_copy; //readl((void*)(SOCFPGA_MGR_ADDRESS + 0x10));
}

uint32_t fpga::fpga_gpi_read()
{
	return readl((void*)(SOCFPGA_MGR_ADDRESS + 0x14));
}

void fpga::fpga_core_write(uint32_t offset, uint32_t value)
{
	if (offset <= 0x1FFFFF)
		writel(value, (void*)(SOCFPGA_LWFPGASLAVES_ADDRESS + (offset & ~3)));
}

uint32_t fpga::fpga_core_read(uint32_t offset)
{
	uint32_t result = 0;

	if (offset <= 0x1FFFFF)
		result = readl((void*)(SOCFPGA_LWFPGASLAVES_ADDRESS + (offset & ~3)));

	return result;
}

