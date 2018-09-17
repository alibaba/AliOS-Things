/* The functions in this file are only meant to support Dhrystone on an
 * embedded RV32 system and are obviously incorrect in general. */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#undef putchar
int putchar(int ch)
{
  return write(1, &ch, 1) == 1 ? ch : -1;
}

static void sprintf_putch(int ch, void** data)
{
  char** pstr = (char**)data;
  **pstr = ch;
  (*pstr)++;
}

static unsigned long getuint(va_list *ap, int lflag)
{
  if (lflag)
    return va_arg(*ap, unsigned long);
  else
    return va_arg(*ap, unsigned int);
}

static long getint(va_list *ap, int lflag)
{
  if (lflag)
    return va_arg(*ap, long);
  else
    return va_arg(*ap, int);
}

static inline void printnum(void (*putch)(int, void**), void **putdat,
                    unsigned long num, unsigned base, int width, int padc)
{
  unsigned digs[sizeof(num)*8];
  int pos = 0;

  while (1)
  {
    digs[pos++] = num % base;
    if (num < base)
      break;
    num /= base;
  }

  while (width-- > pos)
    putch(padc, putdat);

  while (pos-- > 0)
    putch(digs[pos] + (digs[pos] >= 10 ? 'a' - 10 : '0'), putdat);
}

static inline void print_double(void (*putch)(int, void**), void **putdat,
                                double num, int width, int prec)
{
  union {
    double d;
    uint64_t u;
  } u;
  u.d = num;

  if (u.u & (1ULL << 63)) {
    putch('-', putdat);
    u.u &= ~(1ULL << 63);
  }

  for (int i = 0; i < prec; i++)
    u.d *= 10;

  char buf[32], *pbuf = buf;
  printnum(sprintf_putch, (void**)&pbuf, (unsigned long)u.d, 10, 0, 0);
  if (prec > 0) {
    for (int i = 0; i < prec; i++) {
      pbuf[-i] = pbuf[-i-1];
    }
    pbuf[-prec] = '.';
    pbuf++;
  }

  for (char* p = buf; p < pbuf; p++)
    putch(*p, putdat);
}

static void vprintfmt(void (*putch)(int, void**), void **putdat, const char *fmt, va_list ap)
{
  register const char* p;
  const char* last_fmt;
  register int ch, err;
  unsigned long num;
  int base, lflag, width, precision, altflag;
  char padc;

  while (1) {
    while ((ch = *(unsigned char *) fmt) != '%') {
      if (ch == '\0')
        return;
      fmt++;
      putch(ch, putdat);
    }
    fmt++;

    // Process a %-escape sequence
    last_fmt = fmt;
    padc = ' ';
    width = -1;
    precision = -1;
    lflag = 0;
    altflag = 0;
  reswitch:
    switch (ch = *(unsigned char *) fmt++) {

    // flag to pad on the right
    case '-':
      padc = '-';
      goto reswitch;
      
    // flag to pad with 0's instead of spaces
    case '0':
      padc = '0';
      goto reswitch;

    // width field
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      for (precision = 0; ; ++fmt) {
        precision = precision * 10 + ch - '0';
        ch = *fmt;
        if (ch < '0' || ch > '9')
          break;
      }
      goto process_precision;

    case '*':
      precision = va_arg(ap, int);
      goto process_precision;

    case '.':
      if (width < 0)
        width = 0;
      goto reswitch;

    case '#':
      altflag = 1;
      goto reswitch;

    process_precision:
      if (width < 0)
        width = precision, precision = -1;
      goto reswitch;

    // long flag
    case 'l':
      if (lflag)
        goto bad;
      goto reswitch;

    // character
    case 'c':
      putch(va_arg(ap, int), putdat);
      break;

    // double
    case 'f':
      print_double(putch, putdat, va_arg(ap, double), width, precision);
      break;

    // string
    case 's':
      if ((p = va_arg(ap, char *)) == NULL)
        p = "(null)";
      if (width > 0 && padc != '-')
        for (width -= strnlen(p, precision); width > 0; width--)
          putch(padc, putdat);
      for (; (ch = *p) != '\0' && (precision < 0 || --precision >= 0); width--) {
        putch(ch, putdat);
        p++;
      }
      for (; width > 0; width--)
        putch(' ', putdat);
      break;

    // (signed) decimal
    case 'd':
      num = getint(&ap, lflag);
      if ((long) num < 0) {
        putch('-', putdat);
        num = -(long) num;
      }
      base = 10;
      goto signed_number;

    // unsigned decimal
    case 'u':
      base = 10;
      goto unsigned_number;

    // (unsigned) octal
    case 'o':
      // should do something with padding so it's always 3 octits
      base = 8;
      goto unsigned_number;

    // pointer
    case 'p':
      lflag = 1;
      putch('0', putdat);
      putch('x', putdat);
      /* fall through to 'x' */

    // (unsigned) hexadecimal
    case 'x':
      base = 16;
    unsigned_number:
      num = getuint(&ap, lflag);
    signed_number:
      printnum(putch, putdat, num, base, width, padc);
      break;

    // escaped '%' character
    case '%':
      putch(ch, putdat);
      break;
      
    // unrecognized escape sequence - just print it literally
    default:
    bad:
      putch('%', putdat);
      fmt = last_fmt;
      break;
    }
  }
}

int __wrap_printf(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vprintfmt((void*)putchar, 0, fmt, ap);

  va_end(ap);
  return 0; // incorrect return value, but who cares, anyway?
}

int __wrap_sprintf(char* str, const char* fmt, ...)
{
  va_list ap;
  char* str0 = str;
  va_start(ap, fmt);

  vprintfmt(sprintf_putch, (void**)&str, fmt, ap);
  *str = 0;

  va_end(ap);
  return str - str0;
}
