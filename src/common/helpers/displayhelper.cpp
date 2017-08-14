#include "displayhelper.h"
#include <stdio.h>

#define SIZE_FORMAT_BUFFER_SIZE 32

/*
 * Format data in Binary units (where 1KiB = 1024 bytes and 1MB = 1,048,576 bytes)
 */
char* displayhelper::formatSize(uint64_t size, bool si)
{
	static char result[SIZE_FORMAT_BUFFER_SIZE];
	static uint64_t unit = 1024;
	static char prefixes[] = { "KMGTPE" };
	static char prefixessi[] = { "kMGTPE" };
	static int prefixesSize = sizeof(prefixes) / sizeof(prefixes[0]);

	if (size < unit)
	{
		snprintf(result, SIZE_FORMAT_BUFFER_SIZE, "%lluB", size);
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
			snprintf(result, SIZE_FORMAT_BUFFER_SIZE, "%.1f%ciB", value, prefixessi[decade]);
		}
		else
		{
			snprintf(result, SIZE_FORMAT_BUFFER_SIZE, "%.1f%ciB", value, prefixes[decade]);
		}
	}

	return result;
}

