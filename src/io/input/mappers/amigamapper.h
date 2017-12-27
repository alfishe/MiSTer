#ifndef IO_INPUT_MAPPERS_AMIGAMAPPER_H_
#define IO_INPUT_MAPPERS_AMIGAMAPPER_H_

#include <stdint.h>

#include "basemapper.h"

class AmigaMapper: public BaseMapper
{
protected:
	static const uint32_t map[];

public:
	uint32_t getCode(uint16_t key);
};

#endif /* IO_INPUT_MAPPERS_AMIGAMAPPER_H_ */
