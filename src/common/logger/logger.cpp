#include "logger.h"

#include <stdio.h>
#include <cstdarg>

// Field definitions
char logger::buffer[MAX_LOG_MESSAGE_LENGTH];


// Private constructor
logger::logger()
{
}

// Private methods
void logger::log()
{
	// Extra safety
	buffer[MAX_LOG_MESSAGE_LENGTH - 1] = '\x00';

	fprintf(stdout, buffer);
}

// Public methods
void logger::info(const char* format, ...)
{
	va_list arguments;

	snprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	fprintf(stdout, buffer);
}

void logger::warning(const char* format, ...)
{
	va_list arguments;

	snprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	fprintf(stderr, buffer);
}

void logger::error(const char* format, ...)
{
	va_list arguments;

	snprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	fprintf(stderr, buffer);
}

void logger::debug(const char* format, ...)
{
	va_list arguments;

	snprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	fprintf(stderr, buffer);
}

void logger::trace(const char* format, ...)
{
	va_list arguments;

	snprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	fprintf(stderr, buffer);
}
