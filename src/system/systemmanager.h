#ifndef SYSTEM_SYSTEMMANAGER_H_
#define SYSTEM_SYSTEMMANAGER_H_

#include <string>
#include "../common/types.h"

using namespace std;

class SystemManager
{
public:
	static SystemManager& instance();							// Singleton instance
	SystemManager(SystemManager&&) = delete;						// Disable move constructor (C++11 feature)
	SystemManager(const SystemManager& that) = delete; 			// Disable copy constructor (C++11 feature)
	SystemManager& operator =(SystemManager const&) = delete;		// Disable assignment operator (C++11 feature)
private:
	SystemManager() {};

// Helper methods
public:
	static uint32_t getBoardIPAddr();
	static string getBoardIP();
};

#endif /* SYSTEM_SYSTEMMANAGER_H_ */
