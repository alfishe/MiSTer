#include "filemanager.h"

#include "../logger/logger.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/statvfs.h>
#include "../consts.h"
#include "../../3rdparty/openbsd/string.h"

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

bool filemanager::isFileExist(char *path)
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
	if (stat64(path, &st) == 0)
	{
		result = st.st_size;
	}
	else
	{
		LOGWARN("%s: unable to get file size\n%s\n", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}

uint64_t filemanager::getFileSize(int fd)
{
	uint64_t result = 0;

	struct stat64 st;
	if (fstat64(fd, &st) == 0)
	{
		result = st.st_size;
	}
	else
	{
		LOGWARN("%s: unable to get file size\n%s", __PRETTY_FUNCTION__, logger::geterror());
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
		LOGWARN("%s: unable to get available disk space on data volume\n%s", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}

/**
 * Flush all unsaved FS information
 */
void filemanager::flush()
{
	sync();
}

/*
 * file: file descriptor
 * offset: Offset value in bytes
 * origin: Origin for offset. Acceptable values: SEEK_SET, SEEK_CUR, SEEK_END. See: https://linux.die.net/man/3/lseek64
 */
bool filemanager::fileSeek(FileDescriptor *file, __off64_t offset, int origin)
{
	bool result = false;

	if (file == nullptr)
	{
		LOGWARN("%s: empty file argument", __PRETTY_FUNCTION__);
		return result;
	}

	__off64_t newoffset = lseek64(file->fd, offset, origin);
	if (newoffset >= 0)
	{
		file->offset = newoffset;
	}
	else
	{
		LOGWARN("%s: unable to perform seek\n%s", __PRETTY_FUNCTION__, logger::geterror());
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
		LOGWARN("%s: incorrect fd argument", __PRETTY_FUNCTION__);
		return result;
	}

	__off64_t newoffset = lseek64(fd, offset, origin);
	if (newoffset < 0)
	{
		LOGWARN("%s: unable to perform seek\n%s", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}

bool filemanager::openFile(FileDescriptor *file, char *filepath)
{
	bool result = false;

	if (file != nullptr)
	{
		// Close previously opened descriptor
		if (file->fd != INVALID_FILE_DESCRIPTOR)
		{
			close(file->fd);
			file->fd = INVALID_FILE_DESCRIPTOR;
		}

		file->fd = open(filepath, O_RDWR);

		if (file->fd != INVALID_FILE_DESCRIPTOR)
		{
			// Safely copy full filepath
			strlcpy(file->name, filepath, sizeof(file->name) / sizeof(file->name[0]));
			result = true;

			TRACE("%s: File '%s' successfully opened as read-write", __PRETTY_FUNCTION__, filepath);
		}
	}

	return result;
}

bool filemanager::openFileReadOnly(FileDescriptor *file, char *filepath)
{
	bool result = false;

	if (file != nullptr)
	{
		// Close previously opened descriptor
		if (file->fd != INVALID_FILE_DESCRIPTOR)
		{
			close(file->fd);
			file->fd = INVALID_FILE_DESCRIPTOR;
		}

		file->fd = open(filepath, O_RDONLY);

		if (file->fd != INVALID_FILE_DESCRIPTOR)
		{
			// Safely copy full filepath
			strlcpy(file->name, filepath, sizeof(file->name) / sizeof(file->name[0]));
			result = true;

			TRACE("%s: File '%s' successfully opened as read-only", __PRETTY_FUNCTION__, filepath);
		}
	}

	return result;
}

void filemanager::closeFile(FileDescriptor *file)
{
	if (file != nullptr && file->fd != INVALID_FILE_DESCRIPTOR)
	{
		close(file->fd);
		file->fd = INVALID_FILE_DESCRIPTOR;
	}
}

bool filemanager::readFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize)
{
	bool result = false;

	if (file != nullptr && file->fd != INVALID_FILE_DESCRIPTOR && buffer != nullptr && bufferSize > 0)
	{
		int res = read(file->fd, buffer, bufferSize);

		if (res > 0)
		{
			result = true;
		}
		else
		{
			LOGWARN("Unable to read file: %s into buffer with size=%ul", file->name, bufferSize);
		}
	}

	return result;
}

bool filemanager::writeFile(FileDescriptor *file, uint8_t *buffer, uint32_t bufferSize)
{
	bool result = false;

	if (file != nullptr && file->fd != INVALID_FILE_DESCRIPTOR && !file->readonly)
	{
		if (writeFile(file, buffer, bufferSize))
		{
			result = true;
		}
	}

	return result;
}

bool filemanager::readFileIntoMemory(char *filepath, uint8_t* buffer, uint32_t bufferSize)
{
	bool result = false;

	// Initial validation
	if (filepath == nullptr || buffer == nullptr || bufferSize == 0)
	{
		return result;
	}

	FileDescriptor file;
	if (openFileReadOnly(&file, filepath))
	{
		if (readFile(&file, buffer, bufferSize))
		{
			result = true;
		}

		closeFile(&file);
	}
	else
	{
		LOGERROR("%s: Unable to open file %s as read-only", __PRETTY_FUNCTION__, filepath);
	}

	return result;
}


