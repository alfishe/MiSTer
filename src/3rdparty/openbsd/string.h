#ifndef _3RDPARTY_OPENBSD_STRING_H_
#define _3RDPARTY_OPENBSD_STRING_H_

#include <sys/types.h>

#if defined (__cplusplus)
extern "C" {
#endif

size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);

#if defined (__cplusplus)
}
#endif

#endif /* _3RDPARTY_OPENBSD_STRING_H_ */
