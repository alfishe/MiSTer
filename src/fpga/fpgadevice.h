#ifndef FPGA_FPGADEVICE_H_
#define FPGA_FPGADEVICE_H_

#include <stdint.h>
#include <string>
#include "socfpga_base_addrs.h"
#include "../common/consts.h"
#include "../common/file/path/path.h"

using namespace std;

// Shortcuts to common functionality
#define DISKLED_ON  FPGADevice::instance().set_led(ON)
#define DISKLED_OFF FPGADevice::instance().set_led(OFF)

// Map I/O register address against base address
#define MAP_ADDR(x) (volatile uint32_t*)(&FPGADevice::instance().map_base[(((uint32_t)(x)) & 0xFFFFFF) >> 2])
#define IS_REG(x) (((((uint32_t)(x))-1)>=(FPGA_REG_BASE - 1)) && ((((uint32_t)(x))-1)<(FPGA_REG_BASE + FPGA_REG_SIZE - 1)))

// Generic rounding for alignment
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

// Forward declarations. Included from fpgadevice.cpp
class FPGAConnector;
class FPGACommand;

class FPGADevice
{
	// Allow those classes to access protected methods and properties directly (save on property methods)
	friend class FPGAConnector;
	friend class FPGACommand;

protected:
	// Fields
	bool isInitialized = false;
	int fdFPGAMemory = INVALID_FILE_DESCRIPTOR;
	uint32_t *map_base = INVALID_ADDRESS_UINT32;

	// Map SocFPGA standard address regions to readable structures
	// Note: for now absolute addresses in physical address space used. Might be needed to adjust, based on map_base address
	//       obtained from mmap() call
	struct socfpga_reset_manager  *reset_regs = (socfpga_reset_manager *)((void *)SOCFPGA_RSTMGR_ADDRESS);
	struct socfpga_fpga_manager   *fpgamgr_regs = (socfpga_fpga_manager *)((void *)SOCFPGA_FPGAMGRREGS_ADDRESS);
	struct socfpga_system_manager *sysmgr_regs = (socfpga_system_manager *)((void *)SOCFPGA_SYSMGR_ADDRESS);
	struct socfpga_sdram_controller *sdram_regs = (socfpga_sdram_controller *)((void *)SOCFPGA_SDR_ADDRESS);
	struct nic301_registers       *nic301_regs = (nic301_registers *)((void *)SOCFPGA_L3REGS_ADDRESS);

	// Cached "shadow" copy of FPGA gpo register
	volatile uint32_t gpo_caching_copy = 0;

// Methods
public:
	// Singleton instance
	static FPGADevice& instance();

	// FPGA interfacing helpers
	FPGAConnector *connector = nullptr;
	FPGACommand *command = nullptr;

	// Constructors / destructors
	FPGADevice(const FPGADevice& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~FPGADevice();
private:
	// Private constructor to prevent explicit instantiation (only singleton instantiation allowed for now)
	FPGADevice();

public:
	// Service methods
	bool init();
	void reboot(bool cold);
	void core_reset();
	void core_reset(bool reset);

	// Info methods
	bool is_fpga_ready(int quick);
	int get_fpga_mode();


	// FPGA load methods
	bool load_rbf(const string& name);
	bool program(const void* rbf_data, uint32_t rbf_size);
	void disableHPSFPGABridges();
	void enableHPSFPGABridges();

#ifdef REBOOT_ON_RBF_LOAD
	void saveCoreNameForUboot(const string& name);
#endif

	// Indicators
	void set_led(bool on);
	int get_buttons_state();

protected:
	// Address resolving
	static __inline uint32_t mapFPGAAddress(uint32_t) __attribute__((always_inline))
	{
		uint32_t result = 0;

		return result;
	}

	// Register access
 	static __inline void writel(uint32_t val, const void* reg) __attribute__((always_inline))
	{
		/*
			if (!IS_REG(reg))
			{
				printf("ERROR: Trying to write undefined address: %p\n.", reg);
				fatal(-1);
			}
		*/
		*MAP_ADDR(reg) = val;
	}

	static __inline uint32_t readl(const void* reg) __attribute__((always_inline))
	{
		/*
			if (!IS_REG(reg))
			{
				printf("ERROR: Trying to read undefined address: %p\n.", reg);
				fatal(-1);
			}
		*/
		return *MAP_ADDR(reg);
	}

	static __inline void clrbits_le32(void* addr, uint32_t clear) __attribute__((always_inline))
	{
		uint32_t reg = readl(addr);
		reg &= ~clear;
		writel(reg, addr);
	}

	static __inline void setbits_le32(void* addr, uint32_t set) __attribute__((always_inline))
	{
		uint32_t reg = readl(addr);
		reg |= set;
		writel(reg, addr);
	}

	static __inline void clrsetbits_le32(void* addr, uint32_t clear, uint32_t set) __attribute__((always_inline))
	{
		uint32_t reg = readl(addr);
		reg &= ~clear;
		reg |= set;
		writel(reg, addr);
	}

	// Helper methods
	void gpo_write(uint32_t value);
	uint32_t gpo_read();
	uint32_t gpi_read();
	void core_write(uint32_t offset, uint32_t value);
	uint32_t core_read(uint32_t offset);

	// FPGA manager helpers
	bool fpgamanager_init_programming();
	void fpgamanager_program_write(const void *rbf_data, uint32_t rbf_size);
	bool fpgamanager_program_poll_cd();
	bool fpgamanager_program_poll_initphase();
	bool fpgamanager_program_poll_usermode();
	uint32_t fpgamanager_get_mode();
	void fpgamanager_set_cd_ratio(uint32_t ratio);
	bool fpgamanager_dclkcnt_set(uint32_t cnt);
};

#endif /* FPGA_FPGADEVICE_H_ */
