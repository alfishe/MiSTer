#ifndef COMMON_HELPERS_BITMASKHELPER_H_
#define COMMON_HELPERS_BITMASKHELPER_H_

#include "../types.h"

class BitMaskHelper
{
public:
	static uint32_t setMaskedValue(uint32_t prevValue, uint32_t newValue, uint32_t mask);
};

#endif /* COMMON_HELPERS_BITMASKHELPER_H_ */
