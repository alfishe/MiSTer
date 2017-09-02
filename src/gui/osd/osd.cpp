#include "osd.h"

#include "../../common/logger/logger.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgaconnector.h"
#include "../../fpga/fpgacommand.h"

osd& osd::instance()
{
	static osd instance;

	return instance;
}

osd::~osd()
{
}

void osd::clear()
{
	memset(framebuffer, 0, sizeof(framebuffer));
}

void enable(unsigned char mode)
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);
}

void osd::disable()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDDISABLE);
}

void osd::setTitle(char *title, int arrowDirection)
{

}

void osd::printText(unsigned char line, char *text, unsigned int start, unsigned int width, unsigned int offset, bool invert)
{

}
