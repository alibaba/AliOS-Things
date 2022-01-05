/******************************************************************************
*
* Copyright(c) 2007 - 2021 Realtek Corporation. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/

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
