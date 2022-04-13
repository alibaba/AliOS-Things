#ifndef __DEBUG_PRINT_H
#define __DEBUG_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

int printk_direct(const char *fmt, ...);

/* for ulog encoder*/
int print_driver(const char *fmt, va_list ap, unsigned int buf[]);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_PRINT_H */
