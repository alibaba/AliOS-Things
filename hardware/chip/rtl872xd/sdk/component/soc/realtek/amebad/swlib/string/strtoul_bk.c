#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"

/* Minimum and maximum values a `signed long int' can hold.
   (Same as `int').  */
#ifndef __LONG_MAX__
#if defined (__alpha__) || (defined (__sparc__) && defined(__arch64__)) || defined (__sparcv9) || defined (__s390x__)
#define __LONG_MAX__ 9223372036854775807L
#else
#define __LONG_MAX__ 2147483647L
#endif /* __alpha__ || sparc64 */
#endif
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif

HAL_ROM_TEXT_SECTION
static u32 Isspace(char ch)
{
    return (u32)((ch - 9) < 5u  ||  ch == ' ');
}

/**
 * _strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoul instead.
 */
HAL_ROM_TEXT_SECTION
_LONG_CALL_ unsigned long _strtoul(const char *cp, char **endp, unsigned int base)
{
        return _strtoull(cp, endp, base);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_ 
u32 Strtoul(const u8 *nptr,  u8 **endptr,  u32 base)
{
	u32 v=0;

	while(Isspace(*nptr)) ++nptr;
	if (*nptr == '+') ++nptr;
	if (base==16 && nptr[0]=='0') goto skip0x;
	if (!base) {
		if (*nptr=='0') {
			base=8;
skip0x:
			if (nptr[1]=='x'||nptr[1]=='X') {
				nptr+=2;
				base=16;
			}
		} else
			base=10;
	}
	while(*nptr) {
		register u8 c=*nptr;
		c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
		if (c>=base) break;
		{
			register u32 w=v*base;
			if (w<v) {
				//errno=ERANGE;
				return ULONG_MAX;
			}
			v=w+c;
		}
		++nptr;
	}
	if (endptr) *endptr=(u8 *)nptr;
	//errno=0;    /* in case v==ULONG_MAX, ugh! */
	return v;
}