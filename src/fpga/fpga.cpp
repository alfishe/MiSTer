#include "fpga.h"
#include <sys/stat.h>


fpga& fpga::instance()
{
	static fpga instance;

	return instance;
}

fpga::fpga()
{
}

fpga::~fpga()
{
}

