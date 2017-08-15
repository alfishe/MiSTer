#ifndef FPGA_FPGA_H_
#define FPGA_FPGA_H_

#include <stdint.h>
#include "socfpga_base_addrs.h"

// Shortcuts to common functionality
#define DISKLED_ON  fpga::instance().set_led(ON)
#define DISKLED_OFF fpga::instance().set_led(OFF)

// Map I/O register address against base address
#define MAP_ADDR(x) (volatile uint32_t*)(&fpga::instance().map_base[(((uint32_t)(x)) & 0xFFFFFF) >> 2])
#define IS_REG(x) (((((uint32_t)(x))-1)>=(FPGA_REG_BASE - 1)) && ((((uint32_t)(x))-1)<(FPGA_REG_BASE + FPGA_REG_SIZE - 1)))

class fpga
{
// Fields
private:
	int mem_fd = -1;
	uint32_t *map_base = nullptr;

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

	void do_bridge(bool enable);

	// Indicators
	void set_led(bool on);
	int get_buttons_state();

	// Helper methods
	void fpga_gpo_write(uint32_t value);
	uint32_t fpga_gpo_read();
	uint32_t fpga_gpi_read();
	void fpga_core_write(uint32_t offset, uint32_t value);
	uint32_t fpga_core_read(uint32_t offset);
};

#endif /* FPGA_FPGA_H_ */
