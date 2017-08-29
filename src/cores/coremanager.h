#ifndef CORES_COREMANAGER_H_
#define CORES_COREMANAGER_H_

#include "../interfaces/icoreinterface.h"

class CoreManager
{
protected:
	// Fields
	ICoreInterface* currentCore = nullptr;

public:
	CoreManager& instance();

	bool loadCore(char* filename);

private:
	// Ensure class instance cannot be created directly
	CoreManager() {};
};

#endif /* CORES_COREMANAGER_H_ */
