#ifndef COMMON_CONSTS_H_
#define COMMON_CONSTS_H_

#include <stdint.h>

#define LINUX_MEMORY_DEVICE "/dev/mem"
#define DATA_ROOT "/media/fat"
#define CONFIG_DIR "config"

#define LINUX_INPUT_DEVICE "/sys/class/input/event%d/device/"
#define LINUX_INPUT_DEVICE_ID "/sys/class/input/event%d/device/id/"
#define LINUX_INPUT_DEVICE_VID "/sys/class/input/event%d/device/id/vendor"
#define LINUX_INPUT_DEVICE_PID "/sys/class/input/event%d/device/id/product"

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

// Magic number that identifies MiSTer system core (sent via gpi[31:8] when gpo[31] bit is not set, i.e. =0)
#define MISTER_CORE_MAGIC_NUMBER 0x5CA623

// Signature prefix for U-Boot extended env settings that might be passed during warm reset.
// Usually such buffer contains name of FPGA core to load after reboot.
#define UBOOT_EXTRA_ENV_SIGNATURE 0x87654321

#endif /* COMMON_CONSTS_H_ */
