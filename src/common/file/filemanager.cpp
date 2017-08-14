#include "filemanager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/statvfs.h>
#include "../logger/logger.h"

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

	struct stat64 st;
	if (stat64(path, &st))
	{
		result = st.st_size;
	}
	else
	{
		LOGWARN("__PRETTY_FUNCTION__: unable to get file size\n%s", logger::geterror());
	}

	return result;
}

uint64_t filemanager::getFileSize(int fd)
{
	uint64_t result = 0;

	struct stat64 st;
	if (fstat64(fd, &st))
	{
		result = st.st_size;
	}
	else
	{
		LOGWARN("__PRETTY_FUNCTION__: unable to get file size\n%s", logger::geterror());
	}

	return result;
}

uint64_t filemanager::getDiskFreeSpace()
{
	uint64_t result = 0;

	struct statvfs svfs;

	if (statvfs(DATA_VOLUME_MOUNT_POINT, &svfs))
	{
		result = svfs.f_bsize * svfs.f_bfree;
	}
	else
	{
		LOGWARN("__PRETTY_FUNCTION__: unable to get available disk space on data volume\n%s", logger::geterror());
	}

	return result;
}

/*
 * file: file descriptor
 * offset: Offset value in bytes
 * origin: Origin for offset. Acceptable values: SEEK_SET, SEEK_CUR, SEEK_END. See: https://linux.die.net/man/3/lseek64
 */
bool fileSeek(fileDescriptor *file, __off64_t offset, int origin)
{
	bool result = false;

	if (file == nullptr)
	{
		LOGWARN("__PRETTY_FUNCTION__: empty file argument");
		return result;
	}

	__off64_t newoffset = lseek64(file->fd, offset, origin);
	if (newoffset >= 0)
	{
		file->offset = newoffset;
	}
	else
	{
		LOGWARN("__PRETTY_FUNCTION__: unable to perform seek\n%s", logger::geterror());
	}

	return result;
}

int filemanager::openFileReadOnly(char *path)
{
	int fd = open(path, O_RDONLY);

	return fd;
}

/*
 * fd: Linux file descriptor
 * offset: Offset value in bytes
 * origin: Origin for offset. Acceptable values: SEEK_SET, SEEK_CUR, SEEK_END. See: https://linux.die.net/man/3/lseek64
 */
bool filemanager::fileSeek(int fd, __off64_t offset, int origin)
{
	bool result = false;


	if (fd == -1 || fcntl(fd, F_GETFD) == -1)
	{
		LOGWARN("__PRETTY_FUNCTION__: incorrect fd argument");
		return result;
	}

	__off64_t newoffset = lseek64(fd, offset, origin);
	if (newoffset < 0)
	{
		LOGWARN("__PRETTY_FUNCTION__: unable to perform seek\n%s", logger::geterror());
	}

	return result;
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


