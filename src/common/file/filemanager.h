#ifndef COMMON_FILE_FILEMANAGER_H_
#define COMMON_FILE_FILEMANAGER_H_

#include <string>
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include "../../common/consts.h"
#include "../../common/types.h"

using namespace std;

class filemanager
{
public:
	// Files/folders information
	static bool isFolderExists(const string& path);
	static bool isPathMounted(const string& path);
	static bool isFileExist(const string& path);
	static bool isFileWritable(const string& path);
	static uint64_t getFileSize(const string& path);
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
	static bool openFile(FileDescriptor *file, const string& filepath);
	static bool openFileReadOnly(FileDescriptor *file, const string& filepath);
	static void closeFile(FileDescriptor *file);

	// Partial file operations
	static bool readFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize);
	static bool writeFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize);

	// Sector operations
	static bool readSector(FileDescriptor *file, uint8_t *buffer);
	static bool writeSector(FileDescriptor *file, uint8_t *buffer);

	// Whole file operations
	static bool readFileIntoMemory(const string& filepath, uint8_t* buffer, uint32_t bufferSize);

	// Complex file operations using plugins
	static bool lockFileForCore(const string& filepath, const string& core);
	static bool unlockFileForCore(const string& filepath, const string& core);

	// Path helpers
	static string getExtension(const string& filename);
	static string getName(const string& filename);


private:
	// Static class, disallow objects creation
	filemanager() {};
};

#endif /* COMMON_FILE_FILEMANAGER_H_ */
