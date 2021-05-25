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

/**
 * stricmp - A small but sufficient implementation for case insensitive strcmp.
 * @str1:
 * @str2:
 * @note support from B CUT
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_ int _stricmp(const char* str1, const char* str2)
{
	char c1, c2;

	do {
		c1 = *str1++;
		c2 = *str2++;
		if (c1 != c2) {
			char c1_upc = c1 | 0x20;
			if ((c1_upc >= 'a') && (c1_upc <= 'z')) {
				/* characters are not equal an one is in the alphabet range:
				downcase both chars and check again */
				char c2_upc = c2 | 0x20;
				if (c1_upc != c2_upc) {
					/* still not equal */
					/* don't care for < or > */
					return 1;
				}
			} else {
				/* characters are not equal but none is in the alphabet range */
				return 1;
			}
		}
	} while (c1 != 0);
	return 0;
}
