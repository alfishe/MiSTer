#ifndef CORES_COREMANAGER_H_
#define CORES_COREMANAGER_H_

#include <string>
#include "../common/events/events.h"
#include "../interfaces/icoreinterface.h"
#include "../fpga/fpgacommand.h"

using namespace std;

class CoreManager: public EventSource
{
protected:
	// Fields
	ICoreInterface* currentCore = nullptr;

public:
	static CoreManager& instance();
	CoreManager(CoreManager&&) = delete;						// Disable move constructor (C++11 feature)
	CoreManager(const CoreManager& that) = delete; 			// Disable copy constructor (C++11 feature)
	CoreManager& operator =(CoreManager const&) = delete;		// Disable assignment operator (C++11 feature)

	bool loadCore(const string& filename);
	CoreType getCoreType();
	const string getCoreName();
	bool isMenuCore();

	ICoreInterface* getCurrentCore();

private:
	// Ensure class instance cannot be created directly
	CoreManager() {};
};

#endif /* CORES_COREMANAGER_H_ */
