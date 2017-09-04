#include "common/logger/logger.h"

#include <iostream>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

#include "3rdparty/backward/backward.hpp"
#include "common/system/sysmanager.h"
#include "fpga/fpgadevice.h"
#include "fpga/fpgacommand.h"
#include "cores/coremanager.h"
#include "gui/osd/osd.h"

using namespace std;
using namespace backward;

// Perform compile-time diagnostics to be sure that everything set up well.
#ifndef _LARGEFILE64_SOURCE
#pragma GCC error "ERROR _LARGEFILE64_SOURCE is not set. Large file support requires it. Please pass -D_LARGEFILE64_SOURCE as a compilation flag"
#endif

#ifndef _FILE_OFFSET_BITS
#pragma GCC error "ERROR _FILE_OFFSET_BITS is not set. Required for LFS. Please ass -D_FILE_OFFSET_BITS=64 as a compilation parameter"
#endif

void handler(int sig)
{
	StackTrace st;

	st.load_here(20); //Limit the number of trace depth to 99
	st.skip_n_firsts(3);	//This will skip some backward internal function from the trace

	Printer p;
	p.snippet = true;
	p.object = true;
	p.color_mode = ColorMode::always;
	p.address = true;
	p.print(st, stderr);

	exit(1);
}

int main(int argc, char *argv[])
{
	// Register handler for error/exceptional cases
	signal(SIGSEGV, handler);

	LOGINFO("MiSTer process started with PID:%d\n", sysmanager::getProcessID());

	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	LOGINFO("Setting up FPGA...\n");

	// TODO: Remove debug code
	uint8_t coreType = command.getCoreType();
	//CoreManager::instance().loadCore("memtest.rbf");
	sleep(2);
	coreType = command.getCoreType();

	osd& osd = osd::instance();
	osd.enable();
	sleep(5);

	for (int i = 0; i < 100; i++)
	{
		osd.fill();
		usleep(50 * 1000);
		osd.clear();
		usleep(50 * 1000);
	}

	sleep(5);
	osd.disable();
	// End of debug code

	fflush(stdout);

	return 0;
}
