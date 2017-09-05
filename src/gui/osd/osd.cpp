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

void osd::show()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);

	highResolution = false;
}

void osd::showHighres()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);
	command.sendOSDCommand(OSD_CMD_OSD);

	highResolution = true;
}

void osd::hide()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDDISABLE);

	highResolution = false;
}

void osd::fill()
{
	memset(framebuffer, 0xAA, sizeof(framebuffer));

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

void osd::compose()
{
	// Copy Title over framebuffer content
	for (unsigned idx = 0; idx < sizeof(titlebuffer); idx++)
	{
		int i = idx / 8;
		int j = idx % 8;
		framebuffer[i][j] = titlebuffer[i];
	}

	transferFramebuffer();
}

void osd::setTitle(const char *title, bool arrowDirection)
{
	// Compose the title, condensing character gaps
	this->arrowDirection = arrowDirection;
	int zeros = 0;
	int i = 0, j = 0;
	int curOffset = 0;

	while (1)
	{
		uint8_t symbol = title[i++];

		if (symbol != '\0' && (curOffset < OSD_HEIGHT_PX))
		{
			for (j = 0; j < 8; ++j)
			{
				uint8_t fontByte = charfont[symbol][j];

				if (fontByte != 0)
				{
					zeros = 0;
					titlebuffer[curOffset++] = fontByte;
				}
				else if (zeros == 0)
				{
					titlebuffer[curOffset++] = 0;
					zeros = 1;
				}

				if (curOffset > 63)
					break;
			}
		}
		else
			break;
	}

	for (i = curOffset; i < OSD_HEIGHT_PX; i++)
	{
		titlebuffer[i] = 0;
	}

	// Now centre it:
	int c = (OSD_HEIGHT_PX - 1 - curOffset) / 2;
	memmove(titlebuffer + c, titlebuffer, curOffset);
	for (i = 0; i < c; ++i)
		titlebuffer[i] = 0;

	// Finally rotate it.
	uint8_t rotationBuffer[8];
	for (i = 0; i< OSD_HEIGHT_PX; i += 8)
	{
		rotateCharacter(&titlebuffer[i], rotationBuffer);

		for (c = 0; c < 8; c++)
		{
			titlebuffer[i + c] = rotationBuffer[c];
		}
	}
}

void osd::printLine(uint8_t line, const char *text, bool invert)
{
	uint8_t heightLimit = highResolution ? OSD_HIGHRES_HEIGHT_LINES : OSD_HEIGHT_LINES;
	if (line >= heightLimit)
		return;

	int i = 0;
	int curOffset = 2; // Make 2 symbol right shift (since will be overlapped with title anyway)

	while (true)
	{
		uint8_t symbol = text[i++];

		if (symbol != '\0' && (curOffset < OSD_LINE_LENGTH_BYTES))
		{
			for (int j = 0; j < 8; ++j)
			{
				uint8_t fontByte = charfont[symbol][j];
				if (invert)
					fontByte = ~fontByte;

				framebuffer[line][curOffset++] = fontByte;
			}
		}
		else
		{
			break;
		}
	}
}

void osd::printText(uint8_t line, const char *text, uint8_t start, uint8_t width, uint8_t offset, bool invert)
{
	// line : OSD line number (0-7)
	// text : pointer to null-terminated string
	// start : start position (in pixels)
	// width : printed text length in pixels
	// offset : scroll offset in pixels counting from the start of the string (0-7)
	// invert : inversion flag

	uint8_t *pTitleBuffer;
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

	pTitleBuffer = &titlebuffer[(OSD_HEIGHT_LINES - 1 - line) * 8];
	if (start > 2)
	{
		connector.transferWord(0xFFFF);
		start -= 2;
	}

	i = start > 16 ? 16 : start;
	for (j = 0; j < i / 2; ++j)
	{
		uint8_t value = 255 ^ *pTitleBuffer++;
		connector.transferByte(value);
		connector.transferByte(value);
	}

	if (i & 1)
		connector.transferByte(255 ^ *pTitleBuffer);

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
		pTitleBuffer = &charfont[lineIdx][offset];
		for (; offset < 8; offset++)
			connector.transferByte(*pTitleBuffer++ ^ invert);
	}

	while (width > 8)
	{
		uint8_t b;
		uint8_t lineIdx = *text++;
		pTitleBuffer = &charfont[lineIdx][0];

		for (b = 0; b < 8; b++)
		{
			connector.transferByte(*pTitleBuffer++ ^ invert);
		}

		width -= 8;
	}

	if (width)
	{
		uint8_t lineIdx = *text++;
		pTitleBuffer = &charfont[lineIdx][0];

		while (width--)
			connector.transferByte(*pTitleBuffer++ ^ invert);
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
