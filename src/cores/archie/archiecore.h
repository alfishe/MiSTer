#ifndef CORES_ARCHIE_ARCHIECORE_H_
#define CORES_ARCHIE_ARCHIECORE_H_

#include "../../interfaces/icoreinterface.h"

class ArchieCore: public ICoreInterface
{
public:
	ArchieCore();
	virtual ~ArchieCore();

	void translateKeypress(uint16_t key, bool pressed) override;
	void sendKey(uint16_t key, bool pressed) override;

	void translateMouseCoordinates(int16_t x, int16_t y, uint8_t buttons) override;
};

#endif /* CORES_ARCHIE_ARCHIECORE_H_ */
