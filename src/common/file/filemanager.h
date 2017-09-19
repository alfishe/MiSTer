#ifndef COMMON_FILE_FILEMANAGER_H_
#define COMMON_FILE_FILEMANAGER_H_

#include <string>
#include <stdint.h>
#include <inttypes.h>
#include "../../common/consts.h"
#include "../../common/types.h"

using namespace std;

class filemanager
{
public:
	// Files/folders information
	static bool isFolderExists(const char *path);
	static bool isPathMounted(char *path);
	static bool isFileExist(const char *path);
	static bool isFileWritable(const char *path);
	static uint64_t getFileSize(const char *path);
	static uint64_t getFileSize(int fd);
	static uint64_t getDiskFreeSpace();

	// Raw POSIX oriented file methods (for compatibility only)
	static void flush();
	static int openFileReadOnly(char *filepath);
	static bool fileSeek(int fd, __off64_t offset, int origin);

	// Higher level fileDescriptor oriented file methods
	static FileDescriptor getFileInfo(char *path);
	static FileDescriptor getFileInfo(int fd);
	static bool fileSeek(FileDescriptor *file, __off64_t offset, int origin);
	static bool openFile(FileDescriptor *file, char *filepath);
	static bool openFileReadOnly(FileDescriptor *file, char *filepath);
	static void closeFile(FileDescriptor *file);

	// Partial file operations
	static bool readFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize);
	static bool writeFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize);

	// Sector operations
	static bool readSector(FileDescriptor *file, uint8_t *buffer);
	static bool writeSector(FileDescriptor *file, uint8_t *buffer);

	// Whole file operations
	static bool readFileIntoMemory(char *filepath, uint8_t* buffer, uint32_t bufferSize);

	// Complex file operations using plugins
	static bool lockFileForCore(char *path, char *core);
	static bool unlockFileForCore(char *path, char* *core);

	// Path helpers
	static string getExtension(const string& filename);
	static string getName(const string& filename);


private:
	// Static class, disallow objects creation
	filemanager() {};
};

#endif /* COMMON_FILE_FILEMANAGER_H_ */
