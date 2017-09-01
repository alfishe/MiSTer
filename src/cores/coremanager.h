#ifndef CORES_COREMANAGER_H_
#define CORES_COREMANAGER_H_

#include "../interfaces/icoreinterface.h"

class CoreManager
{
protected:
	// Fields
	ICoreInterface* currentCore = nullptr;

public:
	static CoreManager& instance();
	CoreManager(const CoreManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)

	bool loadCore(const char* filename);

private:
	// Ensure class instance cannot be created directly
	CoreManager() {};
};

#endif /* CORES_COREMANAGER_H_ */
