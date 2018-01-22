#include "coreselectionmenu.h"

#include "../../common/logger/logger.h"

#include <algorithm>
#include "../../common/helpers/collectionhelper.h"

void CoreSelectionMenu::readAvailableCores()
{
	ScanDir scan;
	scan.scanFolder(DATA_ROOT, ScanDir::getFPGACoreFilter(), ScanDir::getAlphaSortCaseInsensitive());

	m_coreNames = scan.getScanResults();
	if (m_coreNames.size() > 0)
	{
		for_each(m_coreNames.begin(), m_coreNames.end(),
			[&](const DirectoryEntry& entry)
			{

			}
		);
	}
	else
	{
		LOGINFO("No FPGA cores available \n");
	}

	// Free up ScanDir buffers
	scan.dispose();
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
