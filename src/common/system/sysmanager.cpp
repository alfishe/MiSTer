#include "sysmanager.h"
#include "../logger/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include "../../fpga/fpgadevice.h"

int sysmanager::getProcessID()
{
	int result = getpid();

	return result;
}

/*
 * Get application executable name (from Linux processes list)
 */
char* sysmanager::getApplicationName()
{
	static char dest[PATH_MAX];
	memset(dest, 0, sizeof(dest));

	char path[64];
	sprintf(path, "/proc/%d/exe", getpid());
	readlink(path, dest, PATH_MAX);

	return dest;
}

/*
 * Fully restart application
 */
void sysmanager::restartApplication()
{
	// Flush all pending changes to disk(s)
	sync();

	FPGADevice::instance().core_reset(true);

	char *appname = getApplicationName();
	printf("Restarting the %s...\n", appname);

	// Spawn new copy of main executable
	char *argv[] = { appname, NULL };
	execve(appname, argv, NULL);

	// Control won't reach this point in normal conditions
	LOGERROR("Spawning via execve failed. Rebooting the device ...\n");
	FPGADevice::instance().reboot(false);
}

const char* sysmanager::getDataRootDir()
{
	return DATA_ROOT;
}

void sysmanager::ensureConfigFolderExists()
{
	char fullPath[PATH_MAX];

	snprintf(fullPath, PATH_MAX, "%s/%s", getDataRootDir(), CONFIG_DIR);

	DIR* dir = opendir(fullPath);
	if (!dir)
	{
		mkdir(fullPath, S_IRWXU | S_IRWXG | S_IRWXO);
	}
	else
	{
		closedir(dir);
	}
}
