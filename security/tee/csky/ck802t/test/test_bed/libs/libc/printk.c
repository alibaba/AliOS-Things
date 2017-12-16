/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 *
 */

#include <stdarg.h>
#include "test_bed.h"

#define BUF_SZ  (128)

extern void putch(int8_t ch);

int32_t vsnprintk(int8_t *buf, uint32_t size, const int8_t *fmt, va_list ap)
{
    int8_t *p;
    int32_t i, base, idx, width, sign = 0, lead, uc = 0;
    int32_t val, flag;
    uint32_t uval;
    int8_t *s, *digit;
    int8_t tmp[20];

    p = (int8_t *)fmt;
    /* keep one char for '\0' */
    size--;

    for (i = 0; i < size && *p; p++) {
        if ('%' != *p) {
            buf[i++] = *p;
            continue;
        }
        p++;
        if ('0' == *p) {
            lead = 1;
            p++;
        } else {
            lead = 0;
        }
        width = 0;
        while (*p >= '0' && *p <= '9') {
            width *= 10;
            width += (*p - '0');
            p++;
        }
        /* skip 'l' */
        if ('l' == *p) {
            p++;
        }
        sign = uc = 0;
        switch (*p) {
            case '%':
                buf[i++] = '%';
                continue;
            case 'c':
                val = va_arg(ap, int32_t);
                buf[i++] = val & 0xFF;
                continue;
            case 's':
                s = va_arg(ap, int8_t *);
                if (NULL == s) {
                    s = (int8_t *)"NULL";
                }
                while (i < size && *s) {
                    buf[i++] = *s++;
                }
                continue;
            case 'd':
                sign = 1;
            case 'u':
                base = 10;
                break;
            case 'X':
                uc = 1;
            case 'x':
            case 'p':
                base = 16;
                break;
            default:
                buf[i++] = '.';
                continue;
        }
        if (uc) {
            digit = (int8_t *)"0123456789ABCDEF";
        } else {
            digit = (int8_t *)"0123456789abcdef";
        }
        flag = 0;
        val = va_arg(ap, int32_t);
        if (sign && val < 0) {
            val = -val;
            flag = 1;
        }

        uval = (uint32_t)val;
        idx = 0;
        do {
            tmp[idx++] = digit[uval%base];
            uval /= base;
        } while (uval);

        if (width && flag) {
            width--;
        }

        width -= idx;

        if (lead) {
            while (width-- > 0) {
                tmp[idx++] = '0';
            }
        }
        if (flag) {
            tmp[idx++] = '-';
        }
        while (width-- > 0) {
            tmp[idx++] = ' ';
        }
        while (i < size && idx > 0) {
            buf[i++] = tmp[--idx];
        }
    }
    buf[i] = '\0';
    return i;
}

static void delay (int sec)
{
    int i;
    volatile int j;

    for (i = 0x00; i < sec * 100; i ++)
        j = i;
}

int32_t printk(const int8_t *fmt, ...)
{
    va_list args;
    int8_t buf[BUF_SZ];
    int8_t *ptr;

    //FIXME, only used in ck-sky
    delay(1);

    va_start (args, fmt);
    vsnprintk(buf, sizeof(buf) - 1, fmt, args);
    va_end(args);

    buf[BUF_SZ - 1] = '\0';

    ptr = buf;
    while (*ptr) {
        putch(*ptr);
        ++ptr;
    }

    return strlen(buf);
}


static void uart_putc(unsigned char c)
{
    putch((char)c);
}

static void hex_to_char(unsigned char data, char *h, char *l)
{
    unsigned char tmp = data;
    char h_tmp, l_tmp;

    tmp &= 0xff;
    l_tmp = (((tmp & 0xf) > 9) ? ('a' + ((tmp & 0xf) - 10) ) :
                                ( '0' + (tmp & 0xf) ) );
    h_tmp = ((((tmp >> 4) & 0xf) > 9) ? ('a' + (((tmp >> 4) & 0xf) - 10) ) :
                                ( '0' + ((tmp >> 4) & 0xf) ) );
    *h = h_tmp;
    *l = l_tmp;
    return;
}

static void uart_print_uint8(unsigned char data)
{
    char l, h;

    hex_to_char(data, &h, &l);
    uart_putc('0');
    uart_putc('x');
    uart_putc(h);
    uart_putc(l);
}

#if 0 /* NOT USED TILL NOW */
void uart_print_uint32(unsigned int data)
{
    char l, h;
    unsigned char tmp;

    uart_putc('0');
    uart_putc('x');
    tmp = ((data >> 24) & 0xff);
    hex_to_char(tmp, &h, &l);
    uart_putc(h);
    uart_putc(l);
    tmp = ((data >> 16) & 0xff);
    hex_to_char(tmp, &h, &l);
    uart_putc(h);
    uart_putc(l);
    tmp = ((data >> 8) & 0xff);
    hex_to_char(tmp, &h, &l);
    uart_putc(h);
    uart_putc(l);
    tmp = ((data >> 0) & 0xff);
    hex_to_char(tmp, &h, &l);
    uart_putc(h);
    uart_putc(l);
}

#endif

static void uart_print_string(char *data)
{
    unsigned int i = 0;
    while(data[i]) {
        if (data[i] == '\n')
            uart_putc('\r');
        uart_putc(data[i++]);
    }
}

void dump_data(char *prefix, unsigned char *data, unsigned int size)
{
    unsigned int i;

    uart_print_string(prefix);
    uart_print_string(" :\n");

    for (i=0; i<size; i++) {
        if ((i&0xf) == 0) {
            uart_print_string("\n");
        }
        uart_print_uint8(data[i]);
        uart_print_string(" ");
    }
    uart_print_string("\n");
}

void dump_code(char *name, unsigned char *data, unsigned int size)
{
    unsigned int i;

    uart_print_string("unsigned char ");
    uart_print_string(name);
    uart_print_string("[] = {");

    for (i=0; i<size; i++) {
        if ((i&0xf) == 0) {
            uart_print_string("\n");
        }
        uart_print_uint8(data[i]);
        uart_print_string(", ");
    }
    uart_print_string("};\n");
}
