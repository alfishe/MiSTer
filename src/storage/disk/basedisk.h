#ifndef STORAGE_DISK_BASEDISK_H_
#define STORAGE_DISK_BASEDISK_H_

#include <string>
#include <vector>
#include "../../3rdparty/betterenums/enum.h"

using namespace std;

// Declare enum using Better-enums macro (allows to operate with enum names, indexes and size in runtime)
BETTER_ENUM(DiskTypeEnum, uint8_t,
	Unknown = 0,
	PhysicalImage,		// Contains physical media layout information
	LogicalImage,		// Contains only logical information (file allocation tables, dirs and files)
	PhysicalDisk,
	PhysicalPartition,
	Mapped,				// Data blocks are located in multiple file and combined into disk image on-the-fly
	Remote				// Disk image is stored remotely (latency and access reliability can be unpredictable)
)


class BaseDisk
{
	// Better-enums workaround (declare proxy enum within struct/class)
	typedef DiskTypeEnum DiskType;

	// Constructors
public:
	BaseDisk();
	virtual ~BaseDisk();

	// Public methods
public:

};

#endif /* STORAGE_DISK_BASEDISK_H_ */
