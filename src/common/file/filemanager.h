#ifndef COMMON_FILE_FILEMANAGER_H_
#define COMMON_FILE_FILEMANAGER_H_

#include <stdint.h>
#include <inttypes.h>
#include "../../common/consts.h"
#include "../../common/types.h"

class filemanager
{
public:
	// Files/folders information
	static bool isFolderExists(char *path);
	static bool isFileExists(char *path);
	static bool isFileWritable(char *path);
	static uint64_t getFileSize(char *path);
	static uint64_t getFileSize(int fd);
	static uint64_t getDiskFreeSpace();

	// Raw POSIX oriented file methods
	static int openFileReadOnly(char *path);
	static bool fileSeek(int fd, __off64_t offset, int origin);

	// Higher level fileDescriptor oriented file methods
	static fileDescriptor getFileInfo(char *path);
	static fileDescriptor getFileInfo(int fd);
	static bool fileSeek(fileDescriptor *file, __off64_t offset, int origin);



	// File operations
	static uint8_t* readFileIntoMemory(char *filename, int maxSize);
	static bool readFileIntoMemory(char *filename, uint8_t* buffer, int bufferSize);


	// Complex file operations using plugins

	static bool lockFileForCore(char *path, char *core);
	static bool unlockFileForCore(char *path, char* *core);


private:
	// Static class, disallow objects creation
	filemanager() {};
};

#endif /* COMMON_FILE_FILEMANAGER_H_ */
