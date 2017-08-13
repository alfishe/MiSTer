#ifndef COMMON_CONSTS_H_
#define COMMON_CONSTS_H_

// Max size for dynamically allocated buffer should not exceed 128MB (128 * 1024 * 1024 = 134217728)
#define MAX_MEMORY_BUFFER 134217728

// Timeout count for FPGA (in clock counts)
#define FPGA_TIMEOUT_CNT	 0x1000000

#endif /* COMMON_CONSTS_H_ */
