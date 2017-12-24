#include "directorymanager.h"

#include "../logger/logger.h"

 #include <sys/types.h>
 #include <dirent.h>
#include <limits>
#include <stdio.h>
#include <string.h>

#include "../../3rdparty/openbsd/string.h"
#include "../file/filemanager.h"
#include "../system/sysmanager.h"

StringSet* DirectoryManager::fileExclusions = new StringSet();

// Constructors / destructors

DirectoryManager& DirectoryManager::instance()
{
	static DirectoryManager instance;

	return instance;
}

DirectoryManager::DirectoryManager()
{
	// Fill out file exclusions list (files that cannot be selected for load)
	fileExclusions->insert("menu.rbf");
	fileExclusions->insert("boot.rom");
	fileExclusions->insert("MiSTer");

	// Windows service folders
	fileExclusions->insert("System Volume Information");
}

DirectoryManager::~DirectoryManager()
{
	if (fileExclusions != nullptr)
	{
		fileExclusions->clear();
		delete fileExclusions;
	}
}


// Public methods

DirectoryListPtr DirectoryManager::scanDirectory(
		string& folderPath,
		StringSet* supportedExtensions,
		bool includeFolders,
		bool withExtensions)
{
	return scanDirectory(folderPath.c_str(), supportedExtensions, includeFolders, withExtensions);
}

DirectoryListPtr DirectoryManager::scanDirectory(
		const char* folderPath,
		StringSet* supportedExtensions,
		bool includeFolders,
		bool withExtensions)
{
	static char fullPath[PATH_MAX + 1];

	DirectoryList* list = new DirectoryList();
	DirectoryListPtr result = DirectoryListPtr(list);

	// Create full absolute path
	snprintf(fullPath, sizeof(fullPath), "%s/%s", sysmanager::getDataRootDir(), folderPath);

	DIR *dir = opendir(fullPath);
	if (dir != nullptr)
	{
		struct dirent *de;
		while (true)
		{
			de = readdir(dir);
			if (de == nullptr)
				break;

			switch (de->d_type)
			{
				case DT_DIR:		// Current entry is folder
					if (strcmp(de->d_name, ".") == 0)
						continue;
					if (strcmp(de->d_name, "..") == 0)
						continue;

					if (includeFolders && isFileAllowed(de->d_name))
					{
						DirectoryEntry* item = new DirectoryEntry();
						item->isFolder = true;
						strlcpy(item->name, de->d_name, sizeof(item->name));
						strlcpy(item->displayname, de->d_name, sizeof(item->displayname));

						list->emplace_back(item);
					}
					break;
				case DT_REG:		// Current entry is regular file
					if (isFileAllowed(de->d_name))
					{
						if (isFileMatchExtension(de->d_name, supportedExtensions))
						{
							DirectoryEntry* item = new DirectoryEntry();
							strlcpy(item->name, de->d_name, sizeof(item->name));

							if (withExtensions)
							{
								strlcpy(item->displayname, de->d_name, sizeof(item->displayname));
							}
							else
							{
								strlcpy(item->displayname, filemanager::getName(de->d_name).c_str(), sizeof(item->displayname));
							}

							list->emplace_back(item);
						}
					}
					break;
				case DT_LNK:		// Current entry is symlink
					break;
				default:
					break;
			}
		}

		closedir(dir);
	}
	else
	{
		LOGERROR("Couldn't open dir: %s\n", fullPath);
	}

	return result;
}

// Helper methods

bool DirectoryManager::isFileAllowed(const string& filename)
{
	bool result = isFileAllowed(filename.c_str());

	return result;
}

bool DirectoryManager::isFileAllowed(const char *filename)
{
	bool result = true;

	if (filename == nullptr) // Filename cannot be NULL
	{
		result = false;
	}
	else if (strlen(filename) == 0) // filename cannot be empty
	{
		result = false;
	}
	else if (filename[0] == '.') // Mask hidden folders and files
	{
		result = false;
	}
	else
	{
		// Filenames from exclution list are also not allowed
		if (find_if(
				fileExclusions->begin(),
				fileExclusions->end(),
				[filename](const char* val)
				{
					return strcmp(val, filename) == 0;
				}) != fileExclusions->end()
			)
		{
			result = false;
		}
	}

	return result;
}

bool DirectoryManager::isFileMatchExtension(const string& filename, StringSet* extensions)
{
	bool result = isFileMatchExtension(filename.c_str(), extensions);

	return result;
}

bool DirectoryManager::isFileMatchExtension(const char *filename, StringSet* extensions)
{
	bool result = false;

	if (filename != nullptr && strlen(filename) > 0)
	{
		if (extensions != nullptr && extensions->size() > 0)
		{
			string ext = filemanager::getExtension(filename);
			const char* pExt = ext.c_str();

			// Find match in extensions set
			if (find_if(
					extensions->begin(),
					extensions->end(),
					[pExt](const char* val)
					{
						return strcasecmp(val, pExt) == 0;
					}) != extensions->end()
				)
			{
				// Extension matched
				result = true;
			}
		}
		else
		{
			// No extensions provided in filter
			// Allow any file
			result = true;
		}
	}

	return result;
}
