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

/*
FUNCTION
	<<strstr>>---find string segment

INDEX
	strstr

ANSI_SYNOPSIS
	#include <string.h>
	char *strstr(const char *<[s1]>, const char *<[s2]>);

TRAD_SYNOPSIS
	#include <string.h>
	char *strstr(<[s1]>, <[s2]>)
	char *<[s1]>;
	char *<[s2]>;

DESCRIPTION
	Locates the first occurrence in the string pointed to by <[s1]> of
	the sequence of characters in the string pointed to by <[s2]>
	(excluding the terminating null character).

RETURNS
	Returns a pointer to the located string segment, or a null
	pointer if the string <[s2]> is not found. If <[s2]> points to
	a string with zero length, <[s1]> is returned.

PORTABILITY
<<strstr>> is ANSI C.

<<strstr>> requires no supporting OS subroutines.

QUICKREF
	strstr ansi pure
*/

#include <section_config.h>
#include <basic_types.h>

#include <string.h>

extern char * _strchr(const char *s1 , int i);

#if !defined(PREFER_SIZE_OVER_SPEED)
# define RETURN_TYPE char *
# define AVAILABLE(h, h_l, j, n_l)			\
  (! _memchr ((h) + (h_l), '\0', (j) + (n_l) - (h_l))	\
   && ((h_l) = (j) + (n_l)))
# include "str-two-way.h"
#endif

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
char * _strstr(const char *searchee , const char *lookfor)
{
#if defined(PREFER_SIZE_OVER_SPEED)

  /* Less code size, but quadratic performance in the worst case.  */
  if (*searchee == 0)
    {
      if (*lookfor)
	return (char *) NULL;
      return (char *) searchee;
    }

  while (*searchee)
    {
      size_t i;
      i = 0;

      while (1)
	{
	  if (lookfor[i] == 0)
	    {
	      return (char *) searchee;
	    }

	  if (lookfor[i] != searchee[i])
	    {
	      break;
	    }
	  i++;
	}
      searchee++;
    }

  return (char *) NULL;

#else /* compilation for speed */

  /* Larger code size, but guaranteed linear performance.  */
  const char *haystack = searchee;
  const char *needle = lookfor;
  size_t needle_len; /* Length of NEEDLE.  */
  size_t haystack_len; /* Known minimum length of HAYSTACK.  */
  int ok = 1; /* True if NEEDLE is prefix of HAYSTACK.  */

  /* Determine length of NEEDLE, and in the process, make sure
     HAYSTACK is at least as long (no point processing all of a long
     NEEDLE if HAYSTACK is too short).  */
  while (*haystack && *needle)
    ok &= *haystack++ == *needle++;
  if (*needle)
    return NULL;
  if (ok)
    return (char *) searchee;

  /* Reduce the size of haystack using strchr, since it has a smaller
     linear coefficient than the Two-Way algorithm.  */
  needle_len = needle - lookfor;
  haystack = _strchr (searchee + 1, *lookfor);
  if (!haystack || needle_len == 1)
    return (char *) haystack;
  haystack_len = (haystack > searchee + needle_len ? 1
		  : needle_len + searchee - haystack);

  /* Perform the search.  */
  if (needle_len < LONG_NEEDLE_THRESHOLD)
    return __rtl_two_way_short_needle ((const unsigned char *) haystack,
				 haystack_len,
				 (const unsigned char *) lookfor, needle_len);
  return __rtl_two_way_long_needle ((const unsigned char *) haystack, haystack_len,
			      (const unsigned char *) lookfor, needle_len);
#endif /* compilation for speed */
}
