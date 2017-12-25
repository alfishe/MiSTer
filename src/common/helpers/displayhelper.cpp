#include "displayhelper.h"
#include <stdio.h>

/*
 * Format data in Binary units (where 1KiB = 1024 bytes and 1MB = 1,048,576 bytes)
 */
string DisplayHelper::formatSize(uint64_t size, bool si)
{
	string result;

	static uint64_t unit = 1024;
	static char prefixes[] = { "KMGTPE" };
	static char prefixessi[] = { "kMGTPE" };
	static int prefixesSize = sizeof(prefixes) / sizeof(prefixes[0]);

	if (size < unit)
	{
		result = tfm::format("%lluB", size);
	}
	else
	{
		float value = 0.0;
		int decade = 0;
		uint64_t remainder = size;

		while (size > unit && decade < prefixesSize)
		{
			remainder = size % unit;
			decade++;
			size /= unit;
		}

		value = size + remainder;

		if (si)
		{
			result = tfm::format("%.1f%ciB", value, prefixessi[decade]);
		}
		else
		{
			result = tfm::format("%.1f%ciB", value, prefixes[decade]);
		}
	}

	return result;
}

string DisplayHelper::formatBits(uint8_t value)
{
	string result;
	result.resize(9); // 8 bits + spacer between 4-bit groups

	int idx = 0;
	for (int i = 1 << 7; i > 0; i >>= 1)
	{
		result[idx++] = (value & i) ? '1': '0';

		// Inser spacer between 4-bit groups
		if (idx == 4)
		{
			result[idx] = ' ';
			idx++;
		}
	}

	return result;
}

string DisplayHelper::formatBits(uint16_t value)
{
	string result;

	uint8_t lowByte = value & 0xFF;
	uint8_t highByte = (value & 0x00FF) >> 8;

	result = tfm::format("%s %s", formatBits(highByte), formatBits(lowByte));

	return result;
}

string DisplayHelper::formatBits(uint32_t value)
{
	string result;

	uint8_t byte0 = value & 0x000000FF;
	uint8_t byte1 = (value & 0x0000FF00) >> 8;
	uint8_t byte2 = (value & 0x00FF0000) >> 16;
	uint8_t byte3 = (value & 0xFF000000) >> 24;


	result = tfm::format("%s %s %s %s",
		formatBits(byte0),
		formatBits(byte1),
		formatBits(byte2),
		formatBits(byte3)
		);

	return result;
}
