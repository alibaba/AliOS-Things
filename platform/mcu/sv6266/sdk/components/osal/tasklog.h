#ifndef _TASKLOG_H
#define _TASKLOG_H

#include <stdarg.h>	/* va_list, va_arg() */

void tasklog_init(void);
void tasklog_printf(const char *fmt, ...);

#endif
