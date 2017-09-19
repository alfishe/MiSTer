#ifndef INTERFACES_ICOREINTERFACE_H_
#define INTERFACES_ICOREINTERFACE_H_

#include <stdint.h>

class ICoreInterface
{
public:
	ICoreInterface();
	ICoreInterface(const ICoreInterface& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~ICoreInterface();

	// Core info methods
	virtual char* getCoreName() = 0;

	// Core lifetime events
	virtual bool init() = 0;
	virtual void finalize() = 0;

	// Keyboard handling
	virtual void translateKeypress(uint16_t key, bool pressed) = 0;
	virtual void sendKey(uint16_t key, bool pressed) = 0;

	// Mouse handling
	virtual void translateMouseCoordinates(int16_t x, int16_t y, uint8_t buttons) = 0;
};

#endif /* INTERFACES_ICOREINTERFACE_H_ */
