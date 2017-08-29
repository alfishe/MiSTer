#ifndef CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_
#define CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_

#include "../../interfaces/icoreinterface.h"

class newtemplatecore: public ICoreInterface
{
public:
	newtemplatecore();
	virtual ~newtemplatecore();

	virtual bool init() override;
	virtual void finalize() override;
};

#endif /* CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_ */
