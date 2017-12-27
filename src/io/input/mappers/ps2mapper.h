#ifndef IO_INPUT_MAPPERS_PS2MAPPER_H_
#define IO_INPUT_MAPPERS_PS2MAPPER_H_

#include <stdint.h>
#include "basemapper.h"

class PS2Mapper: public BaseMapper
{
protected:
	static const uint32_t map[];

public:
	uint32_t getCode(uint16_t key);
};

#endif /* IO_INPUT_MAPPERS_PS2MAPPER_H_ */
