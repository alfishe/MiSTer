#ifndef COMMON_LOGGER_LOGGER_H_
#define COMMON_LOGGER_LOGGER_H_

#define MAX_LOG_MESSAGE_LENGTH 1024

// Shortcuts
#define LOGINFO(...) logger::info(__VA_ARGS__)
#define LOGWARN(...) logger::warning(__VA_ARGS__)
#define LOGERROR(...) logger::error(__VA_ARGS__)

class logger
{
private:
	// Preallocate message buffer
	static char buffer[MAX_LOG_MESSAGE_LENGTH];

	static void log();

public:
	static void info(const char* format, ...);
	static void warning(const char* format, ...);
	static void error(const char* format, ...);

	static void debug(const char* format, ...);
	static void trace(const char* format, ...);

	// System errors processing helpers
	static char* geterror();

private:
	/// Static class, disallow objects creation
	logger();
};

#endif /* COMMON_LOGGER_LOGGER_H_ */
