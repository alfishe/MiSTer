#include <iostream>
#include <unistd.h>

#include "common/logger/logger.h"

#include "common/system/sysmanager.h"
#include "fpga/fpgadevice.h"
#include "fpga/fpgacommand.h"
#include "cores/coremanager.h"

using namespace std;

// Perform compile-time diagnostics to be sure that everything set up well.
#ifndef _LARGEFILE64_SOURCE
#pragma GCC error "ERROR _LARGEFILE64_SOURCE is not set. Large file support requires it. Please pass -D_LARGEFILE64_SOURCE as a compilation flag"
#endif

#ifndef _FILE_OFFSET_BITS
#pragma GCC error "ERROR _FILE_OFFSET_BITS is not set. Required for LFS. Please ass -D_FILE_OFFSET_BITS=64 as a compilation parameter"
#endif


int main(int argc, char *argv[])
{
	LOGINFO("MiSTer process started with PID:%d\n", sysmanager::getProcessID());

	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	LOGINFO("Setting up FPGA...\n");

	// TODO: Remove debug code
	uint8_t coreType = command.getCoreType();
	//CoreManager::instance().loadCore("memtest.rbf");
	sleep(2);
	coreType = command.getCoreType();

	command.sendOSDCommand(0x41);
	sleep(5);
	command.sendOSDCommand(0x40);
	// End of debug code

	fflush(stdout);

	return 0;
}
