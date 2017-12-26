#include "filemanager.h"

#include "../logger/logger.h"

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include "linux/magic.h"
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/statvfs.h>
#include "../consts.h"
#include "../../3rdparty/openbsd/string.h"
#include "../helpers/stringhelper.h"

bool filemanager::isFolderExists(const char *path)
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

bool filemanager::isPathMounted(char *path)
{
	bool result = false;

	struct stat file_stat;
	struct stat parent_stat;

	if (stat(path, &file_stat) != -1)
	{
		if (file_stat.st_mode & S_IFDIR)
		{
			if (stat(dirname(path), &parent_stat) != -1)
			{
				if (file_stat.st_dev != parent_stat.st_dev ||
					(file_stat.st_dev == parent_stat.st_dev &&
					file_stat.st_ino == parent_stat.st_ino)
				)
				{
					LOGINFO("%s is a mountpoint.", path);

					struct statfs fs_stat;
					if (!statfs(path, &fs_stat))
					{
						LOGINFO("%s is FS: 0x%08X", path, fs_stat.f_type);
						if (fs_stat.f_type != EXT4_SUPER_MAGIC)
						{
							LOGINFO("%s is not EXT2/3/4 (which is ok since FAT32 or ExFAT expected).", path);
							result = true;
						}
					}
				}
			}
			else
			{
				LOGERROR("failed to stat /media\n");
			}
		}
		else
		{
			LOGERROR("%s is not a directory.\n", path);
		}
	}
	else
	{
		LOGERROR("failed to stat %s\n", path);
	}

	return result;
}

bool filemanager::isFileExist(const char *path)
{
	bool result = false;

	if (access(path, F_OK) != -1)
	{
		result = true;
	}

	return result;
}

bool filemanager::isFileWritable(const char *path)
{
	bool result = false;

	if (access(path, W_OK) != -1)
	{
		result = true;
	}

	return result;
}

uint64_t filemanager::getFileSize(const char *path)
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
		LOGWARN("%s: invalid parameters", __PRETTY_FUNCTION__);

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

// Path helpers
string filemanager::getExtension(const string& filename)
{
	string result;

	StringVector parts;
	StringHelper::split(filename, '.', &parts);

	if (parts.size() > 1)
	{
		result = parts.back();
	}

	return result;
}

string filemanager::getName(const string& filename)
{
	string result;

	StringVector parts;
	StringHelper::split(filename, '.', &parts);

	if (parts.size() > 0)
	{
		result = parts.front();
	}

	return result;
}
