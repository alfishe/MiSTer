#include "MiSTer.h"

#include "common/logger/logger.h"
#include <cxxabi.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <sys/prctl.h>

#include "3rdparty/backward/backward.hpp"
#include "3rdparty/tinyformat/tinyformat.h"
#include "common/system/sysmanager.h"
#include "common/exception/misterexception.h"
#include "common/events/messagecenter.h"
#include "common/file/directorymanager.h"
#include "common/file/filemanager.h"
#include "fpga/fpgadevice.h"
#include "fpga/fpgacommand.h"
#include "cores/coremanager.h"
#include "gui/osd/osd.h"
#include "io/input/inputmanager.h"
#include "io/input/baseinputdevice.h"
#include "common/file/scandir/scandir.h"
#include "io/input/keyboard.h"
#include "io/input/devicedetector/devicedetector.h"

using namespace std;
using namespace backward;
using std::chrono::steady_clock;

// Perform compile-time diagnostics to be sure that everything set up well.
#ifndef _LARGEFILE64_SOURCE
#pragma GCC error "ERROR _LARGEFILE64_SOURCE is not set. Large file support requires it. Please pass -D_LARGEFILE64_SOURCE as a compilation flag"
#endif

#ifndef _FILE_OFFSET_BITS
#pragma GCC error "ERROR _FILE_OFFSET_BITS is not set. Required for LFS. Please ass -D_FILE_OFFSET_BITS=64 as a compilation parameter"
#endif

chrono::steady_clock::time_point applicationStart = steady_clock::now();

void testFilesystem()
{
	string value = filemanager::getExtension("/media/fat/menu.rbf");
	LOGINFO("Ext: %s", value.c_str());

	value = filemanager::getExtension("menu.bin");
	LOGINFO("Ext: %s", value.c_str());

	value = filemanager::getExtension("test.file.with.multiple.extensions.txt");
	LOGINFO("Ext: %s", value.c_str());

	value = filemanager::getExtension("file_with_no_extensions");
	LOGINFO("Ext: %s", value.c_str());
}

void testScanDir()
{
	ScanDir instance;
	instance.scanFolder(string("/dev/input"), ScanDir::getInputDevicesFilter(), ScanDir::getAlphaSort());
	auto results = instance.getScanResults();
	instance.dispose();

	LOGINFO("ScanDir returned %d entries", results.size());
	for_each(results.begin(), results.end(),
			[] (DirectoryEntry ent)
			{
				if (ent.isFolder)
					LOGINFO("<DIR>");
				LOGINFO("%", ent.name.c_str());
			}
			);
}

void testDirectories()
{
	DirectoryManager& dirManager = DirectoryManager::instance();

	auto res = dirManager.scanDirectory("", nullptr, true, true);
	auto& files = *res.get();
	LOGINFO("/media/fat has %d files", files.size());
	for (auto it = files.begin(); it != files.end(); it++)
	{
		DirectoryEntryChar* item = (*it).get();
		LOGINFO("%s %s", item->name, item->isFolder ? "<DIR>" : "");
	}

	CharStringSet extensions;
	extensions.insert("rbf");
	extensions.insert("bin");
	auto res1 = dirManager.scanDirectory("", &extensions, false);
	auto& files1 = *res1.get();
	LOGINFO("/media/fat has %d files filtered by extension", files1.size());
	for (auto it = files1.begin(); it != files1.end(); it++)
	{
		DirectoryEntryChar* item = (*it).get();
		LOGINFO("%s - %s", item->name, item->displayname);
	}
}

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

void testDeviceDetector()
{
	DeviceDetector& detector = DeviceDetector::instance();
	detector.init();
	detector.start();

	sleep(20);

	detector.stop();
	detector.dispose();

	// Restart
	detector.init();
	detector.start();
}

void testInputDevices()
{
	InputManager& inputmgr = InputManager::instance();
	inputmgr.detectDevices();

	for_each(inputmgr.m_keyboards.begin(), inputmgr.m_keyboards.end(),
		[](InputDevicePair pair)
		{
			InputDevice& device = pair.second;
			Keyboard keyboard(device.name, device.path);

			keyboard.openDeviceWrite();

			// Test keys operations
			for (int i = 0; i < 1000; i++)
			{
				keyboard.pollKeys();
				if (keyboard.getPressedKeysCount() > 0)
				{
					string pressedKeys = keyboard.dumpKeyBits();
					LOGINFO("%s\n", pressedKeys.c_str());
				}

				usleep(300 * 1000);
			}
			// -Test keys operations

			// Test LED operations
			uint16_t ledBits = keyboard.getDeviceLEDBits();
			for (int i = 0; i < 100; i++)
			{
				uint16_t state = 0x0000;

				switch (i % 3)
				{
					case 0:
						state = LED_NUML;
						break;
					case 1:
						state = LED_SCROLLL;
						break;
					case 2:
						state = LED_CAPSL;
						break;
				}


				keyboard.setLEDState(state, true);

				usleep(100 * 1000);
				//ledBits = keyboard.getLEDState();

				keyboard.setLEDState(state, false);
			}
			// -Test LED operations

			keyboard.closeDevice();
		}
	);

	VIDPID device0 = BaseInputDevice::getInputDeviceVIDPID(0);
	VIDPID device1 = BaseInputDevice::getInputDeviceVIDPID(1);

	string device0name = BaseInputDevice::getInputDeviceName(0);
	string device1name = BaseInputDevice::getInputDeviceName(1);

	LOGINFO("Device0 name: %s", device0name.c_str());
	LOGINFO("Device1 name: %s", device1name.c_str());
}

void testEventMessaging()
{

}

// ==========================================================================================

void handler(int sig)
{
	int result = 1;

	if (sig == SIGHUP)
	{
		LOGINFO("SIGHUP received. Shutting down...");
		// TODO: initiate graceful shutdown for the application
		result = 0;
	}

	if (sig == SIGINT)
	{
		LOGINFO("SIGINT (Ctrl+C) received. Shutting down...");
		// TODO: initiate graceful shutdown for the application
		result = 0;
	}

	if (sig == SIGSEGV)
	{
		StackTrace st;

		st.load_here(99); 	// Limit the number of trace depth to 20
		st.skip_n_firsts(3);	// This will skip some backward internal function from the trace

		Printer p;
		p.snippet = true;
		p.object = true;
		p.color_mode = ColorMode::always;
		p.address = true;
		p.print(st, stderr);
	}

	if (sig == SIGKILL)
	{
		LOGINFO("SIGKILL received. Exiting immediately");
		exit(result);
	}

	// Try to reclaim resources gracefully
	dispose();

	exit(result);
}

int main(int argc, char *argv[])
{
	// Register handlers for various cases
	signal(SIGSEGV, handler);	// System error/exceptional
	signal(SIGHUP, handler);
	signal(SIGINT, handler);
	signal(SIGKILL, handler);

	try
	{
		LOGINFO("MiSTer process started with PID:%d", sysmanager::getProcessID());

		init();

		FPGADevice& fpga = FPGADevice::instance();
		FPGACommand& command = *(fpga.command);

		LOGINFO("Setting up FPGA...");

		// TODO: Remove debug code
		CoreType coreType = command.getCoreType();

		LOGINFO("Core name: %s", command.getCoreName());
		LOGINFO("Core config: %s", command.getCoreConfig());

		//CoreManager::instance().loadCore("memtest.rbf");
		//sleep(2);
		//coreType = command.getCoreType();

		//for (int i = 0; i < 10000000; i++)
		//	testScanDir();

		//for (int i = 0; i < 1000; i++)
		{
			//testEventMessaging();
			//testDeviceDetector();
			testInputDevices();

			sleep(100);

			testScanDir();
			//testFilesystem();
			//testDirectories();
			//testOSD();
		}
		// End of debug code

		fflush(stdout);
	}
	catch (MiSTerException const& e)
	{
		int status;
		string type = abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status);
		istringstream stream(e.what());

		// Extract original exception reason (first line)
		string originalReason;
		getline(stream, originalReason, '\n');

		// Extract the rest as stacktrace information
		istreambuf_iterator<char> begin(stream), end;
		string stacktrace(begin, end);

		LOGERROR("Uncaught exception: %s(\"%s\")", type.c_str(), originalReason.c_str());
		LOGERROR("%s", stacktrace.c_str());
	}
	catch (exception const& e)
	{
		int status;
		string type = abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status);

		LOGERROR("Uncaught exception: %s(\"%s\")", type.c_str(), e.what());

		StackTrace st;
		st.load_here(32);
		Printer p;
		p.print(st);
	}

	dispose();

	return 0;
}

void init()
{
	// Set main thread name
	const string threadName("main");
	pthread_getname_np(pthread_self(), (char*)threadName.c_str(), threadName.size());
	prctl(PR_SET_NAME, threadName.c_str(), 0, 0, 0);

	// Ensure that /config folder exists on data disk
	sysmanager::ensureConfigFolderExists();

	// Notify application
	application.onStart();
}

void dispose()
{
	// Notify application
	application.onTerminate();
}

// Wrapping C++ throw (requires -Wl,--wrap=__cxa_throw or -Xl,--wrap=__cxa_throw options for the linker)
extern "C" void __real___cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *)) __attribute__((noreturn));
extern "C" void __wrap___cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *))
{
	stringstream ss;
	StackTrace st;
	st.load_here(20);
	st.skip_n_firsts(3);

	Printer p;
	p.snippet = true;
	p.object = false;
	p.color_mode = ColorMode::never;
	p.address = false;
	p.ascending = false;
	p.print(st, ss);

	LOGERROR(ss.str().c_str());

	// Call original __cxa_throw
	__real___cxa_throw(thrown_exception, tinfo, dest);
}

