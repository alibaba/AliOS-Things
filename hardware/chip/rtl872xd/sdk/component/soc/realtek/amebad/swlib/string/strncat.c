/*
FUNCTION
	<<strncat>>---concatenate strings

INDEX
	strncat

ANSI_SYNOPSIS
	#include <string.h>
	char *strncat(char *restrict <[dst]>, const char *restrict <[src]>,
                      size_t <[length]>);

TRAD_SYNOPSIS
	#include <string.h>
	char *strncat(<[dst]>, <[src]>, <[length]>)
	char *<[dst]>;
	char *<[src]>;
	size_t <[length]>;

DESCRIPTION
	<<strncat>> appends not more than <[length]> characters from
	the string pointed to by <[src]> (including the	terminating
	null character) to the end of the string pointed to by
	<[dst]>.  The initial character of <[src]> overwrites the null
	character at the end of <[dst]>.  A terminating null character
	is always appended to the result

WARNINGS
	Note that a null is always appended, so that if the copy is
	limited by the <[length]> argument, the number of characters
	appended to <[dst]> is <<n + 1>>.

RETURNS
	This function returns the initial value of <[dst]>

PORTABILITY
<<strncat>> is ANSI C.

<<strncat>> requires no supporting OS subroutines.

QUICKREF
	strncat ansi pure
*/

#include <section_config.h>
#include <basic_types.h>

#include <string.h>
#include <limits.h>

/* Nonzero if X is aligned on a "long" boundary.  */
#define ALIGNED(X) \
  (((long)X & (sizeof (long) - 1)) == 0)

#if LONG_MAX == 2147483647L
#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
/* Nonzero if X (a long int) contains a NULL byte. */
#define DETECTNULL(X) (((X) - 0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

#ifndef DETECTNULL
#error long int is not a 32bit or 64bit byte
#endif

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
char * _strncat(char *__restrict s1 , const char *__restrict s2 , size_t n)
{
#if defined(PREFER_SIZE_OVER_SPEED)
  char *s = s1;

  while (*s1)
    s1++;
  while (n-- != 0 && (*s1++ = *s2++))
    {
      if (n == 0)
	*s1 = '\0';
    }

  return s;
#else
  char *s = s1;

  /* Skip over the data in s1 as quickly as possible.  */
  if (ALIGNED (s1))
    {
      unsigned long *aligned_s1 = (unsigned long *)s1;
      while (!DETECTNULL (*aligned_s1))
	aligned_s1++;

      s1 = (char *)aligned_s1;
    }

  while (*s1)
    s1++;

  /* s1 now points to the its trailing null character, now copy
     up to N bytes from S2 into S1 stopping if a NULL is encountered
     in S2.

     It is not safe to use strncpy here since it copies EXACTLY N
     characters, NULL padding if necessary.  */
  while (n-- != 0 && (*s1++ = *s2++))
    {
      if (n == 0)
	*s1 = '\0';
    }
	
  return s;
#endif /* not PREFER_SIZE_OVER_SPEED */
}
