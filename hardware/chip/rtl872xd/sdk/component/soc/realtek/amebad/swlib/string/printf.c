/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2017 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 
#include "ameba_soc.h"
#include <stdarg.h>
#include <strproc.h>
#include <stddef.h> /* for size_t */
#include <stdarg.h>
#include <sys/types.h>
#include "printf.h"

HAL_ROM_DATA_SECTION
static const char hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

HAL_ROM_DATA_SECTION
static const char hextable_caps[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

union double_int {
    double d;
    uint64_t i;
};

#define PNTC(c) buf[pos++] = (c)
#define PNTSTR(str) do { for (i = 0; (str)[i] != 0; i++) PNTC((str)[i]); } while (0)
#define PNTREV(c) do { if (&buf[pos] == buf) goto done; else buf[--pos] = (c); } while (0)
#define OUTPUT_STRING(str, len) do {chars_written += outString (str, len);} while(0)
#define OUTPUT_CHAR(c) do { char __temp[1] = { c }; \
		OUTPUT_STRING(__temp, 1);\
	} while (0)
	
static char *longlong_to_string(char *buf, unsigned long long n, size_t len, uint flag, char *signchar)
{
    size_t pos = len;
    int negative = 0;

    if ((flag & SIGNEDFLAG) && (long long)n < 0) {
        negative = 1;
        n = -n;
    }

    buf[--pos] = 0;

    /* only do the math if the number is >= 10 */
    while (n >= 10) {
        int digit = n % 10;

        n /= 10;

        buf[--pos] = digit + '0';
    }
    buf[--pos] = n + '0';

    if (negative)
        *signchar = '-';
    else if ((flag & SHOWSIGNFLAG))
        *signchar = '+';
    else if ((flag & BLANKPOSFLAG))
        *signchar = ' ';
    else
        *signchar = '\0';

    return &buf[pos];
}

static char *longlong_to_octalstring(char *buf, unsigned long long u, size_t len, uint flag)
{
	size_t pos = len;

	buf[--pos] = 0;
	/* only do the math if the number is >= 8 */
       while (u >= 8) {
           unsigned int digit = u % 8;

           u /= 8;

           buf[--pos] = digit + '0';
       }
       buf[--pos] = u + '0';

	 return &buf[pos];
}

static char *longlong_to_hexstring(char *buf, unsigned long long u, size_t len, uint flag)
{
    size_t pos = len;
    const char *table = (flag & CAPSFLAG) ? hextable_caps : hextable;

    buf[--pos] = 0;
    do {
        unsigned int digit = u % 16;
        u /= 16;

        buf[--pos] = table[digit];
    } while (u != 0);

    return &buf[pos];
}

/* print up to a 4 digit exponent as string, with sign */
static size_t exponent_to_string(char *buf, int32_t exponent)
{
    size_t pos = 0;

    /* handle sign */
    if (exponent < 0) {
        PNTC('-');
        exponent = -exponent;
    } else {
        PNTC('+');
    }

    /* see how far we need to bump into the string to print from the right */
    if (exponent >= 1000) pos += 4;
    else if (exponent >= 100) pos += 3;
    else if (exponent >= 10) pos += 2;
    else pos++;

    /* print decimal string, from the right */
    uint i = pos;
    do {
        uint digit = (uint32_t)exponent % 10;

        buf[--i] = digit + '0';

        exponent /= 10;
    } while (exponent != 0);

    /* return number of characters printed */
    return pos;
}

static char *double_to_string(char *buf, size_t len, double d, uint flag)
{
    size_t pos = 0;
    int i;
    union double_int u = { d };

    uint32_t exponent = (u.i >> 52) & 0x7ff;
    uint64_t fraction = (u.i & ((1ULL << 52) - 1));
    bool neg = !!(u.i & (1ULL << 63));

    /* start constructing the string */
    if (neg) {
        PNTC('-');
        d = -d;
    }

    /* longest:
     * 
        1797693134862315708145274237317043567980705675258449965989174768031572607800285
        3876058955863276687817154045895351438246423432132688946418276846754670353751698
        6049910576551282076245490090389328944075868508455133942304583236903222948165808
        559332123348274797826204144723168738177180919299881250404026184124858368.
        000000o
     */

    /* look for special cases */
    if (exponent == 0x7ff) {
        if (fraction == 0) {
            /* infinity */
            if (flag & CAPSFLAG) 
			PNTSTR("INF");
            else 
			PNTSTR("inf");
        } else {
            /* NaN */
            if (flag & CAPSFLAG)
			PNTSTR("NAN");
            else 
			PNTSTR("nan");
        }
    } else if (exponent == 0) {
        if (fraction == 0) {
            /* zero */
            PNTSTR("0.000000");
        } else {
            /* denormalized */
            /* XXX does not handle */
            if (flag & CAPSFLAG) 
			PNTSTR("DEN");
            else 
			PNTSTR("den");
        }
    } else {
        /* see if it's in the range of floats we can easily print */
        int exponent_signed = exponent - 1023;
        if (exponent_signed < -52 || exponent_signed > 52) {	//保证精度
            PNTSTR("<range>");
        } else {
            /* start by walking backwards through the string */
            pos = len;
            PNTREV(0);

            /* reserve space for the fractional component first */
            for (i = 0; i <= 6; i++)
                PNTREV('0');
            size_t decimal_spot = pos;

            /* print the integer portion */
            uint64_t u;
            if (exponent_signed >= 0) {
                u = fraction;
                u |= (1ULL<<52);
                u >>= (52 - exponent_signed);

                char *s = longlong_to_string(buf, u, pos + 1, flag, &(char) {0});

                pos = s - buf;
            } else {
                /* exponent is negative */
                u = 0;
                PNTREV('0');
            }

            buf[decimal_spot] = '.';

            /* handle the fractional part */
            uint32_t frac = ((d - u) * 1000000) + .5;

            uint i = decimal_spot + 6 + 1;
            while (frac != 0) {
                uint digit = frac % 10;

                buf[--i] = digit + '0';

                frac /= 10;
            }

            if (neg)
                PNTREV('-');

done:
            /* separate return path, since we've been walking backwards 
through the string */
            return &buf[pos];
        }
#undef PNTREV
    }

    buf[pos] = 0;
    return buf;
}

static char *double_to_hexstring(char *buf, size_t len, double d, uint flag)
{
    size_t pos = 0;
    int i;
    union double_int u = { d };

    uint32_t exponent = (u.i >> 52) & 0x7ff;
    uint64_t fraction = (u.i & ((1ULL << 52) - 1));
    bool neg = !!(u.i & (1ULL << 63));

    /* start constructing the string */
    if (neg) {
        PNTC('-');
    }

    /* look for special cases */
    if (exponent == 0x7ff) {
        if (fraction == 0) {
            /* infinity */
            if (flag & CAPSFLAG) PNTSTR("INF");
            else PNTSTR("inf");
        } else {
            /* NaN */
            if (flag & CAPSFLAG) PNTSTR("NAN");
            else PNTSTR("nan");
        }
    } else if (exponent == 0) {
        if (fraction == 0) {
            /* zero */
            if (flag & CAPSFLAG) PNTSTR("0X0P+0");
            else PNTSTR("0x0p+0");
        } else {
            /* denormalized */
            /* XXX does not handle */
            if (flag & CAPSFLAG) PNTSTR("DEN");
            else PNTSTR("den");
        }
    } else {
        /* regular normalized numbers:
         * 0x1p+1
         * 0x1.0000000000001p+1
         * 0X1.FFFFFFFFFFFFFP+1023
         * 0x1.FFFFFFFFFFFFFP+1023
         */
        int exponent_signed = exponent - 1023;

        /* implicit 1. */
        if (flag & CAPSFLAG) PNTSTR("0X1");
        else PNTSTR("0x1");

        /* select the appropriate hex case table */
        const char *table = (flag & CAPSFLAG) ? hextable_caps : hextable;

        int zero_count = 0;
        bool output_dot = FALSE;
        for (i = 52 - 4; i >= 0; i -= 4) {
            uint digit = (fraction >> i) & 0xf;

            if (digit == 0) {
                zero_count++;
            } else {
                /* output a . the first time we output a char */
                if (!output_dot) {
                    PNTC('.');
                    output_dot = TRUE;
                }
                /* if we have a non zero digit, see if we need to output a string of zeros */
                while (zero_count > 0) {
                    PNTC('0');
                    zero_count--;
                }
                buf[pos++] = table[digit];
            }
        }

        /* handle the exponent */
        buf[pos++] = (flag & CAPSFLAG) ? 'P' : 'p';
        pos += exponent_to_string(&buf[pos], exponent_signed);
    }

    buf[pos] = 0;
    return buf;
}

static unsigned int outString(const char *buffer, int len)
{
	unsigned int count = 0;
	int i;
	char c;

	for (i = 0; i < len ; i++)
	{
		c = buffer[i];

		DiagPutChar(c);
		count ++;
	}

	return count;
}

LIBC_ROM_TEXT_SECTION _LONG_CALL_
unsigned int printf_engine(const char *fmt, va_list args)
{
	int err = 0;
	char c;
	unsigned char uc;
	const char *s;
	size_t string_len;
	unsigned long long n;
	void *ptr;
	int flags;
	unsigned int format_num;
	char signchar;
	size_t chars_written = 0;
	char num_buffer[32];
    
	for (;;) {
		flags = 0;
		format_num = 0;
		signchar = '\0';

		/* handle regular chars that aren't format related */
		s = fmt;
		string_len = 0;
	    
		while ((c = *fmt++) != 0) {
			if (c == '%')
				break; /* we saw a '%', break and start parsing format */
			string_len++;
		}

		OUTPUT_STRING(s, string_len);


		if (c == 0)
			break;

next_format:
		/* grab the next format character */
		c = *fmt++;
		if (c == 0)
			break;

		switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (c == '0' && format_num == 0)
					flags |= LEADZEROFLAG;
				format_num *= 10;
				format_num += c - '0';
				goto next_format;
			case '.':
				/* XXX for now eat numeric formatting */
				goto next_format;
			case '%':
				OUTPUT_CHAR('%');
			break;
			case 'c':
				uc = va_arg(args, unsigned int);

				/*add for %(digit)c, e.g. %3c*/
				for (; format_num > 1; format_num--)
					OUTPUT_CHAR(' ');

				OUTPUT_CHAR(uc);
			break;
			case 's':
				s = va_arg(args, const char *);
				if (s == 0)
					s = "<null>";
				flags &= ~LEADZEROFLAG; /* doesn't make sense for strings */
				goto _output_string;
			case '-':
				flags |= LEFTFORMATFLAG;
				goto next_format;
			case '+':
				flags |= SHOWSIGNFLAG;
				goto next_format;
			case ' ':
				flags |= BLANKPOSFLAG;
				goto next_format;
			case '#':
				flags |= ALTFLAG;
				goto next_format;
			case 'l':
				if (flags & LONGFLAG)
					flags |= LONGLONGFLAG;
				flags |= LONGFLAG;
				goto next_format;
			case 'h':
				if (flags & HALFFLAG)
					flags |= HALFHALFFLAG;
				flags |= HALFFLAG;
				goto next_format;
			case 'z':
				flags |= SIZETFLAG;
				goto next_format;
			case 'j':
				flags |= INTMAXFLAG;
				goto next_format;
			case 't':
				flags |= PTRDIFFFLAG;
				goto next_format;     
			case 'i':
			case 'd':
				n = (flags & LONGLONGFLAG) ? va_arg(args, long long) :
					(flags & LONGFLAG) ? va_arg(args, long) :
					(flags & HALFHALFFLAG) ? (signed char)va_arg(args, int) :
					(flags & HALFFLAG) ? (short)va_arg(args, int) :
					(flags & SIZETFLAG) ? va_arg(args, ssize_t) :
					(flags & INTMAXFLAG) ? va_arg(args, intmax_t) :
					(flags & PTRDIFFFLAG) ? va_arg(args, ptrdiff_t) :
					va_arg(args, int);

				flags |= SIGNEDFLAG;
				s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags, &signchar);
				goto _output_string;
			case 'u':
				n = (flags & LONGLONGFLAG) ? va_arg(args, unsigned long long) :
					(flags & LONGFLAG) ? va_arg(args, unsigned long) :
					(flags & HALFHALFFLAG) ? (unsigned char)va_arg(args, unsigned int) :	
					(flags & HALFFLAG) ? (unsigned short)va_arg(args, unsigned int) :	
					(flags & SIZETFLAG) ? va_arg(args, size_t) :	
					(flags & INTMAXFLAG) ? va_arg(args, uintmax_t) :
					(flags & PTRDIFFFLAG) ? (uintptr_t)va_arg(args, ptrdiff_t) : 
					va_arg(args, unsigned int);

				s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags, &signchar);
				goto _output_string;
			case 'o':
				n = (flags & LONGLONGFLAG) ? va_arg(args, unsigned long long) :
					(flags & LONGFLAG) ? va_arg(args, unsigned long) :
					(flags & HALFHALFFLAG) ? (unsigned char)va_arg(args, unsigned int) :
					(flags & HALFFLAG) ? (unsigned short)va_arg(args, unsigned int) :
					(flags & SIZETFLAG) ? va_arg(args, size_t) :
					(flags & INTMAXFLAG) ? va_arg(args, uintmax_t) :
					(flags & PTRDIFFFLAG) ? (uintptr_t)va_arg(args, ptrdiff_t) :
					va_arg(args, unsigned int);
				s = longlong_to_octalstring(num_buffer, n, sizeof(num_buffer), flags);
				if (flags & ALTFLAG)
					OUTPUT_CHAR('0');
				goto _output_string;
			case 'P':
				flags |= CAPSFLAG;
			case 'p':
				flags |= LONGFLAG | ALTFLAG;
				goto hex;
			case 'X':
				flags |= CAPSFLAG;

				/* fallthrough */
				hex:
			case 'x':
				n = (flags & LONGLONGFLAG) ? va_arg(args, unsigned long long) :
					(flags & LONGFLAG) ? va_arg(args, unsigned long) :
					(flags & HALFHALFFLAG) ? (unsigned char)va_arg(args, unsigned int) :
					(flags & HALFFLAG) ? (unsigned short)va_arg(args, unsigned int) :
					(flags & SIZETFLAG) ? va_arg(args, size_t) :
					(flags & INTMAXFLAG) ? va_arg(args, uintmax_t) :
					(flags & PTRDIFFFLAG) ? (uintptr_t)va_arg(args, ptrdiff_t) :
					va_arg(args, unsigned int);

				s = longlong_to_hexstring(num_buffer, n, sizeof(num_buffer), flags);
				if (flags & ALTFLAG) {
					OUTPUT_CHAR('0');
					OUTPUT_CHAR((flags & CAPSFLAG) ? 'X': 'x');
				}
				goto _output_string;
			case 'n':
				ptr = va_arg(args, void *);
				if (flags & LONGLONGFLAG)
					*(long long *)ptr = chars_written;
				else if (flags & LONGFLAG)
					*(long *)ptr = chars_written;
				else if (flags & HALFHALFFLAG)
					*(signed char *)ptr = chars_written;
				else if (flags & HALFFLAG)
					*(short *)ptr = chars_written;
				else if (flags & SIZETFLAG)
					*(size_t *)ptr = chars_written;
				else
					*(int *)ptr = chars_written;
			break;

			case 'F':
				flags |= CAPSFLAG;
				/* fallthrough */
			case 'f': {
				double d = va_arg(args, double);
				s = double_to_string(num_buffer, sizeof(num_buffer), d, flags);
				goto _output_string;
			}
			case 'A':
				flags |= CAPSFLAG;
				/* fallthrough */
			case 'a': {
				double d = va_arg(args, double);
				s = double_to_hexstring(num_buffer, sizeof(num_buffer), d, flags);
				goto _output_string;
			}

			default:
				OUTPUT_CHAR('%');
				OUTPUT_CHAR(c);
			break;
		}

		continue;

_output_string:
		string_len = _strlen(s);

		if (flags & LEFTFORMATFLAG) {
			/* left justify the text */
			/*
			OUTPUT_STRING(s, string_len);
			uint written = err;*/

			uint count = 0; 

			/*output the signchar (if necessary), debug for leftformat, negative data output*/
			if(signchar != '\0'){
				OUTPUT_CHAR(signchar);
				count++;
			}

			/*add count, debug for leftformat, output more than string_len*/
			while(*s != 0) {
				OUTPUT_CHAR(*s++);
				count++;
			}

			/* pad to the right (if necessary) */
			for (; format_num > count/*written*/; format_num--)
				OUTPUT_CHAR(' ');
		} else {
			/* right justify the text (digits) */

			/* if we're going to print a sign digit,
			it'll chew up one byte of the format size */
			if (signchar != '\0' && format_num > 0)
				format_num--;

			/* output the sign char before the leading zeros */
			if (flags & LEADZEROFLAG && signchar != '\0')
				OUTPUT_CHAR(signchar);

			/* pad according to the format string */
			for (; format_num > string_len; format_num--)
				OUTPUT_CHAR(flags & LEADZEROFLAG ? '0' : ' ');

			/* if not leading zeros, output the sign char just before the number */
			if (!(flags & LEADZEROFLAG) && signchar != '\0')
				OUTPUT_CHAR(signchar);

			/* output the string */
			OUTPUT_STRING(s, string_len);
	    }
	    continue;

	}
	return (err < 0) ? err : (int)chars_written;
}

LIBC_ROM_TEXT_SECTION _LONG_CALL_
int _printf(const char * fmt,...)
{
	int count;
	va_list list;

	va_start(list, fmt);
	count = printf_engine(fmt, list);
	va_end(list);
	(void)list;

	return count;
}
