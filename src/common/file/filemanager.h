#ifndef COMMON_FILE_FILEMANAGER_H_
#define COMMON_FILE_FILEMANAGER_H_

#include <stdint.h>
#include <inttypes.h>
#include "../../common/consts.h"
#include "../../common/types.h"

class filemanager
{
public:
	static bool isFolderExists(char *path);
	static bool isFileExists(char *path);
	static bool isFileWritable(char *path);

	static uint64_t getFileSize(char *path);
	static uint64_t getFileSize(int fd);

	// Higher level fileDescriptor oriented file methods
	static fileDescriptor getFileInfo(char *path);
	static fileDescriptor getFileInfo(int fd);

	// Raw Posix oriented file methods
	static int openFileReadOnly(char *path);

	static uint8_t* readFileIntoMemory(char *filename, int maxSize);
	static bool readFileIntoMemory(char *filename, uint8_t* buffer, int bufferSize);

private:
	// Static class, disallow objects creation
	filemanager() {};
};

#endif /* COMMON_FILE_FILEMANAGER_H_ */
