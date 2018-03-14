#ifndef SYSTEM_HDMI_HDMI_H_
#define SYSTEM_HDMI_HDMI_H_

#include <utility>
#include <vector>
#include "../../common/types.h"

using namespace std;

typedef pair<uint16_t, uint32_t> PLLPortType;
typedef vector<PLLPortType> PLLPortVector;

// VESA-like videomode description
// See details: http://martin.hinner.info/vga/timing.html
// See details: http://tinyvga.com/vga-timing
struct HDMIVideoModeType
{
	uint16_t resWidth;		// Width in pixels
	uint16_t hFrontPorch;	// Left margin for scanline in pixels
	uint16_t hSync;			// Horizontal sync pulse duration in pixels
	uint16_t hBackPorch;		// Right margin for scanline in pixels
	uint16_t resHeight;		// Height in pixels
	uint16_t vFrontPorch;	// Top margin in lines
	uint16_t vSync;			// Vertical sync pulse duration in lines
	uint16_t vBackPorch;		// Bottom margin in lines
};
typedef struct HDMIVideoModeType HDMIVideoModeType;

struct HDMIVideoMode
{
	union
	{
		uint16_t vmodes[8];
		HDMIVideoModeType videoMode;
	};

	double freqPixelClock;	// Pixel clock frequency in MHz
};

struct HDMIVideoModePacket
{
	uint8_t nVideoMode;		// 0 - custom video mode. [1:10] - standard video mode code.

	HDMIVideoMode videoMode;

	PLLPortVector pllRegisters;
};
typedef struct HDMIVideoModePacket HDMIVideoModePacket;

#endif /* SYSTEM_HDMI_HDMI_H_ */
