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
}

void OSD::showHighres()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDENABLE);
	command.sendOSDCommand(OSD_CMD_OSD);
}

void OSD::hide()
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *(fpga.command);

	command.sendOSDCommand(MM1_OSDCMDDISABLE);
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
}

void OSD::compose()
{
	/*
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
	*/

	transferFramebuffer();
}

void OSD::setTitle(const string& title, uint8_t arrows)
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

void OSD::printLine(uint8_t line, const string& text, bool invert)
{
	uint8_t heightLimit = OSD_HIGHRES_HEIGHT_LINES;
	if (line >= heightLimit)
		return;

	unsigned i = 0;
	int curOffset = 16; // (in px). Make 2 symbols right shift (since will be overlapped with title anyway)

	while (i < text.size())
	{
		uint8_t symbol = text[i++];

		if (curOffset < OSD_LINE_LENGTH_BYTES)
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

void OSD::printSymbol(uint8_t row, uint8_t column, char symbol, bool invert)
{
	uint8_t heightLimit = OSD_HIGHRES_HEIGHT_LINES;
	uint8_t widthLimit = OSD_LINE_LENGTH;
	if (row >= heightLimit || column >= widthLimit)
		return;

	// Print symbol by copying bytes from font array
	uint8_t curOffset = column * 8;
	for (uint8_t j = 0; j < 8; ++j)
	{
		uint8_t fontByte = charfont[(uint8_t)symbol][j];
		if (invert)
			fontByte = ~fontByte;

		framebuffer[row][curOffset++] = fontByte;
	}
}

bool OSD::getPixel(const int x, const int y)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (x >= widthLimit || y >= heightLimit)
	{
		LOGWARN("%s: coordinates outside boundaries", __PRETTY_FUNCTION__);
	}

	uint8_t byteY = y / 8;
	uint8_t bitIdx = y % 8;
	uint8_t value = framebuffer[byteY][x];

	value &= (1 << bitIdx);

	bool result = value != 0;

	return result;
}

void OSD::setPixel(const int x, const int y, bool invert)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (x >= widthLimit || y >= heightLimit)
	{
		LOGWARN("%s: coordinates outside boundaries", __PRETTY_FUNCTION__);
	}

	uint8_t byteY = y / 8;
	uint8_t bitIdx = y % 8;
	uint8_t value = framebuffer[byteY][x];

	if (!invert)
	{
		value |= (1 << bitIdx);
	}
	else
	{
		value &= ~(1 << bitIdx);
	}

	framebuffer[byteY][x] = value;
}

// Rectangular region operations
void OSD::fillRect(uint8_t left, uint8_t top, uint8_t width, uint8_t height, bool clear)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (left >= widthLimit || top >= heightLimit)
	{
		LOGWARN("%s: left/top coordinates outside boundaries", __PRETTY_FUNCTION__);
		return;
	}

	for (int x = left; x < left + width && x < widthLimit; x++)
	{
		for (int y = top; y < top + height && y < heightLimit; y++)
		{
			setPixel(x, y, clear);
		}
	}
}

void OSD::fillRectOptimized(uint8_t left, uint8_t top, uint8_t width, uint8_t height, bool clear)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (left >= widthLimit || top >= heightLimit)
	{
		LOGWARN("%s: left/top coordinates outside boundaries", __PRETTY_FUNCTION__);
		return;
	}

	// Calculate if pixel lines are not aligned to byte boundary (8 vertical line pixels correspond to a byte in framebuffer)
	uint8_t topOffset = top % 8;
	uint8_t topByteY = top / 8;
	uint8_t bottomOffset = (top + height) % 8;
	uint8_t bottomByteY = (top + height) / 8;

	// Calculate aligned rectangular where we can use byte operations for better speed
	int alignedLines = height - topOffset - bottomOffset;
	int alignedTop = -1;
	int alignedHeight = -1;
	if (alignedLines > 0 && alignedLines % 8 == 0)
	{
		alignedTop = top + topOffset;
		alignedHeight = alignedLines;
	}

	// Fill upper non-aligned rect
	if (topOffset != 0)
	{
		// Create mask for upper <topOffset> bits
		uint8_t topMask = (1 << topOffset) - 1;
		uint8_t bottomMask = ~topMask;

		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			uint8_t value = framebuffer[topByteY][x];
			if (!clear)
			{
				value = (value & topMask) | (0xFF & bottomMask);
			}
			else
			{
				value = value & bottomMask;
			}
			framebuffer[topByteY][x] = value;
		}
	}

	// Fill aligned rect
	if (alignedTop >= 0 && alignedHeight > 0)
	{
		uint8_t fillByte = clear ? 0x00 : 0xFF;

		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			for (int y = alignedTop / 8; y < (alignedTop + alignedHeight) / 8; y++)
			{
				framebuffer[y][x] = fillByte;
			}
		}
	}

	// Fill bottom non-aligned rect
	if (bottomOffset != 0)
	{
		// Create mask for lower <bottomOffset> bits
		uint8_t topMask = (1 << bottomOffset) - 1;
		uint8_t bottomMask = ~topMask;

		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			uint8_t value = framebuffer[bottomByteY][x];
			if (!clear)
			{
				value = (0xFF & topMask) | (value & bottomMask);
			}
			else
			{
				value = value & topMask;
			}
			framebuffer[bottomByteY][x] = value;
		}
	}
}

void OSD::invertRect(uint8_t left, uint8_t top, uint8_t width, uint8_t height)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (left >= widthLimit || top >= heightLimit)
	{
		LOGWARN("%s: left/top coordinates outside boundaries", __PRETTY_FUNCTION__);
		return;
	}

	for (int x = left; x < left + width && x < widthLimit; x++)
	{
		for (int y = top; y < top + height && y < heightLimit; y++)
		{
			bool value = getPixel(x, y);
			value = !value;
			setPixel(x, y, !value);
		}
	}
}

void OSD::invertRectOptimized(uint8_t left, uint8_t top, uint8_t width, uint8_t height)
{
	static int heightLimit = OSD_HIGHRES_HEIGHT_PX;
	static int widthLimit = OSD_LINE_LENGTH_BYTES;

	if (left >= widthLimit || top >= heightLimit)
	{
		LOGWARN("%s: left/top coordinates outside boundaries", __PRETTY_FUNCTION__);
		return;
	}

	// Calculate if pixel lines are not aligned to byte boundary (8 vertical line pixels correspond to a byte in framebuffer)
	uint8_t topOffset = top % 8;
	uint8_t topByteY = top / 8;
	uint8_t bottomOffset = (top + height) % 8;
	uint8_t bottomByteY = (top + height) / 8;

	// Calculate aligned rectangular where we can use byte operations for better speed
	int alignedLines = height - topOffset - bottomOffset;
	int alignedTop = -1;
	int alignedHeight = -1;
	if (alignedLines > 0 && alignedLines % 8 == 0)
	{
		alignedTop = top + topOffset;
		alignedHeight = alignedLines;
	}

	// Invert upper non-aligned rect
	if (topOffset != 0)
	{
		// Create mask for upper <topOffset> bits
		uint8_t topMask = (1 << topOffset) - 1;
		uint8_t bottomMask = ~topMask;

		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			uint8_t value = framebuffer[topByteY][x];
			value = (value & topMask) | (~value & bottomMask);
			framebuffer[topByteY][x] = value;
		}
	}

	// Invert aligned rect
	if (alignedTop >= 0 && alignedHeight > 0)
	{
		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			for (int y = alignedTop / 8; y < (alignedTop + alignedHeight) / 8; y++)
			{
				uint8_t value = framebuffer[y][x];
				value = ~value;
				framebuffer[y][x] = value;
			}
		}
	}

	// Invert bottom non-aligned rect
	if (bottomOffset != 0)
	{
		// Create mask for lower <bottomOffset> bits
		uint8_t topMask = (1 << bottomOffset) - 1;
		uint8_t bottomMask = ~topMask;

		for (int x = left; x < left + width && x < widthLimit; x++)
		{
			uint8_t value = framebuffer[bottomByteY][x];
			value = (~value & topMask) | (value & bottomMask);
			framebuffer[bottomByteY][x] = value;
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

	if (command.startOSD())
	{
		TRACE("OSD buffer transfer started");

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

		TRACE("OSD buffer transfer finished");
	}
	else
	{
		LOGWARN("%s: Unable to start FPGA command to transfer OSD buffer", __PRETTY_FUNCTION__);
	}
}
