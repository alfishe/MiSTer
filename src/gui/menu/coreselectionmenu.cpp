#include "coreselectionmenu.h"

#include "../../common/logger/logger.h"

#include <algorithm>
#include "../../common/file/path/path.h"
#include "../../common/helpers/collectionhelper.h"

void CoreSelectionMenu::start()
{
	readAvailableCores();

	auto list = readAvailableCores();
	m_ctrlSelectionList->setDataSource(list);
}

void CoreSelectionMenu::stop()
{

}

DirectoryEntryVector& CoreSelectionMenu::getAvailableCores()
{
	return m_coreNames;
}

void CoreSelectionMenu::selectItem(int index)
{
	if ((int)m_coreNames.size() <= index)
	{
		m_selectedIndex = index;
		m_selectedItem = m_coreNames[index].name;
	}
}

void CoreSelectionMenu::removeSelection()
{
	m_selectedItem = "";
	m_selectedIndex = -1;
}

void CoreSelectionMenu::moveUp()
{
	m_ctrlSelectionList->moveUp();
}

void CoreSelectionMenu::moveDown()
{
	m_ctrlSelectionList->moveDown();
}

// Helper methods
ListItemVector CoreSelectionMenu::readAvailableCores()
{
	ListItemVector result;

	ScanDir scan;
	scan.scanFolder(DATA_ROOT, ScanDir::getFPGACoreFilter(), ScanDir::getAlphaSortCaseInsensitive());

	m_coreNames = scan.getScanResults();

	// Strip extensions from displayname property
	if (m_coreNames.size() > 0)
	{
		for_each(m_coreNames.begin(), m_coreNames.end(),
			[&](DirectoryEntry& entry)
			{
				entry.displayname = Path::getFileNameWithoutExtension(entry.displayname);

				ListItem item(entry.displayname, 0, false);
				result.push_back(item);
			}
		);
	}
	else
	{
		LOGINFO("No FPGA cores available \n");
	}

	// Free up ScanDir buffers
	scan.dispose();

	return result;
}


