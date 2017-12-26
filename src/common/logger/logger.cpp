#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

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
	fflush(stdout);
}

void logger::log(const char* message)
{
	fprintf(stdout, message);
	fflush(stdout);
}

// Public methods
void logger::info(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	printf("I:%s\n", buffer);
	fflush(stdout);

	va_end(arguments);
}

void logger::warning(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	printf("W:%s\n", buffer);
	fflush(stdout);

	va_end(arguments);
}

void logger::error(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	printf("E:%s\n", buffer);
	fflush(stdout);

	va_end(arguments);
}

void logger::debug(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	printf("D:%s\n", buffer);
	fflush(stdout);

	va_end(arguments);
}

void logger::trace(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	vsnprintf(buffer, MAX_LOG_MESSAGE_LENGTH, format, arguments);

	printf("T:%s\n", buffer);
	fflush(stdout);

	va_end(arguments);
}



char* logger::geterror()
{
	char* result = strerror(errno);

	return result;
}
