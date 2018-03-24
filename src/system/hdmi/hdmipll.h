#ifndef SYSTEM_HDMI_HDMIPLL_H_
#define SYSTEM_HDMI_HDMIPLL_H_

#include <string>
#include "../../common/types.h"
#include "hdmi.h"

using namespace std;

class HDMIPLL
{
protected:
	static HDMIVideoMode m_videoModes[];

public:
	static bool setStandardVideoMode(int idxVideoMode);

public:
	static bool isValidStandardVideoMode(int idxVideoMode);
	static bool isValidHDMIFrequency(double freq);
	static bool getPLL(double freqPixelclock, uint32_t *M, uint32_t *K, uint32_t *C);
	static uint32_t getPLLDivisor(uint32_t div);

	static HDMIVideoModePacket* getStandardVideoModePacket(int idxVideoMode);

	static void parseVideoMode(const string& mode);
	static void parseCustomVideoMode(const string& mode);

	static void setVideoMode(HDMIVideoModePacket* modePacket);
};


#endif /* SYSTEM_HDMI_HDMIPLL_H_ */

