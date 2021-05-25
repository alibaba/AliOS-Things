/*
 * rtl8721d_ram_libc.h
 *
 * Definitions for RTL library functions
 */
 
#ifndef _RTL8721D_RAM_LIBC_H_
#define	_RTL8721D_RAM_LIBC_H_

#include <basic_types.h>
#include <diag.h>
#include <va_list.h>

extern int _rtl_vsprintf(char *buf, size_t size, const char *fmt, const int *dp);
extern int _rtl_printf(IN const char* fmt, ...);
extern int _rtl_sprintf(char* str, const char* fmt, ...);
extern int _rtl_snprintf(char* str, size_t size, const char* fmt, ...);
extern int _rtl_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
extern int _rtl_sscanf(const char *buf, const char *fmt, ...);
#endif /* _RTL8721D_RAM_LIBC_H_ */

