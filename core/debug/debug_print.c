#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "k_api.h"
#include "debug_print.h"

#define LONGFLAG       0x00000001
#define LONGLONGFLAG   0x00000002
#define HALFFLAG       0x00000004
#define HALFHALFFLAG   0x00000008
#define SIZETFLAG      0x00000010
#define INTMAXFLAG     0x00000020
#define PTRDIFFFLAG    0x00000040
#define ALTFLAG        0x00000080
#define CAPSFLAG       0x00000100
#define SHOWSIGNFLAG   0x00000200
#define SIGNEDFLAG     0x00000400
#define LEFTFORMATFLAG 0x00000800
#define LEADZEROFLAG   0x00001000
#define BLANKPOSFLAG   0x00002000

union all_format_type {
    unsigned int i[2];
    long long    ll;
    double       d;
} format_var;

/* alios_debug_print depends on mcu*/
__attribute__((weak)) int alios_debug_print(const char *buf, int size)
{
    return 0;
}

#define OUTPUT_BUF(val, mode, buf) \
    do { \
        if (buf) { \
            if (mode == 1) { \
                format_var.ll = val; \
                buf[index++] = format_var.i[0]; \
            } else if(mode == 2) { \
                format_var.ll = val; \
                buf[index++]  = format_var.i[0]; \
                buf[index++]  = format_var.i[1]; \
            } else if(mode == 3) { \
                format_var.d = val; \
                buf[index++] = format_var.i[0]; \
                buf[index++] = format_var.i[1]; \
            } \
        } } while(0)


#define OUTPUT_STRING(str, len)\
    do { \
        if (!buf) { \
            err = alios_debug_print(str, len); \
            if (err < 0) { \
                goto exit; \
            } else { \
                chars_written += err; \
            } \
        } } while(0)

#define OUTPUT_CHAR(c) \
    do { \
        char __temp[1] = { c }; \
        OUTPUT_STRING(__temp, 1); \
    } while (0)


static const char hextable[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static const char hextable_caps[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static char *longlong_to_string(char *buf,
                                unsigned long long n,
                                size_t len,
                                unsigned int flag,
                                char *signchar)
{
    size_t pos = len;
    int negative = 0;

    if ((flag & SIGNEDFLAG) && (long long)n < 0) {
        negative = 1;
        n = -n;
    }

    buf[--pos] = 0;

    /* only do the math if the number is >= 10 */
    union {
        unsigned int ui[2];
        unsigned long long ull;
    } union_ull;
    union_ull.ull = n;
    int digit;
    if (union_ull.ui[0] == 0) {
        buf[--pos] = '0';
    } else {
        while (union_ull.ui[0] > 0) {
            digit = union_ull.ui[0] % 10;
            union_ull.ui[0] /= 10;

            buf[--pos] = digit + '0';
        }
    }

    while (union_ull.ui[1] > 0) {
        digit = union_ull.ui[1] % 10;
        union_ull.ui[1] /= 10;

        buf[--pos] = digit + '0';
    }

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



static char *longlong_to_hexstring(char *buf,
                                   unsigned long long u,
                                   size_t len,
                                   unsigned int flag)
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


union double_int {
    double d;
    uint64_t i;
};

#define OUT(c) buf[pos++] = (c)
#define OUTSTR(str) do { for (size_t i = 0; (str)[i] != 0; i++) OUT((str)[i]); } while (0)

/* print up to a 4 digit exponent as string, with sign */
static size_t exponent_to_string(char *buf, int32_t exponent) {
    size_t pos = 0;

    /* handle sign */
    if (exponent < 0) {
        OUT('-');
        exponent = -exponent;
    } else {
        OUT('+');
    }

    /* see how far we need to bump into the string to print from the right */
    if (exponent >= 1000) pos += 4;
    else if (exponent >= 100) pos += 3;
    else if (exponent >= 10) pos += 2;
    else pos++;

    /* print decimal string, from the right */
    uint32_t i = pos;
    do {
        uint32_t digit = (uint32_t)exponent % 10;

        buf[--i] = digit + '0';

        exponent /= 10;
    } while (exponent != 0);

    /* return number of characters printed */
    return pos;
}

static char *double_to_string(char *buf, size_t len, double d, uint32_t flag) {
    size_t pos = 0;
    union double_int u = { d };

    uint32_t exponent = (u.i >> 52) & 0x7ff;
    uint64_t fraction = (u.i & ((1ULL << 52) - 1));
    uint8_t neg = !!(u.i & (1ULL << 63));

    /* start constructing the string */
    if (neg) {
        OUT('-');
        d = -d;
    }

    /* look for special cases */
    if (exponent == 0x7ff) {
        if (fraction == 0) {
            /* infinity */
            if (flag & CAPSFLAG) OUTSTR("INF");
            else OUTSTR("inf");
        } else {
            /* NaN */
            if (flag & CAPSFLAG) OUTSTR("NAN");
            else OUTSTR("nan");
        }
    } else if (exponent == 0) {
        if (fraction == 0) {
            /* zero */
            OUTSTR("0.000000");
        } else {
            /* denormalized */
            /* XXX does not handle */
            if (flag & CAPSFLAG) OUTSTR("DEN");
            else OUTSTR("den");
        }
    } else {
        /* see if it's in the range of floats we can easily print */
        int exponent_signed = exponent - 1023;
        if (exponent_signed < -52 || exponent_signed > 52) {
            OUTSTR("<range>");
        } else {
            /* start by walking backwards through the string */
#define OUTREV(c) do { if (&buf[pos] == buf) goto done; else buf[--pos] = (c); } while (0)
            pos = len;
            OUTREV(0);

            /* reserve space for the fractional component first */
            for (int i = 0; i <= 6; i++)
                OUTREV('0');
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
                OUTREV('0');
            }

            buf[decimal_spot] = '.';

            /* handle the fractional part */
            uint32_t frac = ((d - u) * 1000000) + .5;

            uint32_t i = decimal_spot + 6 + 1;
            while (frac != 0) {
                uint32_t digit = frac % 10;

                buf[--i] = digit + '0';

                frac /= 10;
            }

            if (neg)
                OUTREV('-');

done:
            /* separate return path, since we've been walking backwards through the string */
            return &buf[pos];
        }
#undef OUTREV
    }

    buf[pos] = 0;
    return buf;
}

static char *double_to_hexstring(char *buf, size_t len, double d, uint32_t flag) {
    size_t pos = 0;
    union double_int u = { d };

    uint32_t exponent = (u.i >> 52) & 0x7ff;
    uint64_t fraction = (u.i & ((1ULL << 52) - 1));
    uint8_t neg = !!(u.i & (1ULL << 63));

    /* start constructing the string */
    if (neg) {
        OUT('-');
    }

    /* look for special cases */
    if (exponent == 0x7ff) {
        if (fraction == 0) {
            /* infinity */
            if (flag & CAPSFLAG) OUTSTR("INF");
            else OUTSTR("inf");
        } else {
            /* NaN */
            if (flag & CAPSFLAG) OUTSTR("NAN");
            else OUTSTR("nan");
        }
    } else if (exponent == 0) {
        if (fraction == 0) {
            /* zero */
            if (flag & CAPSFLAG) OUTSTR("0X0P+0");
            else OUTSTR("0x0p+0");
        } else {
            /* denormalized */
            /* XXX does not handle */
            if (flag & CAPSFLAG) OUTSTR("DEN");
            else OUTSTR("den");
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
        if (flag & CAPSFLAG) OUTSTR("0X1");
        else OUTSTR("0x1");

        /* select the appropriate hex case table */
        const char *table = (flag & CAPSFLAG) ? hextable_caps : hextable;

        int zero_count = 0;
        uint8_t output_dot = 0;
        for (int i = 52 - 4; i >= 0; i -= 4) {
            uint32_t digit = (fraction >> i) & 0xf;

            if (digit == 0) {
                zero_count++;
            } else {
                /* output a . the first time we output a char */
                if (!output_dot) {
                    OUT('.');
                    output_dot = 1;
                }
                /* if we have a non zero digit, see if we need to output a string of zeros */
                while (zero_count > 0) {
                    OUT('0');
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

#undef OUT
#undef OUTSTR



int print_driver(const char *fmt, va_list ap, unsigned int buf[])
{
    int err = 0;
    char c;
    unsigned char uc;
    const char *s;
    size_t string_len;
    unsigned long long n;
    //void *ptr;
    int flags;
    unsigned int format_num;
    char signchar;
    size_t chars_written = 0;
    char num_buffer[32];

    int index = 0;

    for (;;) {
        /* reset the format state */
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
        /* output the string we've accumulated */
        if (string_len > 0)
            OUTPUT_STRING(s, string_len);

        /* make sure we haven't just hit the end of the string */
        if (c == 0)
            break;

next_format:
        /* grab the next format character */
        c = *fmt++;
        if (c == 0)
            break;

        switch (c) {
            case '0'...'9':
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
                uc = va_arg(ap, unsigned int);
                OUTPUT_CHAR(uc);
                OUTPUT_BUF(uc, 1, buf);
                break;
            case 's':
                s = va_arg(ap, const char *);
                OUTPUT_BUF((unsigned int)s, 1, buf);
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
                if (flags & LONGLONGFLAG) {
                    n = va_arg(ap, long long);
                    OUTPUT_BUF(n, 2, buf);
                } else {
                    n = (flags & LONGFLAG) ? va_arg(ap, long) :
                        (flags & HALFHALFFLAG) ? (signed char)va_arg(ap, int) :
                        (flags & HALFFLAG) ? (short)va_arg(ap, int) :
                        (flags & SIZETFLAG) ? va_arg(ap, ssize_t) :
                        (flags & INTMAXFLAG) ? va_arg(ap, intmax_t) :
                        (flags & PTRDIFFFLAG) ? va_arg(ap, ptrdiff_t) :
                        va_arg(ap, int);

                    OUTPUT_BUF(n, 1, buf);
                }
                flags |= SIGNEDFLAG;
                s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags, &signchar);
                goto _output_string;
            case 'u':
                if (flags & LONGLONGFLAG) {
                    n = va_arg(ap, unsigned long long);
                    OUTPUT_BUF(n, 2, buf);
                } else {
                    n = (flags & LONGFLAG) ? va_arg(ap, unsigned long) :
                        (flags & HALFHALFFLAG) ? (unsigned char)va_arg(ap, unsigned int) :
                        (flags & HALFFLAG) ? (unsigned short)va_arg(ap, unsigned int) :
                        (flags & SIZETFLAG) ? va_arg(ap, size_t) :
                        (flags & INTMAXFLAG) ? va_arg(ap, uintmax_t) :
                        (flags & PTRDIFFFLAG) ? (uintptr_t)va_arg(ap, ptrdiff_t) :
                        va_arg(ap, unsigned int);

                    OUTPUT_BUF(n, 1, buf);
                }
                s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags, &signchar);
                goto _output_string;
            case 'p':
                flags |= LONGFLAG | ALTFLAG;
                goto hex;
            case 'X':
                flags |= CAPSFLAG;
                /* fallthrough */
hex:
            case 'x':
                if (flags & LONGLONGFLAG) {
                    n = va_arg(ap, unsigned long long);
                    OUTPUT_BUF(n, 2, buf);
                } else {
                    n = (flags & LONGFLAG) ? va_arg(ap, unsigned long) :
                        (flags & HALFHALFFLAG) ? (unsigned char)va_arg(ap, unsigned int) :
                        (flags & HALFFLAG) ? (unsigned short)va_arg(ap, unsigned int) :
                        (flags & SIZETFLAG) ? va_arg(ap, size_t) :
                        (flags & INTMAXFLAG) ? va_arg(ap, uintmax_t) :
                        (flags & PTRDIFFFLAG) ? (uintptr_t)va_arg(ap, ptrdiff_t) :
                        va_arg(ap, unsigned int);

                    OUTPUT_BUF(n, 1, buf);
                }
                s = longlong_to_hexstring(num_buffer, n, sizeof(num_buffer), flags);
                if (flags & ALTFLAG) {
                    OUTPUT_CHAR('0');
                    OUTPUT_CHAR((flags & CAPSFLAG) ? 'X': 'x');
                }
                goto _output_string;
            case 'n':
                break;
            case 'F':
                flags |= CAPSFLAG;
                /* fallthrough */
            case 'f': {
                          double d = va_arg(ap, double);
                          OUTPUT_BUF(d, 3, buf);
                          s = double_to_string(num_buffer, sizeof(num_buffer), d, flags);
                          goto _output_string;
                      }
            case 'A':
                      flags |= CAPSFLAG;
                      /* fallthrough */
            case 'a': {
                          double d = va_arg(ap, double);
                          OUTPUT_BUF(d, 3, buf);
                          s = double_to_hexstring(num_buffer, sizeof(num_buffer), d, flags);
                          goto _output_string;
                      }

            default:
                      OUTPUT_CHAR('%');
                      OUTPUT_CHAR(c);
                      break;
        }

        /* move on to the next field */
        continue;

        /* shared output code */
_output_string:
        string_len = strlen(s);

        if (flags & LEFTFORMATFLAG) {
            /* left justify the text */
            OUTPUT_STRING(s, string_len);
            unsigned int written = err;
            /* pad to the right (if necessary) */
            for (; format_num > written; format_num--)
                OUTPUT_CHAR(' ');
        } else {
            /* right justify the text (digits) */

            /* if we're going to print a sign digit,
             *                it'll chew up one byte of the format size */
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

    //return index;
exit:
    return (err < 0) ? err : index;
    //return (err < 0) ? err : (int)chars_written;
}

#undef OUTPUT_STRING
#undef OUTPUT_CHAR


static int _vfprintf(const char *fmt, va_list ap)
{
    return print_driver(fmt, ap, NULL);
}

int printk(const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = _vfprintf(fmt, ap);
    va_end(ap);

    return ret;
}
