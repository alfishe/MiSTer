#include "filemanager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

bool filemanager::isFolderExists(char *path)
{
	bool result = false;

	DIR* dir = opendir(path);
	if (dir)
	{
	    // Directory exists
	    closedir(dir);

	    result = true;
	}

	return result;
}

bool filemanager::isFileExists(char *path)
{
	bool result = false;

	if (access(path, F_OK) != -1)
	{
		result = true;
	}

	return result;
}

bool filemanager::isFileWritable(char *path)
{
	bool result = false;

	if (access(path, W_OK) != -1)
	{
		result = true;
	}

	return result;
}

uint64_t filemanager::getFileSize(char *path)
{
	uint64_t result = 0;

	return result;
}

uint64_t filemanager::getFileSize(int fd)
{
	uint64_t result = 0;

	return result;
}

int filemanager::openFileReadOnly(char *path)
{
	int fd = open(path, O_RDONLY);

	return fd;
}


uint8_t* filemanager::readFileIntoMemory(char *filename, int maxSize)
{
	uint8_t* result = nullptr;

	// Initial validation
	if (filename == nullptr)
	{
		// No filename provided
		return result;
	}

	if (maxSize > MAX_MEMORY_BUFFER)
	{
		// Memory block can be too large. Disallow operation
		return result;
	}


	return result;
}

bool filemanager::readFileIntoMemory(char *filename, uint8_t* buffer, int bufferSize)
{
	bool result = false;

	return result;
}


