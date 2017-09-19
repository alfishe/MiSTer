#ifndef COMMON_FILE_DIRECTORYMANAGER_H_
#define COMMON_FILE_DIRECTORYMANAGER_H_

#include <stdint.h>
#include <algorithm>
#include <list>
#include <memory>
#include <set>

#include "../types.h"

using namespace std;

/*
enum class ScanningMode: uint8_t
{
	SCAN_INIT = 0, 	// start search from beginning of directory
	SCAN_NEXT,		// find next file in directory
	SCAN_PREV,		// find previous file in directory
	SCAN_NEXT_PAGE,	// find next 8 files in directory
	SCAN_PREV_PAGE, // find previous 8 files in directory
	SCAN_SET_ITEM	// find exact item
};

enum class ScanningOptions: uint8_t
{
	SCAN_DIR = (1 << 0),
	SCAN_UMOUNT = (1 << 1)
};
*/

typedef set<const char *> StringSet;
typedef unique_ptr<DirectoryEntry> DirectoryEntryPtr;
typedef list<DirectoryEntryPtr> DirectoryList;
typedef unique_ptr<DirectoryList> DirectoryListPtr;

class DirectoryManager
{
protected:
	// Fields
	static StringSet* fileExclusions;

public:
	static DirectoryManager& instance();
	virtual ~DirectoryManager();

	// Complex stateful methods to refactor
	DirectoryListPtr scanDirectory(
			const char* folderPath,
			StringSet* supportedExtensions,
			bool withExtensions = true);

private:
	// Static class, disallow objects creation
	DirectoryManager();

	static bool isFileAllowed(const char *filename);
	static bool isFileMatchExtension(const char *filename, StringSet* extensions);
};

#endif /* COMMON_FILE_DIRECTORYMANAGER_H_ */
