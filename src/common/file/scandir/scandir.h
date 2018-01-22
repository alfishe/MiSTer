#ifndef COMMON_FILE_SCANDIR_SCANDIR_H_
#define COMMON_FILE_SCANDIR_SCANDIR_H_

#include "../../logger/logger.h"

#include <algorithm>
#include <functional>
#include <string>
#include <dirent.h>
#include <fnmatch.h>
#include "../../consts.h"
#include "../../types.h"

using namespace std;

typedef int (*filter_func)(const struct dirent *);
typedef int (*compar_func)(const struct dirent **, const struct dirent **);

// Wrapper on top of Linux scandir C function, allowing to pass filter lambdas
class ScanDir
{
protected:
	struct dirent** _entryList = nullptr;
	int _nEntries = 0;

public:
	ScanDir()
	{
		TRACE("ScanDir()");
	}
	virtual ~ScanDir()
	{
		TRACE("~ScanDir()");

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

	// Returns lambda verifying that directory entry has name matching to '*.rbf'
	static filter_func getFPGACoreFilter()
	{
		return [](const struct dirent *ent) -> int
		{
			// Any match to '*.rbf', but not equal to 'menu.rbf' will be returned
			return !fnmatch("*.rbf", ent->d_name, 0) && fnmatch(MENU_CORE, ent->d_name, 0);
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

	static compar_func getAlphaSortCaseInsensitive()
	{
		return [](const struct dirent** dir1, const struct dirent** dir2) -> int
		{
			string str1((*dir1)->d_name);
			string str2((*dir2)->d_name);
			transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
			transform(str2.begin(), str2.end(), str2.begin(), ::tolower);

			return str1.compare(str2);
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
