#ifndef INTERFACES_COREINTERFACE_H_
#define INTERFACES_COREINTERFACE_H_

class coreinterface
{
public:
	coreinterface();
	virtual ~coreinterface();

	// Core info methods
	virtual char* getCoreName() = 0;

	// Core lifetime events
	virtual bool init() = 0;
	virtual void finalize() = 0;
};

#endif /* INTERFACES_COREINTERFACE_H_ */
