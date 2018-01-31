#ifndef GUI_MENU_CONTROLS_SELECTIONLIST_H_
#define GUI_MENU_CONTROLS_SELECTIONLIST_H_

#include <string>
#include <vector>

using namespace std;

struct ListItem
{
	// Item name to display
	string name;

	// Associated value
	int value;

	// Enabled / disabled flag (Disabled items cannot be selected and grayed-out visually)
	bool disabled = false;

	ListItem() {};
	ListItem(const string& name, int value, bool disabled): name(name), value(value), disabled(disabled) {};
};
typedef struct ListItem ListItem;
typedef vector<ListItem> ListItemVector;

class SelectionList
{
protected:
	// Control boundaries
	const int m_left;
	const int m_top;
	const int m_width;
	const int m_height;

	// DataSource
	ListItemVector m_data;
	int m_selectedIndex = -1;

	// Context
	int m_topIndex = -1;				// What record displayed in the very first line
	int m_selectedOnScreenPos = -1;	// What line is currently highlighted on-screen (y-pos in lines)

	// Parameters to show scroll bar
	bool m_showScrollBar = false;
	int m_scrollBarTop = -1;			// Coordinates relative to SelectionList
	int m_scrollBarHeight = 0;

// Constructors
public:
	SelectionList(int left, int top, int width, int height) : m_left(left), m_top(top), m_width(width), m_height(height) {};

// Public methods
public:
	void setDataSource(const ListItemVector& data);
	int getSelectedIndex();
	const ListItem* getSelectedItem();

// Control methods
public:
	void moveUp();
	void moveDown();

// Helper methods
protected:
	void drawContent();
	void drawScrollBar();
	void removeSelectedHighlight();
	void recalcPosition();
};

#endif /* GUI_MENU_CONTROLS_SELECTIONLIST_H_ */
