/*

	this is the c lib patch, It can help when the clib provided by IAR
	does not work well.

	How to use this:
		1.You must include platform_stdlib.h in you source file。
		2.There is a macro USE_CLIB_PATCH in platform_stdlib.h should be opened.
		
		If there is some problems using this patch,
		You'd better check if you code runs into these functions:

		DiagSscanfPatch
		DiagStrtokPatch
		DiagStrstrPatch
		DiagSnPrintfPatch
		DiagPrintfPatch
		DiagSPrintfPatch
		DiagPrintfPatch
		DiagSPrintfPatch
		DiagSnPrintfPatch
		DiagStrstrPatch
		DiagStrtokPatch

 */
#ifndef CONFIG_PLATFORM_8711B

#include <stdarg.h>

#define DiagPutChar	HalSerialPutcRtl8195a

#define IN
#define NULL 0

typedef unsigned int size_t;
typedef unsigned int SIZE_T;
typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short int	u16;
typedef unsigned char		u8;
typedef signed long long 	s64;
typedef signed int		s32;
typedef signed short int	s16;
typedef unsigned char		bool;


#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == ',')
#define ULLONG_MAX      (~0ULL)
#define USHRT_MAX       ((u16)(~0U))
#define KSTRTOX_OVERFLOW        (1U << 31)
#define SHRT_MAX        ((s16)(USHRT_MAX>>1))

static inline char _tolower(const char c)
{
	return c | 0x20;
}


extern s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder);
extern s64 div_s64(s64 dividend, s32 divisor);
extern inline char _tolower(const char c);
extern u64 div_u64_rem(u64 dividend, u32 divisor, u32 *remainder);
extern u64 div_u64(u64 dividend, u32 divisor);
extern unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p);
extern const char *_parse_integer_fixup_radix(const char *s, unsigned int *base);
extern char *skip_spaces(const char *str);
extern int skip_atoi(const char **s);
extern void  HalSerialPutcRtl8195a(u8 c);


static unsigned long long simple_strtoull_patch(const char *cp, char **endp, unsigned int base)
{
	unsigned long long result;
	unsigned int rv;

	cp = _parse_integer_fixup_radix(cp, &base);
	rv = _parse_integer(cp, base, &result);

	return result;
}

static long long simple_strtoll_patch(const char *cp, char **endp, unsigned int base)
{
	if(*cp == '-')
		return -simple_strtoull_patch(cp + 1, endp, base);

	return simple_strtoull_patch(cp, endp, base);
}
static unsigned long simple_strtoul_patch(const char *cp, char **endp, unsigned int base)
{
	return simple_strtoull_patch(cp, endp, base);
}

static long simple_strtol_patch(const char *cp, char **endp, unsigned int base)
{
	if(*cp == '-')
		return -simple_strtoul_patch(cp + 1, endp, base);

	return simple_strtoul_patch(cp, endp, base);
}




static int judge_digit_width(const char *str)
{

	int width = 0;

	while(isdigit(*str)) {
		width++;
		str++;
	}

	return width;
}


static int _vsscanf_patch(const char *buf, const char *fmt, va_list args)
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

	char str_store[20] = {0};



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
			digit = *(str + 1);

		if(!digit
		        || (base == 16 && !isxdigit(digit))
		        || (base == 10 && !isdigit(digit))
		        || (base == 8 && (!isdigit(digit) || digit > '7'))
		        || (base == 0 && !isdigit(digit))) {
			break;
		}

		//here problem *******************************************



		//troy add ,fix support %2d, but not support %d
		if(field_width <= 0) {

			field_width = judge_digit_width(str);
		}


		/////troy add, fix str passed inwidth wrong
		for(i = 0; i<field_width ; i++)
			str_store[i] = str[i];

		next = (char*)str + field_width;



		if(is_sign) {
			val.s = qualifier != 'L' ?
			        simple_strtol_patch(str_store, &next, base) :
			        simple_strtoll_patch(str_store, &next, base);
		} else {
			val.u = qualifier != 'L' ?
			        simple_strtoul_patch(str_store, &next, base) :
			        simple_strtoull_patch(str_store, &next, base);
		}


		////troy add
		for(i = 0; i<20 ; i++)
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


int DiagSscanfPatch(const char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsscanf_patch(buf, fmt, args);
	va_end(args);

	return i;
}



/*********************************************************/



char* DiagStrtokPatch(char *str, const char* delim) {
	static char* _buffer;

	if(str != NULL) _buffer = str;

	if(_buffer[0] == '\0') return NULL;

	char *ret = _buffer, *b;
	const char *d;

	for(b = _buffer; *b !='\0'; b++) {
		for(d = delim; *d != '\0'; d++) {
			if(*b == *d) {
				*b = '\0';
				_buffer = b+1;

				// skip the beginning delimiters
				if(b == ret) {
					ret++;
					continue;
				}

				return ret;
			}
		}
	}

	return ret;
}



/*********************************************************/



char *DiagStrstrPatch(char *string, char *substring)
{
	register char *a, *b;

	/* First scan quickly through the two strings looking for a
	 * single-character match.  When it's found, then compare the
	 * rest of the substring.
	 */

	b = substring;

	if(*b == 0) {
		return string;
	}

	for(; *string != 0; string += 1) {
		if(*string != *b) {
			continue;
		}

		a = string;

		while(1) {
			if(*b == 0) {
				return string;
			}

			if(*a++ != *b++) {
				break;
			}
		}

		b = substring;
	}

	return (char *) 0;
}





/*********************************************************/




int DiagSnPrintfPatch(char *buf, size_t size, const char *fmt, ...)
{

	va_list     ap;
	char *p, *s, *buf_end = NULL;
	const int *dp = ((const int *)&fmt)+1;

	if(buf == NULL)
		return 0;


	va_start(ap, fmt);
	s = buf;
	buf_end = size? (buf + size):(char*)~0;

	for(; *fmt != '\0'; ++fmt) {

		if(*fmt != '%') {
			*s++ = *fmt;

			if(s >= buf_end) {
				goto Exit;
			}

			continue;
		}

		if(*++fmt == 's') {
			for(p = (char *)*dp++; *p != '\0'; p++) {
				*s++ = *p;

				if(s >= buf_end) {
					goto Exit;
				}
			}
		}
		else {	/* Length of item is bounded */
			char tmp[20], *q = tmp;
			int alt = 0;
			int shift = 0;// = 12;
			const long *lpforchk = (const long *)dp;

			if((*lpforchk) < 0x10) {
				shift = 0;
			}
			else if(((*lpforchk) >= 0x10) && ((*lpforchk) < 0x100)) {
				shift = 4;
			}
			else if(((*lpforchk) >= 0x100) && ((*lpforchk) < 0x1000)) {
				shift = 8;
			}
			else if(((*lpforchk) >= 0x1000) && ((*lpforchk) < 0x10000)) {
				shift = 12;
			}
			else if(((*lpforchk) >= 0x10000) && ((*lpforchk) < 0x100000)) {
				shift = 16;
			}
			else if(((*lpforchk) >= 0x100000) && ((*lpforchk) < 0x1000000)) {
				shift = 20;
			}
			else if(((*lpforchk) >= 0x1000000) && ((*lpforchk) < 0x10000000)) {
				shift = 24;
			}
			else if((*lpforchk) >= 0x10000000) {
				shift = 28;
			}
			else {
				shift = 28;
			}

			if((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;

				for(width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
				{	width = width * 10 + fch - '0';
				}

				shift=(width-1)*4;
			}

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */
			if((*fmt == 'x')||(*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int hex_count = 0;
				unsigned long h_back = h;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;

				if((*fmt == 'p') || (*fmt == 'P'))
					alt=1;

				if(alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}

				while(h_back) {
					hex_count += (h_back & 0xF) ? 1 : 0;
					h_back  = h_back >> 4;
				}

				if(shift < (hex_count - 1)*4)
					shift = (hex_count - 1)*4;

				for(; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if(*fmt == 'd') {
				int i = *dp++;
				char *r;
				int digit_space = 0;


				if(i < 0) {
					*q++ = '-';
					i = -i;
					digit_space++;
				}

				p = q;		/* save beginning of digits */


				do {
					*q++ = '0' + (i % 10);
					i /= 10;
					digit_space++;
				} while(i);


				for(; shift >= 0; shift -= 4) {

					if(digit_space-- > 0) {
						; //do nothing
					} else {
						*q++ = '0';
					}
				}

				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */

				while(--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
			else if(*fmt == 'c')
				*q++ = *dp++;
			else
				*q++ = *fmt;

			/* now output the saved string */
			for(p = tmp; p < q; ++p) {
				*s++ = *p;

				if(s >= buf_end) {
					goto Exit;
				}
			}
		}
	}

Exit:

	if(buf)
		*s = '\0';

	va_end(ap);
	return(s-buf);

}






/*********************************************************/

static int VSprintfPatch(char *buf, const char *fmt, const int *dp)
{
	char *p, *s;
	s = buf;

	for(; *fmt != '\0'; ++fmt) {
		if(*fmt != '%') {
			if(buf) {
				*s++ = *fmt;
			} else {
				DiagPutChar(*fmt);
			}

			continue;
		}

		if(*++fmt == 's') {
			for(p = (char *)*dp++; *p != '\0'; p++) {
				if(buf) {
					*s++ = *p;
				} else {
					DiagPutChar(*p);
				}
			}
		}
		else {	/* Length of item is bounded */
			char tmp[20], *q = tmp;
			int alt = 0;
			int shift = 0;// = 12;
			const long *lpforchk = (const long *)dp;

			if((*lpforchk) < 0x10) {
				shift = 0;
			}
			else if(((*lpforchk) >= 0x10) && ((*lpforchk) < 0x100)) {
				shift = 4;
			}
			else if(((*lpforchk) >= 0x100) && ((*lpforchk) < 0x1000)) {
				shift = 8;
			}
			else if(((*lpforchk) >= 0x1000) && ((*lpforchk) < 0x10000)) {
				shift = 12;
			}
			else if(((*lpforchk) >= 0x10000) && ((*lpforchk) < 0x100000)) {
				shift = 16;
			}
			else if(((*lpforchk) >= 0x100000) && ((*lpforchk) < 0x1000000)) {
				shift = 20;
			}
			else if(((*lpforchk) >= 0x1000000) && ((*lpforchk) < 0x10000000)) {
				shift = 24;
			}
			else if((*lpforchk) >= 0x10000000) {
				shift = 28;
			}
			else {
				shift = 28;
			}

#if 1   //wei patch for %02x

			if((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;

				for(width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
				{	width = width * 10 + fch - '0';
				}

				shift=(width-1)*4;
			}

#endif

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */

			if((*fmt == 'x')||(*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int hex_count = 0;
				unsigned long h_back = h;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;

				if((*fmt == 'p') || (*fmt == 'P'))
					alt=1;

				if(alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}

				//hback 是实际得到的数据，hex_count是统计数据的HEX字符个数
				while(h_back) {
					hex_count += (h_back & 0xF) ? 1 : 0;
					h_back  = h_back >> 4;
				}

				//这里修复 example：  字符有4个，但是用了%02x导致字符被截断的情况
				if(shift < (hex_count - 1)*4)
					shift = (hex_count - 1)*4;

				//printf("(%d,%d)", hex_count, shift);

				for(; shift >= 0; shift -= 4) {

					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
				}

			}
			else if(*fmt == 'd') {
				int i = *dp++;
				char *r;
				int digit_space = 0;

				if(i < 0) {
					*q++ = '-';
					i = -i;
					digit_space++;
				}

				p = q;		/* save beginning of digits */

				do {
					*q++ = '0' + (i % 10);
					i /= 10;
					digit_space++;
				} while(i);

				//这里修复 example：用了%08d后，在数字前面没有0的情况
				for(; shift >= 0; shift -= 4) {

					if(digit_space-- > 0) {
						; //do nothing
					} else {
						*q++ = '0';
					}
				}

				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */

				while(--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
			else if(*fmt == 'c')
				*q++ = *dp++;
			else
				*q++ = *fmt;

			/* now output the saved string */
			for(p = tmp; p < q; ++p) {
				if(buf) {
					*s++ = *p;
				} else {
					DiagPutChar(*p);
				}

				if((*p) == '\n') {
					DiagPutChar('\r');
				}
			}
		}
	}

	if(buf)
		*s = '\0';

	return (s - buf);
}


u32  DiagPrintfPatch(
    IN  const char *fmt, ...
)
{
	(void)VSprintfPatch(0, fmt, ((const int *)&fmt)+1);
	return 1;
}

u32  DiagSPrintfPatch(
    IN  u8 *buf,
    IN  const char *fmt, ...
)
{
	(void)VSprintfPatch((char*)buf, fmt, ((const int *)&fmt)+1);
	return 1;
}
#endif
