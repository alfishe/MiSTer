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

void testOSD()
{
	osd& osd = osd::instance();
	osd.show();
	sleep(1);

	//osd.fill();
	//osd.setTitle("0123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	//osd.printText(0, "0123456789_abcdefghij_0123456789", 0, 24, 0, false);
	osd.printLine(0, " 0 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(1, " 1 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(2, " 2 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(3, " 3 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(4, " 4 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(5, " 5 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(6, " 6 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(7, " 7 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(8, " 8 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(9, " 9 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(10, " A 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(11, " B 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(12, " C 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(13, " D 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(14, " E 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(15, " F 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.compose();

	sleep(5);
	osd.clear();
	sleep(1);
	osd.hide();
	sleep(2);
	osd.showHighres();
	sleep(1);
	osd.setTitle("Test Title 1234567", true);
	//osd.printLine(0, " 0 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	//osd.printLine(1, " 1 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	//osd.printLine(2, " 2 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(3, " 3 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(4, " 4 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(5, " 5 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(6, " 6 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(7, " 7 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(8, " 8 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(9, " 9 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(10, " A 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(11, " B 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(12, " C 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(13, " D 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.printLine(14, " E 123456789_abcdefghij_0123456789_abcdefghij_0123456789");
	osd.printLine(15, " F 123456789_abcdefghij_0123456789_abcdefghij_0123456789", true);
	osd.compose();

	sleep(5);
	osd.hide();
}

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
	CoreType coreType = command.getCoreType();
	//CoreManager::instance().loadCore("memtest.rbf");
	//sleep(2);
	//coreType = command.getCoreType();

	//for (int i = 0; i < 5; i++)
		testOSD();
	// End of debug code

	fflush(stdout);

	return 0;
}
