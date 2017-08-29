#ifndef CORES_AO486_AO486CORE_H_
#define CORES_AO486_AO486CORE_H_

#include "../../interfaces/icoreinterface.h"

class ao486core: public ICoreInterface
{
public:
	ao486core();
	virtual ~ao486core();

	virtual bool init() override;
	virtual void finalize() override;
};

#endif /* CORES_AO486_AO486CORE_H_ */
