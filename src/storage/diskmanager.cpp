#include "../storage/diskmanager.h"

#include "../common/logger/logger.h"

DiskManager& DiskManager::instance()
{
	static DiskManager instance;

	return instance;
}

DiskManager::~DiskManager()
{
	TRACE("~DiskManager()");

	reset();
}

/// Clear all internal information
///
void DiskManager::reset()
{
}
