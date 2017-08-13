#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <unistd.h>
#include <limits.h>

#define OFF 0
#define ON 1

typedef struct
{
	int       fd;
	int       mode;
	__off64_t size;
	__off64_t offset;
	char      name[NAME_MAX + 1];
}  fileDescriptor;



#endif /* COMMON_TYPES_H_ */
