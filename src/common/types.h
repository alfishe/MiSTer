#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <list>
#include <set>
#include <string>
#include <vector>
#include "consts.h"

using namespace std;

#define OFF 0
#define ON 1

typedef struct FileDescriptor
{
	int       fd = INVALID_FILE_DESCRIPTOR;
	int       mode;
	__off64_t size;
	__off64_t offset;
	char      name[PATH_MAX + 1];

	// New attributes
	bool		cacheable;
	bool		readonly;
} FileDescriptor;

typedef struct DirectoryEntryChar
{
	char name[NAME_MAX + 1]; // Full filename
	char displayname[60];	// Trimmed version to fit OSD screen

	bool isFolder = false;
} DirectoryEntryChar;


typedef struct DirectoryEntry
{
	static constexpr int DISPLAY_NAME_SIZE = 60;

	string name;
	string displayname;

	bool isFolder;
} DirectoryEntry;
typedef list<DirectoryEntry> DirectoryEntryList;
typedef vector<DirectoryEntry> DirectoryEntryVector;

typedef struct
{
	uint16_t vid;
	uint16_t pid;
} VIDPID;

typedef vector<string> StringVector;
typedef set<string> StringSet;

#endif /* COMMON_TYPES_H_ */
