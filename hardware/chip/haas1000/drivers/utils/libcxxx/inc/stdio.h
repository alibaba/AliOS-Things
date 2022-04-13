#ifndef STDIO_H
#define STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"

/* Standard sprintf() function. Work as the libc one. */
int sprintf(char * buf, const char *fmt, ...);
/* Standard snprintf() function from BSD, more secure... */
int snprintf(char * buf, size_t size, const char *fmt, ...);
/* Standard sscanf() function. Work as the libc one. */
int sscanf(const char * buf, const char * fmt, ...);
/* If you need to code your own printf... */
int vsprintf(char *buf, const char *fmt, va_list ap);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int vsscanf(const char *fp, const char *fmt0, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */



