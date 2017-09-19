#ifndef CORES_MINIMIG_MINIMIGCORE_H_
#define CORES_MINIMIG_MINIMIGCORE_H_

#include "../../interfaces/icoreinterface.h"

class MinimigCore: public ICoreInterface
{
public:
	MinimigCore();
	virtual ~MinimigCore();

	void translateKeypress(uint16_t key, bool pressed) override;
	void sendKey(uint16_t key, bool pressed) override;

	void translateMouseCoordinates(int16_t x, int16_t y, uint8_t buttons) override;

protected:
	// Minimig specific methods
	uint32_t getAmigaKeyCode(uint16_t key);
};

#endif /* CORES_MINIMIG_MINIMIGCORE_H_ */
