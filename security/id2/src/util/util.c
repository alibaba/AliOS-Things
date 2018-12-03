/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "util/util.h"

const static char* const ascii_table = "0123456789ABCDEF";

/* each of 0, 1, 2, ... -> "0123456789ABCDEF"*/
int hex_to_char(uint8_t hex)
{
	int ch = -1;
	if(hex <= 0x0F)
	{
		ch = ascii_table[hex];
	}
	return ch;
}

/* each of "0123456789ABCDEF" -> 0, 1, 2, ... */
int char_to_hex(char c)
{
	int hex = -1;
	if((c >= '0') && (c <= '9'))
	{
		hex = c - '0';
	}
	else if ((c >= 'a') && (c <= 'f'))
	{
		hex = c - 'a' + 10;
	}
	else if ((c >= 'A') && (c <= 'F'))
	{
		hex = c - 'A' + 10;
	}
	else
	{
	}
	return hex;
}

int hex_to_ascii(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len)
{
    int32_t i;
    uint8_t v;
    uint8_t high, low;

    for (i = in_len - 1; i >= 0; i--)
    {
        v = in_data[i];
        high = (v >> 4) & 0x0F;
        low = (v & 0x0F);

        out_buf[(i << 1)] = ascii_table[high];
        out_buf[(i << 1) + 1] = ascii_table[low];
    }
    *out_len = in_len << 1;
	return 0;
}


int ascii_to_hex(const uint8_t* in_data, uint32_t in_len, uint8_t* out_buf, uint32_t* out_len)
{
	uint32_t i = 0;
	uint32_t j = 0;
	int h;
	int l;
	if ((in_data == NULL) || (out_buf == NULL) || (in_len % 2) != 0)
	{
		return -1;
	}
	if ((in_len >> 1) > *out_len)
	{
		return -1;
	}
	while (i < in_len)
	{
		h = char_to_hex(in_data[i]);
		if (h < 0)
		{
			return -1;
		}
		l = char_to_hex(in_data[i + 1]);
		if (l < 0)
		{
			return -1;
		}
		out_buf[j++] = (uint8_t)((h << 4) | (l & 0x0F));
		i += 2;
	}
	*out_len = j;
	return 0;
}

int crc16_ccit_false(const uint8_t* in_data, uint32_t in_len, uint16_t init_value, uint8_t* pout)
{
	uint16_t poly = init_value;
	uint16_t wcrc = 0xFFFF;
	uint32_t i;
	int j;

	for (i = 0; i < in_len; ++i)
	{
		wcrc ^= (in_data[i] << 8);
		for (j = 0; j < 8; ++j)
		{
			if (wcrc & 0x8000)
			{
				wcrc = (wcrc << 1) ^ poly;
			}
			else
			{
				wcrc <<= 1;
			}
		}
	}
	wcrc ^= 0xFFFF;
	pout[0] = (uint8_t)(wcrc >> 8);
	pout[1] = (uint8_t)(wcrc & 0xFF);
	return 0;
}

int pkcs5_pading(uint8_t* in_data, uint32_t in_len, uint32_t* out_len, uint8_t block_len)
{
	uint8_t fill_len;
	uint8_t fill_value;

	fill_len = block_len - (in_len % block_len);
	fill_value = fill_len;

	while (fill_len)
	{
		in_data[in_len++] = fill_value;
		fill_len--;
	}
	*out_len = in_len;
	return 0;
}

int pkcs5_unpading(uint8_t* in_data, uint32_t in_len, uint32_t* out_len, uint8_t block_len)
{
	int i;
	uint8_t v;
	uint8_t pading_len;

	if ((in_len == 0) || ((in_len % block_len) != 0))
	{
		return -1;
	}

	i = in_len - 1;
	v = in_data[i];
	pading_len = v;
	if (pading_len > block_len)
	{
		return -1;
	}
	while (pading_len-- > 0)
	{
		if (in_data[i--] != v)
		{
			return -1;
		}
	}
	*out_len = in_len - v;
	return 0;
}
