#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#define USHRT_MAX       ((u16)(~0U))
#define SHRT_MAX        ((s16)(USHRT_MAX>>1))
#define ULLONG_MAX      (~0ULL)
#define STR_STORE_MAX_LEN		24
#define KSTRTOX_OVERFLOW        (1U << 31)

/**
 * div_u64_rem - unsigned 64bit divide with 32bit divisor with remainder
 *
 * This is commonly provided by 32bit archs to provide an optimized 64bit
 * divide.
 */
//static inline 
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static u64 div_u64_rem(u64 dividend, u32 divisor, u32 *remainder)
{
        *remainder = (u32)dividend % divisor;
        return (u32)dividend / divisor;
}

/**
 * div_s64_rem - signed 64bit divide with 32bit divisor with remainder
 */
//static inline 
//LIBC_ROM_TEXT_SECTION
//_LONG_CALL_ static s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder)
//{
//        *remainder = (s32)dividend % divisor;
//        return (s32)dividend / divisor;
//}

/**
 * div_u64 - unsigned 64bit divide with 32bit divisor
 *
 * This is the most common 64bit divide and should be used if possible,
 * as many 32bit archs can optimize this variant better than a full 64bit
 * divide.
 */
#ifndef div_u64
//static inline 
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static u64 div_u64(u64 dividend, u32 divisor)
{
        u32 remainder;
        return div_u64_rem(dividend, divisor, &remainder);
}
#endif

/**
 * div_s64 - signed 64bit divide with 32bit divisor
 */
#ifndef div_s64
//static inline 
//LIBC_ROM_TEXT_SECTION
//_LONG_CALL_ static s64 div_s64(s64 dividend, s32 divisor)
//{
//        s32 remainder;
//        return div_s64_rem(dividend, divisor, &remainder);
//}
#endif

LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static const char *_parse_integer_fixup_radix(const char *s, unsigned int *base)
{
        if (*base == 0) {
                if (s[0] == '0') {
                        if (_tolower(s[1]) == 'x' && isxdigit(s[2]))
                                *base = 16;
                        else
                                *base = 8;
                } else
                        *base = 10;
        }
        if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x')
                s += 2;
        return s;
}

/**
 * mul64_8 - unsigned 64bit mul unsigned 8 bit
 *add this function because we do not want link GCC in rom code.
 
        |               32               |    8    |        24        |
 *                                                              |    8    |
 --------------------------------------------
                                           |           24*8             |
  +                            |     8*8          |
  +|           32*8                   |
 --------------------------------------------
 +    |                   sum                    | low 24 bits    |
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static unsigned long long mul64_8 (unsigned long long  u, unsigned char  v)
{

//#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
//	struct DWstruct {int high, low;};
//#else
	struct DWstruct {unsigned int low, high;};
//#endif

	typedef union
	{
		struct DWstruct s;
		unsigned long long ll;
	} DWunion;

	const DWunion uu = {.ll = u};
	const unsigned char vv = v;
	unsigned int temp;
	DWunion w ;

	w.s.low = uu.s.low *  vv;
	w.s.low &= 0xFFFFFF;
		
	w.s.high =  ( uu.s.low >> 24) *  vv ;
	w.s.high += (((uu.s.low & 0xFFFFFF)*vv)>>24);
	
	temp = w.s.high <<24;
	
	w.s.high = (w.s.high >> 8);
	w.s.high += uu.s.high * vv;

	w.s.low |= temp;
	return w.ll;

}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p)
{              
        unsigned long long res;
        unsigned int rv;
        int overflow;
               
        res = 0;
        rv = 0;
        overflow = 0;
        while (*s) {
                unsigned int val;
               
                if ('0' <= *s && *s <= '9')
                        val = *s - '0';
                else if ('a' <= _tolower(*s) && _tolower(*s) <= 'f')
                        val = _tolower(*s) - 'a' + 10;
                else
                        break;
               
                if (val >= base)
                        break;
                /*
                 * Check for overflow only if we are within range of
                 * it in the max base we support (16)
                 */
                if (unlikely(res & (~0ull << 60))) {
                        if (res > div_u64(ULLONG_MAX - val, base))
                                overflow = 1;
                }
                res = mul64_8(res , (unsigned char)base) + val;
                rv++;
                s++;
        }      
        *p = res;
        if (overflow)
                rv |= KSTRTOX_OVERFLOW;
        return rv;
}   

/*
 * _strtoull - convert a string to an unsigned long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoull instead.
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ unsigned long long _strtoull(const char *cp, char **endp, unsigned int base)
{
	unsigned long long result;
	unsigned int rv;

	cp = _parse_integer_fixup_radix(cp, &base);
	rv = _parse_integer(cp, base, &result);

	return result;
}

/**
 * _strtoll - convert a string to a signed long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoll instead.
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ long long _strtoll(const char *cp, char **endp, unsigned int base)
{
        if (*cp == '-')
                return -_strtoull(cp + 1, endp, base);

        return _strtoull(cp, endp, base);
}

#pragma GCC diagnostic pop