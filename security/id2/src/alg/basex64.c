/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "basex64.h"

static const unsigned char encoding_table[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

static const unsigned char decoding_table[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static const unsigned char mod_table[] = { 0, 2, 1 };

int basex64_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t* output_len)
{
    int i, j;
    uint32_t o_len = 4 * ((input_len + 2) / 3);

    if (!input || !output || !output_len)
    {
        return -1;
    }

    if (output_len)
    {
        *output_len = o_len;
    }

    for (i = 0, j = 0; i < input_len;)
    {
        unsigned int octet_a = i < input_len ? input[i++] : 0;
        unsigned int octet_b = i < input_len ? input[i++] : 0;
        unsigned int octet_c = i < input_len ? input[i++] : 0;

        unsigned int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = encoding_table[(triple >> 18) & 0x3F];
        output[j++] = encoding_table[(triple >> 12) & 0x3F];
        output[j++] = encoding_table[(triple >> 6) & 0x3F];
        output[j++] = encoding_table[(triple >> 0) & 0x3F];
    }

    for (i = 0; i < mod_table[input_len % 3]; i++)
    {
        output[o_len - 1 - i] = '=';
    }

    return 0;
}

int basex64_decode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t* output_len)
{
    int i, j;
    uint32_t o_len = input_len / 4 * 3;

    if (!input || !output || !output_len)
    {
        return -1;
    }

    if (input[input_len - 1] == '=')
    {
        o_len--;
    }
    if (input[input_len - 2] == '=')
    {
        o_len--;
    }

    if (output_len)
    {
        *output_len = o_len;
    }

    for (i = 0, j = 0; i < input_len;)
    {
        unsigned int sextet_a = decoding_table[input[i++]];
        unsigned int sextet_b = decoding_table[input[i++]];
        unsigned int sextet_c = decoding_table[input[i++]];
        unsigned int sextet_d = decoding_table[input[i++]];

        unsigned int triple = (sextet_a << 18)
                              + (sextet_b << 12)
                              + (sextet_c << 6)
                              + (sextet_d << 0);

        if (j < o_len)
        {
            output[j++] = (triple >> 16) & 0xFF;
        }
        if (j < o_len)
        {
            output[j++] = (triple >> 8) & 0xFF;
        }
        if (j < o_len)
        {
            output[j++] = (triple >> 0) & 0xFF;
        }
    }

    return 0;
}
