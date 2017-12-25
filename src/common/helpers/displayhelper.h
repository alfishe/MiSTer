#ifndef COMMON_HELPERS_DISPLAYHELPER_H_
#define COMMON_HELPERS_DISPLAYHELPER_H_

#include <inttypes.h>
#include <string>
#include "../../3rdparty/tinyformat/tinyformat.h"

using namespace std;

class DisplayHelper
{
public:
	static string formatSize(uint64_t size, bool si = false);

	static string formatBits(uint8_t value);
	static string formatBits(uint16_t value);
	static string formatBits(uint32_t value);


private:
	DisplayHelper() {};
};

#endif /* COMMON_HELPERS_DISPLAYHELPER_H_ */
