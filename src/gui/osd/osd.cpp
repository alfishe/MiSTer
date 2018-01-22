#include "osd.h"

#include "../../common/logger/logger.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "characters.h"
#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgaconnector.h"
#include "../../fpga/fpgacommand.h"

OSD& OSD::instance()
{
	static OSD instance;

	return instance;
}

OSD::~OSD()
{
}

void OSD::show()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);

	highResolution = false;
}

void OSD::showHighres()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);
	command.sendOSDCommand(OSD_CMD_OSD);

	highResolution = true;
}

void OSD::hide()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDDISABLE);

	highResolution = false;
}

void OSD::fill()
{
	memset(framebuffer, 0xAA, sizeof(framebuffer));

	// Transfer changes to FPGA
	transferFramebuffer();
}

void OSD::clear()
{
	// Clear internal buffer
	clearFramebuffer();

	// Transfer changes to FPGA
	transferFramebuffer();
}

void OSD::compose()
{
	// Copy Title over framebuffer content
	for (unsigned idx = 0; idx < sizeof(titlebuffer) / 2; idx++)
	{
		framebuffer[0][idx] = titlebuffer[0][idx];
		framebuffer[1][idx] = titlebuffer[1][idx];
	}

	// Rotate title strip counter-clockwise and copy over left two columns of framebuffer
	for (unsigned y = 0; y < 2; y++)
	{
		for (unsigned x = 0; x < OSD_HIGHRES_TITLE_WIDTH_PX; x += 8)
		{
			//rotateCharacter(&titlebuffer[y][x], &framebuffer[OSD_HIGHRES_HEIGHT_PX - x][y]);
		}
	}

	transferFramebuffer();
}

void OSD::setTitle(const char *title, uint8_t arrows)
{
	uint8_t idx = 0;
	uint8_t xOffset = 0;

	memset(titlebuffer, 0, sizeof(titlebuffer));

	// Print 2x scaled symbols into title buffer, condensing extra empty columns
	while (true)
	{
		uint8_t symbol = title[idx++];
		if (symbol == '\0')
			break;

		TRACE("Title symbol '%c'", symbol);

		// Determine true symbol width (trimming empty columns on both sides)
		uint8_t startOffset = 0;
		uint8_t width = 8;
		{
			// Process left and right 3 pixel columns (center 2 pixels shouldn't be compressed - Space symbol for example)
			bool isContinuousLeft = charfont[symbol][0] == 0;
			bool isContinuousRight = charfont[symbol][7] == 0;
			for (int i = 0; i < 3; i++)
			{
				uint8_t fontByte = charfont[symbol][i];
				uint8_t fontByteRev = charfont[symbol][7 - i];

				// Process left half
				if (fontByte == 0 && isContinuousLeft)
				{
					startOffset++;
					width--;
				}
				else
					isContinuousLeft = false;

				// Process right half
				if (fontByteRev == 0 && isContinuousRight)
				{
					width--;
				}
				else
					isContinuousRight = false;
			}
		}

		// Print scaled symbol into title buffer
		{
			for (int i = startOffset; i < startOffset + width; i++)
			{
				uint8_t fontByte = charfont[symbol][i];
				uint16_t scaledWord = scale8Bits(fontByte);

				titlebuffer[0][xOffset] = (uint8_t)scaledWord;
				titlebuffer[0][xOffset + 1] = (uint8_t)scaledWord;
				titlebuffer[1][xOffset] = (uint8_t)(scaledWord >> 8);
				titlebuffer[1][xOffset + 1] = (uint8_t)(scaledWord >> 8);

				xOffset += 2;
			}
		}

		if (xOffset >= OSD_HIGHRES_HEIGHT_LINES * 8)
			break;
	}
}

void OSD::printLine(uint8_t line, const char *text, bool invert)
{
	uint8_t heightLimit = highResolution ? OSD_HIGHRES_HEIGHT_LINES : OSD_HEIGHT_LINES;
	if (line >= heightLimit)
		return;

	int i = 0;
	int curOffset = 16; // Make 2 symbols right shift (since will be overlapped with title anyway)

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

// Helper methods

void OSD::clearFramebuffer()
{
	memset(framebuffer, 0, sizeof(framebuffer));
}

void OSD::rotateCharacter(uint8_t *in, uint8_t *out)
{
	uint8_t value;

	for (int i = 0; i < 8; i++)
	{
		value = 0;
		for (int j = 0; j < 8; j++)
		{
			value <<= 1;
			value |= (*(in + j) >> i) & 1;
		}

		*(out + i) = value;
	}
}

uint8_t OSD::scale4Bits(uint8_t byte)
{
	uint8_t result = 0;
	uint8_t mask = 0b00000001;

	uint8_t value = 0;
	for (int i = 0; i < 4; i++)
	{
		value = byte << i;

		result = (result & ~mask) | (value & mask);
		value <<= 1;
		mask <<= 1;
		result = (result & ~mask) | (value & mask);

		mask <<= 1;
	}

	return result;
}

uint16_t OSD::scale8Bits(uint8_t byte)
{
	uint16_t result = 0;
	uint16_t mask = 0b0000000000000001;

	uint16_t expanded = byte;
	uint16_t value = 0;
	for (int i = 0; i < 8; i++)
	{
		value = expanded << i;

		result = (result & ~mask) | (value & mask);
		value <<= 1;
		mask <<= 1;
		result = (result & ~mask) | (value & mask);

		mask <<= 1;
	}

	return result;
}

/*
 * Transfer the whole OSD framebuffer (4096 bytes) to the FPGA side
 * Usually takes ~6.1ms to accomplish
 * Not VBlank synchronized, so don't try to make heavy animations
 */
void OSD::transferFramebuffer()
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
