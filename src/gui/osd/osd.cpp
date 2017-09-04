#include "osd.h"

#include "../../common/logger/logger.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "characters.h"
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

void osd::enable()
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

void osd::fill()
{
	memset(framebuffer, 0xFF, sizeof(framebuffer));

	// Transfer changes to FPGA
	transferFramebuffer();
}

void osd::clear()
{
	// Clear internal buffer
	clearFramebuffer();

	// Transfer changes to FPGA
	transferFramebuffer();
}

void osd::setTitle(const char *title, bool arrowDirection)
{
	// Compose the title, condensing character gaps
	this->arrowDirection = arrowDirection;
	int zeros = 0;
	int i = 0, j = 0;
	int outp = 0;

	while (1)
	{
		int c = title[i++];
		if (c && (outp < OSD_HEIGHT_PX))
		{
			unsigned char *p = &charfont[c][0];
			for (j = 0; j < 8; ++j)
			{
				unsigned char nc = *p++;
				if (nc)
				{
					zeros = 0;
					titlebuffer[outp++] = nc;
				}
				else if (zeros == 0)
				{
					titlebuffer[outp++] = 0;
					zeros = 1;
				}
				if (outp > 63)
					break;
			}
		}
		else
			break;
	}

	for (i = outp; i < OSD_HEIGHT_PX; i++)
	{
		titlebuffer[i] = 0;
	}

	// Now centre it:
	int c = (OSD_HEIGHT_PX - 1 - outp) / 2;
	memmove(titlebuffer + c, titlebuffer, outp);

	for (i = 0; i<c; ++i)
		titlebuffer[i] = 0;

	// Finally rotate it.
	for (i = 0; i< OSD_HEIGHT_PX; i += 8)
	{
		unsigned char tmp[8];
		rotateCharacter(&titlebuffer[i], tmp);
		for (c = 0; c<8; ++c)
		{
			titlebuffer[i + c] = tmp[c];
		}
	}
}

void osd::printText(unsigned char line, const char *text, unsigned int start, unsigned int width, unsigned int offset, bool invert)
{
	// line : OSD line number (0-7)
	// text : pointer to null-terminated string
	// start : start position (in pixels)
	// width : printed text length in pixels
	// offset : scroll offset in pixels counting from the start of the string (0-7)
	// invert : inversion flag

	uint8_t *p;
	int i;
	int j;

	FPGADevice& fpga = FPGADevice::instance();
	FPGAConnector& connector = *(fpga.connector);
	FPGACommand& command = *(fpga.command);

	command.startOSD();

	// Select buffer and line to write to
	command.sendCommand(MM1_OSDCMDWRITE | line);

	if (invert)
		invert = 0xff;

	p = &titlebuffer[(OSD_HEIGHT_LINES - 1 - line) * 8];
	if (start > 2)
	{
		connector.transferWord(0xFFFF);
		start -= 2;
	}

	i = start > 16 ? 16 : start;
	for (j = 0; j < i / 2; ++j)
	{
		uint8_t value = 255 ^ *p++;
		connector.transferByte(value);
		connector.transferByte(value);
	}

	if (i & 1)
		connector.transferByte(255 ^ *p);

	start -= i;

	if (start > 2)
	{
		connector.transferWord(0xffff);
		start -= 2;
	}

	while (start--)
		connector.transferByte(0x00);

	if (offset)
	{
		width -= 8 - offset;

		uint8_t lineIdx = *text++;
		p = &charfont[lineIdx][offset];
		for (; offset < 8; offset++)
			connector.transferByte(*p++ ^ invert);
	}

	while (width > 8)
	{
		uint8_t b;
		uint8_t lineIdx = *text++;
		p = &charfont[lineIdx][0];

		for (b = 0; b < 8; b++)
		{
			connector.transferByte(*p++ ^ invert);
		}

		width -= 8;
	}

	if (width)
	{
		uint8_t lineIdx = *text++;
		p = &charfont[lineIdx][0];

		while (width--)
			connector.transferByte(*p++ ^ invert);
	}

	command.endOSD();
}

// Helper methods

void osd::clearFramebuffer()
{
	memset(framebuffer, 0, sizeof(framebuffer));
}

void osd::rotateCharacter(uint8_t *in, uint8_t *out)
{
	int a;

	for (int b = 0; b < 8; ++b)
	{
		a = 0;
		for (int c = 0; c < 8; ++c)
		{
			a <<= 1;
			a |= (in[c] >> b) & 1;
		}

		out[b] = a;
	}
}

/*
 * Transfer the whole OSD framebuffer (4096) bytes to the FPGA side
 * Usually takes ~6.1ms to accomplish
 * Not VBlank synchronized, so don't try to make heavy animations
 */
void osd::transferFramebuffer()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGAConnector& connector = *(fpga.connector);
	FPGACommand& command = *(fpga.command);

	command.startOSD();

	// Write to buffer command (Line is selected as render start MM1_OSDCMDWRITE | 0)
	command.sendCommand(MM1_OSDCMDWRITE);

	// Transfer changes to FPGA framefuffer (byte size transfers)
	uint8_t *pBuffer = &framebuffer[0][0];
	for (unsigned idx = 0; idx < sizeof(framebuffer); idx++)
	{
		//unsigned j = idx % OSD_LINE_LENGTH_BYTES;
		//unsigned i = idx / OSD_LINE_LENGTH_BYTES;
		//connector.transferByte(framebuffer[i][j]);

		connector.transferByte(*(pBuffer++));
	}

	command.endOSD();
}
