#ifndef FPGA_FPGA_H_
#define FPGA_FPGA_H_

#include <stdint.h>
#include "socfpga_base_addrs.h"
#include "../common/consts.h"

// Shortcuts to common functionality
#define DISKLED_ON  fpga::instance().set_led(ON)
#define DISKLED_OFF fpga::instance().set_led(OFF)

// Map I/O register address against base address
#define MAP_ADDR(x) (volatile uint32_t*)(&fpga::instance().map_base[(((uint32_t)(x)) & 0xFFFFFF) >> 2])
#define IS_REG(x) (((((uint32_t)(x))-1)>=(FPGA_REG_BASE - 1)) && ((((uint32_t)(x))-1)<(FPGA_REG_BASE + FPGA_REG_SIZE - 1)))

// Generic rounding for alignment
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

class fpga
{
// Fields
private:
	int mem_fd = -1;
	uint32_t *map_base = INVALID_ADDRESS_UINT32;

	// Map SocFPGA standard address regions to readable structures
	struct socfpga_reset_manager  *reset_regs = (socfpga_reset_manager *)((void *)SOCFPGA_RSTMGR_ADDRESS);
	struct socfpga_fpga_manager   *fpgamgr_regs = (socfpga_fpga_manager *)((void *)SOCFPGA_FPGAMGRREGS_ADDRESS);
	struct socfpga_system_manager *sysmgr_regs = (socfpga_system_manager *)((void *)SOCFPGA_SYSMGR_ADDRESS);
	struct nic301_registers       *nic301_regs = (nic301_registers *)((void *)SOCFPGA_L3REGS_ADDRESS);

	// Cached "shadow" copy of FPGA gpo register
	volatile uint32_t gpo_caching_copy = 0;


// Methods
public:
	// Singleton instance
	static fpga& instance();

	// Constructors / destructors
	fpga();
	virtual ~fpga();

	// Register access
	static __inline void writel(uint32_t val, const void* reg)
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

	static __inline uint32_t readl(const void* reg)
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

	static __inline void clrbits_le32(void* addr, uint32_t clear)
		{
			uint32_t reg = readl(addr);
			reg &= ~clear;
			writel(reg, addr);
		}

	static __inline void setbits_le32(void* addr, uint32_t set)
	{
		uint32_t reg = readl(addr);
		reg |= set;
		writel(reg, addr);
	}

	static __inline void clrsetbits_le32(void* addr, uint32_t clear, uint32_t set)
	{
		uint32_t reg = readl(addr);
		reg &= ~clear;
		reg |= set;
		writel(reg, addr);
	}

	// Service methods
	bool init();
	void reboot(bool cold);
	void core_reset();

	// Info methods
	bool is_fpga_ready(int quick);
	int get_fpga_mode();
	void core_reset(bool reset);
	int get_core_id();

	// FPGA load methods
	bool load_rbf(const char *name);
	bool program(const void* rbf_data, uint32_t rbf_size);

	void do_bridge(bool enable);

	// Indicators
	void set_led(bool on);
	int get_buttons_state();

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
	bool fpgamgr_program_poll_usermode();
	uint32_t fpgamanager_get_mode();
	void fpgamanager_set_cd_ratio(uint32_t ratio);
	bool fpgamanager_dclkcnt_set(uint32_t cnt);
};

#endif /* FPGA_FPGA_H_ */
