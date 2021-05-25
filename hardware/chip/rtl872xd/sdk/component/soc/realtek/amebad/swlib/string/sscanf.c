/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"

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
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder)
{
        *remainder = (s32)dividend % divisor;
        return (s32)dividend / divisor;
}

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
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static s64 div_s64(s64 dividend, s32 divisor)
{
        s32 remainder;
        return div_s64_rem(dividend, divisor, &remainder);
}
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

LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static char *skip_spaces(const char *str)
{
    while (isspace(*str))
        ++str;
    return (char *)str;
}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_ static int skip_atoi(const char **s)
{
    int i = 0;

    while (isdigit(**s))
        i = i*10 + *((*s)++) - '0';

    return i;
}

LIBC_ROM_TEXT_SECTION
static int judge_digit_width(const char *str, unsigned int base)
{

	int width = 0;

	if ((base == 16 || base == 0) && str[0] == '0' && _tolower(str[1]) == 'x') {
		str += 2;
		width += 2;
		
		while(isxdigit(*str)) {
			width++;
			str++;
		}
	}else if(base == 16){
		while(isxdigit(*str)) {
			width++;
			str++;
		}
	}else if(base == 8 ||(base == 0 && str[0] == '0' )) {
		while(isdigit(*str) && (*str)<'8'){
			width++;
			str++;
		}
	}else{
		while(isdigit(*str)) {
			width++;
			str++;
		}
	}

	return width;
}


LIBC_ROM_TEXT_SECTION
_LONG_CALL_ int _vsscanf(const char *buf, const char *fmt, va_list args)
{
	const char *str = buf;
	char *next;
	char digit;
	int num = 0;
	int i =0;
	u8 qualifier;
	unsigned int base;
	union {
		long long s;
		unsigned long long u;
	} val;
	s16 field_width;
	bool is_sign;

	char str_store[STR_STORE_MAX_LEN];
	for(i = 0; i<STR_STORE_MAX_LEN ; i++)
 		str_store[i] = 0;


	while(*fmt) {
		/* skip any white space in format */
		/* white space in format matchs any amount of
		    * white space, including none, in the input.
		    */
		if(isspace(*fmt)) {
			fmt = skip_spaces(++fmt);
			str = skip_spaces(str);
		}

		/* anything that is not a conversion must match exactly */
		if(*fmt != '%' && *fmt) {
			if(*fmt++ != *str++) {
				break;
			}

			continue;
		}

		if(!*fmt) {
			break;
		}

		++fmt;

		/* skip this conversion.
		    * advance both strings to next white space
		    */
		if(*fmt == '*') {
			if(!*str) {
				break;
			}

			while(!isspace(*fmt) && *fmt != '%' && *fmt)
				fmt++;

			while(!isspace(*str) && *str)
				str++;

			continue;
		}

		/* get field width */
		field_width = -1;

		if(isdigit(*fmt)) {

			field_width = skip_atoi(&fmt);



			if(field_width <= 0) {

				break;
			}
		}

		/* get conversion qualifier */
		qualifier = -1;

		if(*fmt == 'h' || _tolower(*fmt) == 'l' ||
		        _tolower(*fmt) == 'z') {
			qualifier = *fmt++;

			if(qualifier == *fmt) {
				if(qualifier == 'h') {
					qualifier = 'H';
					fmt++;
				} else if(qualifier == 'l') {
					qualifier = 'L';
					fmt++;
				}
			}
		}

		if(!*fmt) {
			break;
		}

		if(*fmt == 'n') {
			/* return number of characters read so far */
			*va_arg(args, int *) = str - buf;
			++fmt;
			continue;
		}

		if(!*str) {
			break;
		}

		base = 10;
		is_sign = 0;

		switch(*fmt++) {
		case 'c': {
			char *s = (char *)va_arg(args, char*);

			if(field_width == -1)
				field_width = 1;

			do {
				*s++ = *str++;
			} while(--field_width > 0 && *str);

			num++;
		}

		continue;

		case 's': {
			char *s = (char *)va_arg(args, char *);

			if(field_width == -1)
				field_width = SHRT_MAX;

			/* first, skip leading white space in buffer */
			str = skip_spaces(str);

			/* now copy until next white space */
			while(*str && !isspace(*str) && field_width--) {
				*s++ = *str++;
			}

			*s = '\0';
			num++;
		}

		continue;

		case 'o':
			base = 8;
			break;

		case 'x':
		case 'X':
			base = 16;
			break;

		case 'i':
			base = 0;

		case 'd':
			is_sign = 1;

		case 'u':
			break;

		case '%':

			/* looking for '%' in str */
			if(*str++ != '%') {
				return num;
			}

			continue;

		default:
			/* invalid format; stop here */
			return num;
		}

		/* have some sort of integer conversion.
		    * first, skip white space in buffer.
		*/
		str = skip_spaces(str);

		digit = *str;

		if(is_sign && digit == '-')
		{
			if(field_width==-1)
			{
				field_width = judge_digit_width(str+1, base);
				field_width++;
			}else{
				field_width = (judge_digit_width(str+1,base)+1)<field_width ?
					(judge_digit_width(str+1,base)+1) :
					field_width;
			}
			digit = *(str + 1);
		}else if(field_width==-1){
			field_width = judge_digit_width(str, base);
		}else{	
			field_width = judge_digit_width(str,base)<field_width ? 
				judge_digit_width(str,base) :
				field_width;
		}

		if(!digit
		        || (base == 16 && !isxdigit(digit))
		        || (base == 10 && !isdigit(digit))
		        || (base == 8 && (!isdigit(digit) || digit > '7'))
		        || (base == 0 && !isdigit(digit))) {
			break;
		}

		//here problem *******************************************



		//troy add ,fix support %2d, but not support %d
#if 0
		if(field_width <= 0) {

			field_width = judge_digit_width(str);
		}
#else
		//field_width = judge_digit_width(str, base);
#endif


		/////troy add, fix str passed inwidth wrong
		assert_param (field_width <= STR_STORE_MAX_LEN);
		for(i = 0; i<field_width ; i++)
			str_store[i] = str[i];

#if 0//ndef CONFIG_STDLIB_STRING
		/* simple strtoul need this */
		next = (char*)str + field_width;
#endif

		if(is_sign) {
			val.s = qualifier != 'L' ?
			        _strtol(str_store, &next, base) :
			        _strtoll(str_store, &next, base);
		} else {
			val.u = qualifier != 'L' ?
			        _strtoul(str_store, &next, base) :
			        _strtoull(str_store, &next, base);
		}

#if 1//def CONFIG_STDLIB_STRING
		/* standard strtoul need this */
		next = (char*)str + field_width;
#endif
		////troy add
		for(i = 0; i<STR_STORE_MAX_LEN ; i++)
			str_store[i] = 0;


		//判断转换的字符串的宽度是否大于 %2d
		if(field_width > 0 && next - str > field_width) {
			if(base == 0)
				_parse_integer_fixup_radix(str, &base);

			while(next - str > field_width) {
				if(is_sign) {
					val.s = div_s64(val.s, base);
				} else {
					val.u = div_u64(val.u, base);
				}

				--next;
			}
		}

		switch(qualifier) {
		case 'H':       /* that's 'hh' in format */
			if(is_sign)
				*va_arg(args, signed char *) = val.s;
			else
				*va_arg(args, unsigned char *) = val.u;

			break;

		case 'h':
			if(is_sign)
				*va_arg(args, short *) = val.s;
			else
				*va_arg(args, unsigned short *) = val.u;

			break;

		case 'l':
			if(is_sign)
				*va_arg(args, long *) = val.s;
			else
				*va_arg(args, unsigned long *) = val.u;

			break;

		case 'L':
			if(is_sign)
				*va_arg(args, long long *) = val.s;
			else
				*va_arg(args, unsigned long long *) = val.u;

			break;

		case 'Z':
		case 'z':
			*va_arg(args, size_t *) = val.u;
			break;

		default:
			if(is_sign)
				*va_arg(args, int *) = val.s;
			else
				*va_arg(args, unsigned int *) = val.u;

			break;
		}

		num++;

		if(!next) {
			break;
		}

		str = next;
	}

	return num;
}

/**
* sscanf - Unformat a buffer into a list of arguments
* @buf:        input buffer
* @fmt:        formatting of buffer
* @...:        resulting arguments
*/
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ int _sscanf(const char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsscanf(buf, fmt, args);
	va_end(args);

	return i;
}