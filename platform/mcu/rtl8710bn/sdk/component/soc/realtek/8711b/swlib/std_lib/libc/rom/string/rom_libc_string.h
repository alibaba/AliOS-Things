/*
 * rom_libc_string.h
 *
 * Definitions for standard library - libc functions.
 */
#ifndef _ROM_LIBC_STRING_H_
#define	_ROM_LIBC_STRING_H_

#include <basic_types.h>

#define __rtl_memchr			__rtl_memchr_v1_00
#define __rtl_memcmp			__rtl_memcmp_v1_00
#define __rtl_memcpy			__rtl_memcpy_v1_00
#define __rtl_memmove			__rtl_memmove_v1_00
#define __rtl_memset			__rtl_memset_v1_00
#define __rtl_strcat			__rtl_strcat_v1_00
#define __rtl_strchr			__rtl_strchr_v1_00
#define __rtl_strcmp			__rtl_strcmp_v1_00
#define __rtl_strcpy			__rtl_strcpy_v1_00
#define __rtl_strlen			__rtl_strlen_v1_00
#define __rtl_strncat			__rtl_strncat_v1_00
#define __rtl_strncmp			__rtl_strncmp_v1_00
#define __rtl_strncpy			__rtl_strncpy_v1_00
#define __rtl_strstr			__rtl_strstr_v1_00
#define __rtl_strsep			__rtl_strsep_v1_00
#define __rtl_strtok			__rtl_strtok_v1_00

#define __rtl_critical_factorization	__rtl_critical_factorization_v1_00
#define __rtl_two_way_short_needle	__rtl_two_way_short_needle_v1_00
#define __rtl_two_way_long_needle		__rtl_two_way_long_needle_v1_00

extern _LONG_CALL_ void * __rtl_memchr_v1_00(const void * src_void , int c , size_t length);
extern _LONG_CALL_ int __rtl_memcmp_v1_00(const void * m1 , const void * m2 , size_t n);
extern _LONG_CALL_ void * __rtl_memcpy_v1_00(void * __restrict dst0 , const void * __restrict src0 , size_t len0);
extern _LONG_CALL_ void * __rtl_memmove_v1_00( void * dst_void , const void * src_void , size_t length);
extern _LONG_CALL_ void * __rtl_memset_v1_00(void * m , int c , size_t n);
extern _LONG_CALL_ char * __rtl_strcat_v1_00(char *__restrict s1 , const char *__restrict s2);
extern _LONG_CALL_ char * __rtl_strchr_v1_00(const char *s1 , int i);
extern _LONG_CALL_ int __rtl_strcmp_v1_00(const char *s1 ,	const char *s2);
extern _LONG_CALL_ char* __rtl_strcpy_v1_00(char *dst0 , const char *src0);
extern _LONG_CALL_ size_t __rtl_strlen_v1_00(const char *str);
extern _LONG_CALL_ char * __rtl_strncat_v1_00(char *__restrict s1 , const char *__restrict s2 , size_t n);
extern _LONG_CALL_ int __rtl_strncmp_v1_00(const char *s1 , const char *s2 , size_t n);
extern _LONG_CALL_ char * __rtl_strncpy_v1_00(char *__restrict dst0 , const char *__restrict src0 , size_t count);
extern _LONG_CALL_ char * __rtl_strstr_v1_00(const char *searchee , const char *lookfor);
extern _LONG_CALL_ char * __rtl_strsep_v1_00(register char **source_ptr , register const char *delim);
extern _LONG_CALL_ char * __rtl_strtok_v1_00(register char *__restrict s , register const char *__restrict delim);

#endif /* _ROM_LIBC_STRING_H_ */
