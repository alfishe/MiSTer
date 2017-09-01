#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <unistd.h>
#include <limits.h>
#include "consts.h"

#define OFF 0
#define ON 1

typedef struct
{
	int       fd = INVALID_FILE_DESCRIPTOR;
	int       mode;
	__off64_t size;
	__off64_t offset;
	char      name[PATH_MAX + 1];

	// New attributes
	bool		cacheable;
	bool		readonly;
}  FileDescriptor;



#endif /* COMMON_TYPES_H_ */
