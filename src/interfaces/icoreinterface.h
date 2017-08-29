#ifndef INTERFACES_ICOREINTERFACE_H_
#define INTERFACES_ICOREINTERFACE_H_

class ICoreInterface
{
public:
	ICoreInterface();
	virtual ~ICoreInterface();

	// Core info methods
	virtual char* getCoreName() = 0;

	// Core lifetime events
	virtual bool init() = 0;
	virtual void finalize() = 0;
};

#endif /* INTERFACES_ICOREINTERFACE_H_ */
