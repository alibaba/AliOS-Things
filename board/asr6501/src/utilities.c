/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Helper functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "utilities.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void srand1( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
  while( size-- )
  {
    *dst++ = value;
  }
}

int8_t Nibble2HexChar( uint8_t a )
{
  if( a < 10 )
  {
    return '0' + a;
  }
  else if( a < 16 )
  {
    return 'A' + ( a - 10 );
  }
  else
  {
    return '?';
  }
}

char *strtok(char *str, const char * delim)
{
    static char *lasts;
    char *ret;
    char ch;

    if (!str) {
        str = lasts;
    }

    ret = str;

    do {
        ch = *str++;
        if (ch == '\0') {
            return ret;
        }
    } while (ch != *delim);

    lasts = str;

    return ret;
}

int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
    const char *hex_end    = hex + bin_length * 2;
    uint8_t    *cur        = bin;
    uint8_t     byte       = 0;
    uint8_t     num_chars  = 0;

    while (hex < hex_end) {
        if ('A' <= *hex && *hex <= 'F') {
            byte |= 10 + (*hex - 'A');
        } else if ('a' <= *hex && *hex <= 'f') {
            byte |= 10 + (*hex - 'a');
        } else if ('0' <= *hex && *hex <= '9') {
            byte |= *hex - '0';
        } else {
            return -1;
        }
        hex++;

        num_chars++;

        if (num_chars >= 2) {
            num_chars = 0;
            *cur++    = byte;
            byte      = 0;
        } else {
            byte <<= 4;
        }
    }
    return cur - bin;
}


