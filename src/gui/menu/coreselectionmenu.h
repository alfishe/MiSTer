#ifndef GUI_MENU_CORESELECTIONMENU_H_
#define GUI_MENU_CORESELECTIONMENU_H_

#include <string>
#include <vector>
#include "../../common/consts.h"
#include "../../common/file/scandir/scandir.h"

using namespace std;

class CoreSelectionMenu
{
public:
	DirectoryEntryVector m_coreNames;
	string m_selectedItem;
	int m_selectedIndex;

public:
	CoreSelectionMenu() {};
	virtual ~CoreSelectionMenu() {};

	void readAvailableCores();
	DirectoryEntryVector& getAvailableCores();
	void selectItem(int index);
	void removeSelection();
};

#endif /* GUI_MENU_CORESELECTIONMENU_H_ */
