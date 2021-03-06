#ifndef COMMON_FILE_DIRECTORYMANAGER_H_
#define COMMON_FILE_DIRECTORYMANAGER_H_

#include <stdint.h>
#include <algorithm>
#include <list>
#include <memory>
#include <set>
#include <string>

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
typedef set<const char*> CharStringSet;
typedef unique_ptr<DirectoryEntryChar> DirectoryEntryPtr;
typedef list<DirectoryEntryPtr> DirectoryList;
typedef unique_ptr<DirectoryList> DirectoryListPtr;

class DirectoryManager
{
protected:
	// Fields
	static CharStringSet* fileExclusions;

public:
	static DirectoryManager& instance();
	virtual ~DirectoryManager();


	DirectoryListPtr scanDirectory(
			string& folderPath,
			CharStringSet* supportedExtensions,
			bool includeFolders = false,
			bool withExtensions = true);

	DirectoryListPtr scanDirectory(
			const char* folderPath,
			CharStringSet* supportedExtensions,
			bool includeFolders = false,
			bool withExtensions = true);

private:
	// Static class, disallow objects creation
	DirectoryManager();

	static bool isFileAllowed(const string& filename);
	static bool isFileAllowed(const char *filename);
	static bool isFileMatchExtension(const string&, CharStringSet* extensions);
	static bool isFileMatchExtension(const char *filename, CharStringSet* extensions);
};

#endif /* COMMON_FILE_DIRECTORYMANAGER_H_ */
