#ifndef STORAGE_DISK_BASEDRIVE_H_
#define STORAGE_DISK_BASEDRIVE_H_

#include <string>
#include <vector>
#include "../../3rdparty/betterenums/enum.h"

using namespace std;

// Declare enum using Better-enums macro (allows to operate with enum names, indexes and size in runtime)
BETTER_ENUM(DriveTypeEnum, uint8_t,
	Unknown = 0,
	FDD,
	HDD,
	Tape,
	Network
)

class BaseDrive
{
	// Better-enums workaround (declare proxy enum within struct/class)
	typedef DriveTypeEnum DriveType;

	// Public Fields
public:
	// Drive type can be set during object construction only
	const DriveType type;

	// Constructors
public:
	BaseDrive(DriveType type);
	virtual ~BaseDrive();

	// Public methods
public:
	bool insertDisk();
	bool ejectDisk();
	bool isWriteEnabled();


	bool read();
	bool write();
};

#endif /* STORAGE_DISK_BASEDRIVE_H_ */
