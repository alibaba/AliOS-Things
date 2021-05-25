/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _STRPROC_H_
#define _STRPROC_H_

#include <stddef.h> /* for size_t */
#include <stdarg.h>
#include "platform_autoconf.h"
#include "basic_types.h"

#ifndef isprint
#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
//#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == ',')

#define isupper(c)	(((c)>='A')&&((c)<='Z'))
#define islower(c)	(((c)>='a')&&((c)<='z'))
#define isalpha(c)	(isupper(c) || islower(c))	
#endif  


extern _LONG_CALL_ int _vsscanf(const char *buf, const char *fmt, va_list args);


extern _LONG_CALL_ SIZE_T _strlen(const char *s);
extern _LONG_CALL_ int _strcmp(const char *cs, const char *ct);
extern _LONG_CALL_ char *_strncpy(char *dest, const char *src, size_t count);
extern _LONG_CALL_ char *_strcpy(char *dest, const char *src);
extern _LONG_CALL_ size_t _strlen(const char *s);
extern _LONG_CALL_ size_t _strnlen(const char *s, size_t count);
extern _LONG_CALL_ int _strncmp(const char *cs, const char *ct, size_t count);
extern _LONG_CALL_ int _sscanf(const char *buf, const char *fmt, ...);
extern _LONG_CALL_ char *_strsep(char **s, const char *ct);

extern _LONG_CALL_ char *_strpbrk(const char *cs, const char *ct);
extern _LONG_CALL_ char *_strchr(const char *s, int c);
extern _LONG_CALL_ int _stricmp(const char* str1, const char* str2);
extern _LONG_CALL_ u8* _strupr(IN  u8  *string);
extern _LONG_CALL_ int _stratoi(IN const char * s);
extern _LONG_CALL_  char * _strstr(IN const char * str1, IN const char * str2);
extern _LONG_CALL_ char* _strtok(IN char *str, IN const char* delim);

extern _LONG_CALL_ long _strtol(const char *cp, char **endp, int base);
extern _LONG_CALL_ unsigned long _strtoul(const char *cp, char **endp, int base);
extern _LONG_CALL_ long long _strtoll(const char *cp, char **endp, unsigned int base);
extern _LONG_CALL_ unsigned long long _strtoull(const char *cp, char **endp, unsigned int base);

extern _LONG_CALL_ u8 _char2num(u8 ch);
extern _LONG_CALL_ u8 _2char2dec(u8 hch, u8 lch);
extern _LONG_CALL_ u8 _2char2hex(u8 hch, u8 lch);

#if 0
/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
static inline char _tolower(const char c)
{
    return c | 0x20;
}
#endif
/* Fast check for octal digit */
static inline int isodigit(const char c)
{
    return c >= '0' && c <= '7';
}

#endif
