#ifndef SYSTEM_HDMI_HDMI_H_
#define SYSTEM_HDMI_HDMI_H_

#include "../../common/types.h"

// VESA-like videomode description
// See details: http://martin.hinner.info/vga/timing.html
// See details: http://tinyvga.com/vga-timing
struct HDMIVideoModeType
{
	uint32_t resWidth;		// Width in pixels
	uint32_t hFrontPorch;	// Left margin for scanline in pixels
	uint32_t hSync;			// Horizontal sync pulse duration in pixels
	uint32_t hBackPorch;		// Right margin for scanline in pixels
	uint32_t resHeight;		// Height in pixels
	uint32_t vFrontPorch;	// Top margin in lines
	uint32_t vSync;			// Vertical sync pulse duration in lines
	uint32_t vBackPorch;		// Bottom margin in lines
};
typedef struct HDMIVideoModeType HDMIVideoModeType;

struct HDMIVideoMode
{
	union
	{
		uint32_t vmodes[8];
		HDMIVideoModeType videoMode;
	};

	double freqPixelClock;	// Pixel clock frequency in MHz
};

struct HDMIVideoModePacketType
{
	uint32_t nVideoMode;		// 0 - custom video mode. [1:10] - standard video mode code.

	// Same fields and order as in HDMIVideoModeType
	// HDMIVideoModeType videoMode;
	uint32_t resWidth;		// Width in pixels
	uint32_t hFrontPorch;	// Left margin for scanline in pixels
	uint32_t hSync;			// Horizontal sync pulse duration in pixels
	uint32_t hBackPorch;		// Right margin for scanline in pixels
	uint32_t resHeight;		// Height in pixels
	uint32_t vFrontPorch;	// Top margin in lines
	uint32_t vSync;			// Vertical sync pulse duration in lines
	uint32_t vBackPorch;		// Bottom margin in lines

	uint32_t v9;
	uint32_t pllM;			// PLL M - Feedback counter multiply factor value
	uint32_t v11;
	uint32_t v12;
	uint32_t v13;
	uint32_t pllC;			// PLL C - Post-scale divide factor value
	uint32_t v15;
	uint32_t v16;
	uint32_t v17;
	uint32_t v18;
	uint32_t v19;
	uint32_t pllK;			// PLL K - Fractional multiply factor value
	uint32_t v21;
	uint32_t v22;
	uint32_t v23;
	uint32_t v24;
	uint32_t v25;
	uint32_t v26;
	uint32_t v27;
	uint32_t v28;
	uint32_t v29;
	uint32_t v30;
	uint32_t v31;
};
typedef struct HDMIVideoModePacketType HDMIVideoModePacketType;

union HDMIVideoModePacket
{
	uint32_t values[32];
	HDMIVideoModePacketType packet;
};
typedef union HDMIVideoModePacket HDMIVideoModePacket;

#endif /* SYSTEM_HDMI_HDMI_H_ */
