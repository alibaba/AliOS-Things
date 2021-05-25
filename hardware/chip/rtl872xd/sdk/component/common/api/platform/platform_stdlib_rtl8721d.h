#ifndef PLATFORM_STDLIB_8721D_H
#define PLATFORM_STDLIB_8721D_H

#define CONFIG_PLATFORM_AMEBA_X 1

#if defined (__IARSTDLIB__)
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdint.h>
	#include <stdarg.h> /* va_list */
	#include "diag.h"

	#define strsep(str, delim)      	_strsep(str, delim)
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h> /* va_list */
	#include "diag.h"
	#include "strproc.h"
	#include "memproc.h"
	#include "basic_types.h"
	#include "rtl8721d.h"
	#include "rtl8721d_ram_libc.h"
#ifndef STD_PRINTF
	#undef printf
	#undef vsnprintf
	#undef sprintf
	#undef snprintf
	#undef sscanf
#endif
	#undef memchr
	#undef memcmp
	#undef memcpy
	#undef memset
	#undef memmove
	#undef strcmp
	#undef strcpy
	#undef strlen
	#undef strncmp
	#undef strncpy
	#undef strsep
	#undef strtok
	#undef strcat
	#undef strchr
	#undef strncat
	#undef strstr
	#undef atol
	#undef atoi
	#undef strpbrk
	#undef strtoul
	#undef strtol
	#undef rand
#ifndef STD_PRINTF
	#define printf						_rtl_printf
	#define sprintf						_rtl_sprintf
	#define snprintf					_rtl_snprintf			// NULL function
	#define vsnprintf					_rtl_vsnprintf
	#define sscanf						_rtl_sscanf	//if use sscanf in std libc.a, please delete _strtol_r symbol in rlx8721d_rom_symbol_acut.ld
#endif
	#define memchr					_memchr
	#define memcmp					_memcmp
	#define memcpy					_memcpy //memcpy_gdma(dst, src, sz)
	#define memmove				_memmove
	#define memset					_memset
	
	#define strchr(s, c)				_strchr(s, c)			// for B-cut ROM
	#define strcmp(str1, str2)			_strcmp(str1, str2)
	#define strcpy					_strcpy
	#define strlen					_strlen
	#define strsep(str, delim)			_strsep(str, delim)
	#define strstr(str1, str2)			_strstr(str1, str2)	// NULL function
	#define strtok(str, delim)			_strtok(str, delim)//_strsep(str, delim)
	#define strcat					_strcat
	
	#define strncmp(str1, str2, cnt)	_strncmp(str1, str2, cnt)
	#define strncpy(dest, src, count)	_strncpy(dest, src, count)
	#define strncat					_strncat

	#define strtoul(str, endp, base)		_strtoul(str, endp, base)
	#define strtol(str, endp, base)		_strtol(str, endp, base)

	#define atol(str)					_strtol(str,NULL,10)
	#define atoi(str)					_stratoi(str)	
	#define strpbrk(cs, ct)			_strpbrk(cs, ct)		// for B-cut ROM
	#define rand						Rand
	#define srand				
	
	//extern int _sscanf_patch(const char *buf, const char *fmt, ...);
	//#define sscanf					_sscanf_patch
	

#endif	// defined (__IARSTDLIB__)

extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );
#define malloc                  pvPortMalloc
#define free                    vPortFree
#define realloc			pvPortReAlloc
#define calloc			rtw_calloc


#endif // PLATFORM_STDLIB_8721D_H
