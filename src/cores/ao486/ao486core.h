#ifndef CORES_AO486_AO486CORE_H_
#define CORES_AO486_AO486CORE_H_

#include "../../interfaces/coreinterface.h"

class ao486core: public coreinterface
{
public:
	ao486core();
	virtual ~ao486core();

	virtual bool init() override;
	virtual void finalize() override;
};

#endif /* CORES_AO486_AO486CORE_H_ */
