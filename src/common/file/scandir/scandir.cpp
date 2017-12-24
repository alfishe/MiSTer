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
