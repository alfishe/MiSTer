#ifndef STORAGE_DISKMANAGER_H_
#define STORAGE_DISKMANAGER_H_

#include <map>
#include <vector>
#include "disk/basedrive.h"
#include "disk/basedisk.h"

using namespace std;

typedef vector<BaseDrive*> DriveVector;
typedef vector<BaseDisk*> DiskVector;

typedef map<DriveTypeEnum, DriveVector*> DriveGroup;

class DiskManager
{
public:
	// Singleton instance
	static DiskManager& instance();
	virtual ~DiskManager();
private:
	DiskManager() {}; // Disallow direct instances creation

public:
	void reset();

};

#endif /* STORAGE_DISKMANAGER_H_ */
