/**
 * @file ch.c Character format functions
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re_types.h>
#include <re_fmt.h>


/**
 * Convert an ASCII hex character to binary format
 *
 * @param ch ASCII hex character
 *
 * @return Binary value
 */
uint8_t ch_hex(char ch)
{
	if ('0' <= ch && ch <= '9')
		return ch - '0';

	else if ('A' <= ch && ch <= 'F')
		return ch - 'A' + 10;

	else if ('a' <= ch && ch <= 'f')
		return ch - 'a' + 10;

	return 0;
}
