#include "scandir.h"

#include <stdlib.h>

// Performs folder specified by <path> scanning
// Optional <filter> lambda can be used to filter out unneccessary entries
// Example:
//		ScanDir instance;
//		instance.scanFolder("/dev/input", ScanDir::getInputDevicesFilter(), ScanDir::getAlphaSort());
//		<access entries>
//		instance.dispose();
void ScanDir::scanFolder(const string& path, filter_func filter, compar_func compar)
{
	scanFolder(path.c_str(), filter, compar);
}

void ScanDir::scanFolder(const char* path, filter_func filter, compar_func compar)
{
	// Free previously claimed resources
	dispose();

	// Start new folder scan process
	struct dirent **namelist;
	int nEntries = scandir(path, &namelist, filter, compar);

	if (nEntries > 0)
	{
		_nEntries = nEntries;
		_entryList = namelist;
	}
}

void ScanDir::dispose()
{
	if (_nEntries > 0 && _entryList != nullptr)
	{
		void* ptr;

		for (int i = 0; i < _nEntries; i++)
		{
			ptr = _entryList[i];

			if (ptr != nullptr)
			{
				free(ptr);
			}
		}

		_nEntries = 0;
		_entryList = nullptr;
	}
}

DirectoryEntryVector ScanDir::getScanResults()
{
	DirectoryEntryVector result;

	if (_nEntries > 0 && _entryList != nullptr)
	{
		struct dirent* entry;

		for (int i = 0; i < _nEntries; i++)
		{
			entry = _entryList[i];

			if (entry != nullptr)
			{
				DirectoryEntry dirEntry;
				dirEntry.name = entry->d_name;
				dirEntry.displayname = dirEntry.name.substr(0, DirectoryEntry::DISPLAY_NAME_SIZE);
				dirEntry.isFolder = entry->d_type == DT_DIR;

				result.push_back(dirEntry);
			}
		}

		_nEntries = 0;
		_entryList = nullptr;
	}

	return result;
}

