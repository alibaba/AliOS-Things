/*
FUNCTION
   <<strtol>>---string to long

INDEX
	strtol
INDEX
	_strtol_r

ANSI_SYNOPSIS
	#include <stdlib.h>
        long strtol(const char *restrict <[s]>, char **restrict <[ptr]>,int <[base]>);

        long _strtol_r(void *<[reent]>, 
                       const char *restrict <[s]>, char **restrict <[ptr]>,int <[base]>);

TRAD_SYNOPSIS
	#include <stdlib.h>
	long strtol (<[s]>, <[ptr]>, <[base]>)
        char *<[s]>;
        char **<[ptr]>;
        int <[base]>;

	long _strtol_r (<[reent]>, <[s]>, <[ptr]>, <[base]>)
	char *<[reent]>;
        char *<[s]>;
        char **<[ptr]>;
        int <[base]>;

DESCRIPTION
The function <<strtol>> converts the string <<*<[s]>>> to
a <<long>>. First, it breaks down the string into three parts:
leading whitespace, which is ignored; a subject string consisting
of characters resembling an integer in the radix specified by <[base]>;
and a trailing portion consisting of zero or more unparseable characters,
and always including the terminating null character. Then, it attempts
to convert the subject string into a <<long>> and returns the
result.

If the value of <[base]> is 0, the subject string is expected to look
like a normal C integer constant: an optional sign, a possible `<<0x>>'
indicating a hexadecimal base, and a number. If <[base]> is between
2 and 36, the expected form of the subject is a sequence of letters
and digits representing an integer in the radix specified by <[base]>,
with an optional plus or minus sign. The letters <<a>>--<<z>> (or,
equivalently, <<A>>--<<Z>>) are used to signify values from 10 to 35;
only letters whose ascribed values are less than <[base]> are
permitted. If <[base]> is 16, a leading <<0x>> is permitted.

The subject sequence is the longest initial sequence of the input
string that has the expected form, starting with the first
non-whitespace character.  If the string is empty or consists entirely
of whitespace, or if the first non-whitespace character is not a
permissible letter or digit, the subject string is empty.

If the subject string is acceptable, and the value of <[base]> is zero,
<<strtol>> attempts to determine the radix from the input string. A
string with a leading <<0x>> is treated as a hexadecimal value; a string with
a leading 0 and no <<x>> is treated as octal; all other strings are
treated as decimal. If <[base]> is between 2 and 36, it is used as the
conversion radix, as described above. If the subject string begins with
a minus sign, the value is negated. Finally, a pointer to the first
character past the converted subject string is stored in <[ptr]>, if
<[ptr]> is not <<NULL>>.

If the subject string is empty (or not in acceptable form), no conversion
is performed and the value of <[s]> is stored in <[ptr]> (if <[ptr]> is
not <<NULL>>).

The alternate function <<_strtol_r>> is a reentrant version.  The
extra argument <[reent]> is a pointer to a reentrancy structure.

RETURNS
<<strtol>> returns the converted value, if any. If no conversion was
made, 0 is returned.

<<strtol>> returns <<LONG_MAX>> or <<LONG_MIN>> if the magnitude of
the converted value is too large, and sets <<errno>> to <<ERANGE>>.

PORTABILITY
<<strtol>> is ANSI.

No supporting OS subroutines are required.
*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <section_config.h>
#include <basic_types.h>
#include <strproc.h>

#include <string.h>
#include <limits.h>

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_
long
_strtol_r (
	_CONST char * nptr ,
	char ** endptr ,
	int base)
{
	register const unsigned char *s = (const unsigned char *)nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
               if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		//rptr->_errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? (char *)s - 1 : nptr);
	return (acc);
}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
long
_strtol(_CONST char *__restrict s ,
	char **__restrict ptr ,
	int base)
{
	return _strtol_r (s, ptr, base);
}
