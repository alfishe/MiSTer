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

#endif /* COMMON_ADDRESSES_H_ */
