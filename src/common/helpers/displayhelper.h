#ifndef COMMON_HELPERS_DISPLAYHELPER_H_
#define COMMON_HELPERS_DISPLAYHELPER_H_

#include <inttypes.h>

class displayhelper
{
public:
	static char* formatSize(uint64_t size, bool si = false);

private:
	displayhelper() {};
};

#endif /* COMMON_HELPERS_DISPLAYHELPER_H_ */
