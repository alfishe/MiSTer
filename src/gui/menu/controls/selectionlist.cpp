#include "selectionlist.h"

#include <math.h>
#include "../../osd/osd.h"

void SelectionList::setDataSource(const ListItemVector& data)
{
	// Make local copy of list
	m_data = data;

	int len = m_data.size();

	// Calculate scroll bar parameters
	if (len > m_height)
	{
		m_showScrollBar = true;

		// Calculate scroll bar height
		// Step 1: Calculate ratio between control height and number of records to display
		// Step 2: Set scroll bar progress height as height * ratio
		// Example:
		//     Control height: 10 lines
		//     Data records:   15
		//     ratio: 10 / 15 = 0.66
		//     scroll bar progress height: 10 * 0.66 = 7 lines
		double ratio = (double)m_height / (double)len;
		m_scrollBarHeight = (int)(ceil((double)m_height * ratio));

		// Set scroll progress on top position (coordinates relative to SelectionList)
		m_scrollBarTop = 0;
	}
	else
	{
		m_showScrollBar = false;
		m_scrollBarTop = -1;
		m_scrollBarHeight = 0;
		m_topIndex = -1;
	}

	// Select first record if available
	if (m_data.size() > 0)
	{
		m_selectedIndex = 0;
		m_topIndex = 0;
	}
	else
	{
		m_selectedIndex = -1;
		m_topIndex = -1;
	}

	drawContent();
}

int SelectionList::getSelectedIndex()
{
	return m_selectedIndex;
}

const ListItem* SelectionList::getSelectedItem()
{
	ListItem* result = nullptr;

	if (m_selectedIndex > 0 && m_selectedIndex < (int)m_data.size())
	{
		result = &m_data[m_selectedIndex];
	}

	return result;
}

// Control methods
void SelectionList::moveUp()
{
	if (m_selectedIndex > 0)
	{
		m_selectedIndex--;
	}

	drawContent();
}

void SelectionList::moveDown()
{
	if (m_selectedIndex < m_height && m_selectedIndex < m_data.size())
	{
		m_selectedIndex++;
	}

	drawContent();
}

// Helper methods
void SelectionList::drawContent()
{
	// Check if we can draw something
	if (m_data.size() == 0)
	{
		return;
	}

	int allowedWidth = m_width;
	if (m_showScrollBar)
	{
		allowedWidth -= 1;
	}

	OSD& osd = OSD::instance();
	/*
	osd.clear();
	for (uint8_t x = 0; x < 100; x++)
		osd.setPixel(x, 80);
	osd.compose();
	return;
	*/

	if (m_topIndex >= 0)
	{
		for (uint8_t row = 0; row < m_height && row < m_data.size(); row++)
		{
			auto& item = m_data[m_topIndex + row];
			bool isItemHighlighted =  m_selectedIndex == row;
			int length = item.name.size();

			// Print item text
			for (uint8_t col = 0; col < length && m_left + col < allowedWidth; col++)
			{
				//osd.printSymbol(m_top + row, m_left + col, item.name[col], isItemHighlighted);
				osd.printSymbol(m_top + row, m_left + col, item.name[col]);
			}

			// If we have space left and it's highlighted menu item - fill with pixels
			/*if (isItemHighlighted && length < allowedWidth)
			{
				uint8_t left = (m_left + length) * 8;
				uint8_t top = (m_top + row) * 8;
				uint8_t width = (allowedWidth - length) * 8;
				uint8_t height = 8;

				osd.fillRect(left, top, width, height);
			}*/

			if (isItemHighlighted)
			{
				uint8_t left = m_left * 8;
				uint8_t top = (m_top + row) * 8;
				uint8_t width = allowedWidth * 8;
				uint8_t height = 8;

				osd.invertRectOptimized(left, top, width, height);
			}
		}

		// Debug - should be called when all controls alread drawn from main GUI module
		osd.compose();
	}
}

void SelectionList::drawScrollBar()
{
	if (!m_showScrollBar || m_selectedIndex < 0)
		return;

	// Fill out with empty texture everything above progress bar
	for (int i = 0; i < m_scrollBarTop && i < m_height; i++)
	{
		// TODO: Fill OSD rectangle with empty pattern
	}

	// Draw scrollbar progress
	for (int i = m_scrollBarTop; i < (m_scrollBarTop + m_scrollBarHeight) && i < m_height; i++)
	{
		// TODO: Fill OSD rectangle with coordinates [m_Left + m_Width - 1; i]
	}

	// Fill out with empty texture everything above progress bar
	for (int i = m_scrollBarTop + m_scrollBarHeight; i <  m_height; i++)
	{
		// TODO: Fill OSD rectangle with empty pattern
	}
}
