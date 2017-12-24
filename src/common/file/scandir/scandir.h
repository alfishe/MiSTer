#ifndef COMMON_FILE_SCANDIR_SCANDIR_H_
#define COMMON_FILE_SCANDIR_SCANDIR_H_

#include <functional>
#include <string>
#include <dirent.h>
#include <fnmatch.h>
#include "../../types.h"

using namespace std;

typedef int (*filter_func)(const struct dirent *);
typedef int (*compar_func)(const struct dirent **, const struct dirent **);

// Wrapper on top of Linux scandir C function, allowing to pass filter lambdas
class ScanDir
{
protected:
	struct dirent **_entryList = nullptr;
	int _nEntries = 0;

public:
	virtual ~ScanDir()
	{
		dispose();
	}

	void scanFolder(
		const string& path,
		filter_func = nullptr,
		compar_func = nullptr);

	void scanFolder(
		const char* path,
		filter_func = nullptr,
		compar_func = nullptr);

	void dispose();

	DirectoryEntryVector getScanResults();

	//============= Default filters =====================
	// Unfortunately, it is not possible to pass lambdas with captured variables to scandir as callbacks
	// So it's tricky to create list-based filters (white/black list). They need to be accessible globally.


	// Returns lambda verifying that directory entry has name matching to 'event*'
	// i.e. /dev/input/event0, /dev/input/event1 etc
	static filter_func getInputDevicesFilter()
	{
		return [](const struct dirent *ent) -> int
		{
			return !fnmatch("event*", ent->d_name, 0);
		};
	};

	//============= Default sorters =====================

	// Returns native scandir alphasort function wrapped into lambda
	static compar_func getAlphaSort()
	{
		return [](const struct dirent** dir1, const struct dirent** dir2) -> int
		{
			return alphasort(dir1, dir2);
		};
	}

	// Returns native scandir versionsort function wrapped into lambda
	static compar_func getVersionSort()
	{
		return [](const struct dirent** dir1, const struct dirent** dir2) -> int
		{
			return versionsort(dir1, dir2);
		};
	}
};

#endif /* COMMON_FILE_SCANDIR_SCANDIR_H_ */
