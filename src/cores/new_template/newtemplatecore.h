#ifndef CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_
#define CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_

#include "../../interfaces/coreinterface.h"

class newtemplatecore: public coreinterface
{
public:
	newtemplatecore();
	virtual ~newtemplatecore();

	virtual bool init() override;
	virtual void finalize() override;
};

#endif /* CORES_NEW_TEMPLATE_NEWTEMPLATECORE_H_ */
