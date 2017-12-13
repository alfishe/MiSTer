#ifndef COMMON_ADDRESSES_H_
#define COMMON_ADDRESSES_H_

/*
 * Base address in HPS address space for FPGA registers
 */
#define FPGA_REG_BASE 0xFF000000

/**
 * Size for FPGA registers in HPS address space
 */
#define FPGA_REG_SIZE 0x01000000

/*
 * Max allowed offset for FPGA addresses
 */
#define MAX_FPGA_OFFSET 0x1FFFFF

/*
 * RAM buffer to pass extra env settings to U-Boot during warm restart
 */
#define UBOOT_EXTRA_ENV_BASE 0x1FFFF000

/*
 * Size for U-Boot extra env settings buffer
 */
#define UBOOT_EXTRA_ENV_SIZE 0x1000

#endif /* COMMON_ADDRESSES_H_ */
