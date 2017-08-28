#ifndef COMMON_CONSTS_H_
#define COMMON_CONSTS_H_

#include <stdint.h>

// Data volume mount point (path)
#define DATA_VOLUME_MOUNT_POINT "/media/fat"

// Max size for dynamically allocated buffer should not exceed 128MB (128 * 1024 * 1024 = 134217728)
#define MAX_MEMORY_BUFFER 134217728

// Timeout count for FPGA (in clock counts)
#define FPGA_TIMEOUT_COUNT	 0x1000000

// Invalid address (0xFFFFFFFF or-1), casted to required pointer type
#define INVALID_ADDRESS (void *)-1
#define INVALID_ADDRESS_UINT32 (uint32_t *)-1

// File descriptor constant
#define INVALID_FILE_DESCRIPTOR -1

#endif /* COMMON_CONSTS_H_ */
