#ifndef IO_INPUT_MAPPERS_BASEMAPPER_H_
#define IO_INPUT_MAPPERS_BASEMAPPER_H_

#include <stdint.h>

class BaseMapper
{
public:
	virtual uint32_t getCode(uint16_t key) = 0;
};

#endif /* IO_INPUT_MAPPERS_BASEMAPPER_H_ */
