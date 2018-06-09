#include "coreselectionmenu.h"

#include "../../common/logger/logger.h"

#include <algorithm>
#include "../../common/file/path/path.h"
#include "../../common/helpers/collectionhelper.h"
#include "../osd/osd.h"
#include "../../cores/coremanager.h"
#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgacommand.h"
#include "../../system/hdmi/hdmipll.h"

void CoreSelectionMenu::start()
{
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

// Control events
void CoreSelectionMenu::pageUp()
{
	m_ctrlSelectionList->pageUp();
}

void CoreSelectionMenu::pageDown()
{
	m_ctrlSelectionList->pageDown();
}

void CoreSelectionMenu::moveUp()
{
	m_ctrlSelectionList->moveUp();
}

void CoreSelectionMenu::moveDown()
{
	m_ctrlSelectionList->moveDown();
}

void CoreSelectionMenu::enter()
{
	m_ctrlSelectionList->enter();

	int selectedIndex = m_ctrlSelectionList->getSelectedIndex();

	if (selectedIndex >= 0)
	{
		// Close OSD now as the new core may not even have one
		OSD::instance().hide();

		const DirectoryEntry& item = m_coreNames[selectedIndex];
		const string filename = item.name;

		// Load FPGA core from file
		CoreManager::instance().loadCore(filename);

		// Post-load and core set-up activities
		FPGACommand& command = *FPGADevice::instance().command;

		uint8_t ioSize = command.getFPGAIOSize();
		uint8_t ioVersion = command.getFPGAIOVersion();
		LOGINFO("ioSize: %d, ioVersion: %d", ioSize, ioVersion);

		// 1. Put core into reset state
		uint32_t mask = UIO_STATUS_RESET;
		command.setStatus(UIO_STATUS_RESET, mask);
		//command.setStatus32(UIO_STATUS_RESET);

		// 2. Pass video mode parameters (VESA + HDMI PLL)
		HDMIPLL::setStandardVideoMode(8); // FullHD 1920x1080 @ 60Hz
		//HDMIPLL::setStandardVideoMode(0); // 1280x720 @ 60 Hz

		// 3. Send UIO_BUT_SW command after all 8 VESA params sent to FPGA side to have settings applied (works as latch)
		command.sendIOCommand(UIO_BUT_SW, (uint16_t)0);

		// 4. [Optional] step to apply saved settings and ROM files for the core

		// 5. Send status bit change sequence to release core from reset
		//command.setStatus(0, mask);
		command.setStatus32(0, mask);

		// Control if video mode set successfully
		LOGINFO("%s", command.getVideoMode().c_str());
	}
}

void CoreSelectionMenu::cancel()
{
	m_ctrlSelectionList->cancel();
}

// Helper methods
ListItemVector CoreSelectionMenu::readAvailableCores()
{
	ListItemVector result;

	// Get all .rbf cores (except menu.rbf), sorted alphabetically in ascending order
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

		LOGINFO("%d cores available", result.size());
	}
	else
	{
		LOGINFO("No FPGA cores available \n");
	}

	// Free up ScanDir buffers
	scan.dispose();

	return result;
}


