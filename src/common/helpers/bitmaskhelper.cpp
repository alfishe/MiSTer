#include "bitmaskhelper.h"

///
/// Returns new value calculated as (prevValue OR bitsToSet) AND bitsToReset
/// Where:
///   bitsToSet = newValue AND mask
///   bitsToReset = newValue XOR mask
uint32_t BitMaskHelper::setMaskedValue(uint32_t prevValue, uint32_t newValue, uint32_t mask)
{
	uint32_t result = 0;

	uint32_t bitsToReset = newValue ^ mask;
	uint32_t bitsToSet = newValue & mask;

	result = (prevValue | bitsToSet) & ~bitsToReset;

	return result;
}
