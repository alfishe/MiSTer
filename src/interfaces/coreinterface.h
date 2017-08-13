#ifndef INTERFACES_COREINTERFACE_H_
#define INTERFACES_COREINTERFACE_H_

class coreinterface
{
public:
	coreinterface();
	virtual ~coreinterface();

	virtual char* getCoreName() = 0;
};

#endif /* INTERFACES_COREINTERFACE_H_ */
