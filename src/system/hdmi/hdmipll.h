#ifndef SYSTEM_HDMI_HDMIPLL_H_
#define SYSTEM_HDMI_HDMIPLL_H_

#include <string>
#include "../../common/types.h"

using namespace std;

struct VideoMode
{
	uint32_t videoParams[8];
	double freqPixelClock;
};
typedef struct VideoMode VideoMode;

class HDMIPLL
{
protected:
	static VideoMode m_videoModes[8];

public:
	static bool getPLL(double freqPixelclock, uint32_t *M, uint32_t *K, uint32_t *C);
	static uint32_t getPLLDivisor(uint32_t div);

	static void parseVideoMode(const string& mode);
	static void parseCustomVideoMode(const string& mode);
};


#endif /* SYSTEM_HDMI_HDMIPLL_H_ */

