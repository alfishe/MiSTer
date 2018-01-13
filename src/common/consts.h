#ifndef COMMON_CONSTS_H_
#define COMMON_CONSTS_H_

#include <stdint.h>

#define LINUX_MEMORY_DEVICE "/dev/mem"
#define DATA_ROOT "/media/fat"
#define CONFIG_DIR "config"

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

// Limit max number of input devices supported
// This limit includes total number of keyboards / mouses / joysticks that
// can be connected to MiSTer board simultaneously
#define MAX_INPUT_DEVICES 16

// Limit max number of input events generated per device (so MAX_INPUT_DEVICES * MAX_INPUT_EVENTS buffer will be allocated)
#define MAX_INPUT_EVENTS 10

// ======== Events ============

#define EVENT_DEVICE_INSERTED "device_inserted"
#define EVENT_DEVICE_REMOVED "device_removed"

#define EVENT_SHOW_OSD "show_osd"
#define EVENT_HIDE_OSD "hide_osd"

#define EVENT_CORE_SELECTED "core_selected"
#define EVENT_CORE_STARTED "core_started"

#endif /* COMMON_CONSTS_H_ */
