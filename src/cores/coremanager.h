#ifndef CORES_COREMANAGER_H_
#define CORES_COREMANAGER_H_

#include <string>
#include "../interfaces/icoreinterface.h"
#include "../fpga/fpgacommand.h"

using namespace std;

class CoreManager
{
protected:
	// Fields
	ICoreInterface* currentCore = nullptr;

public:
	static CoreManager& instance();
	CoreManager(const CoreManager& that) = delete; // Copy constructor is forbidden here (C++11 feature)

	bool loadCore(const char* filename);
	CoreType getCoreType();
	const string getCoreName();
	bool isMenuCore();

	ICoreInterface* getCurrentCore();

private:
	// Ensure class instance cannot be created directly
	CoreManager() {};
};

#endif /* CORES_COREMANAGER_H_ */
