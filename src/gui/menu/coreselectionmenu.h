#ifndef GUI_MENU_CORESELECTIONMENU_H_
#define GUI_MENU_CORESELECTIONMENU_H_

#include <string>
#include <vector>
#include "../../common/consts.h"
#include "../../common/file/scandir/scandir.h"
#include "basemenu.h"
#include "controls/selectionlist.h"

using namespace std;

class CoreSelectionMenu : public BaseMenu
{
public:
	SelectionList* m_ctrlSelectionList;

	DirectoryEntryVector m_coreNames;
	string m_selectedItem;
	int m_selectedIndex;

public:
	CoreSelectionMenu()
	{
		m_ctrlSelectionList = new SelectionList(4, 0, 28, 16);
	};
	virtual ~CoreSelectionMenu()
	{
		if (m_ctrlSelectionList != nullptr)
		{
			delete m_ctrlSelectionList;
		}
	};

	void start();
	void stop();

	DirectoryEntryVector& getAvailableCores();
	void selectItem(int index);
	void removeSelection();

	// Control events
	void pageUp();
	void pageDown();
	void moveUp();
	void moveDown();
	void enter();
	void cancel();

// Helper methods
protected:
	ListItemVector readAvailableCores();
};

#endif /* GUI_MENU_CORESELECTIONMENU_H_ */
