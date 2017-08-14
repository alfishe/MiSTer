#ifndef FPGA_FPGA_H_
#define FPGA_FPGA_H_

#include <stdint.h>

// Shortcuts to common functionality
#define DISKLED_ON  fpga::instance().set_led(ON)
#define DISKLED_OFF fpga::instance().set_led(OFF)

class fpga {
// Fields
private:

// Methods
public:
	// Singleton instance
	static fpga& instance();

	// Constructors / destructors
	fpga();
	virtual ~fpga();

	// Service methods
	void init();
	void reboot();

	int is_fpga_ready(int quick);
	int load_rbf(char *name);

	void core_reset(int reset);
	int get_core_id();

	void app_restart();
	char* get_app_name();

	// Indicators
	void set_led(uint32_t state);
};

#endif /* FPGA_FPGA_H_ */
