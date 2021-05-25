/*
FUNCTION
	<<strtoul>>---string to unsigned long

INDEX
	strtoul
INDEX
	_strtoul_r

ANSI_SYNOPSIS
	#include <stdlib.h>
        unsigned long strtoul(const char *restrict <[s]>, char **restrict <[ptr]>,
                              int <[base]>);

        unsigned long _strtoul_r(void *<[reent]>, const char *restrict <[s]>,
                              char **restrict <[ptr]>, int <[base]>);

TRAD_SYNOPSIS
	#include <stdlib.h>
        unsigned long strtoul(<[s]>, <[ptr]>, <[base]>)
        char *<[s]>;
        char **<[ptr]>;
        int <[base]>;

        unsigned long _strtoul_r(<[reent]>, <[s]>, <[ptr]>, <[base]>)
	char *<[reent]>;
        char *<[s]>;
        char **<[ptr]>;
        int <[base]>;

DESCRIPTION
The function <<strtoul>> converts the string <<*<[s]>>> to
an <<unsigned long>>. First, it breaks down the string into three parts:
leading whitespace, which is ignored; a subject string consisting
of the digits meaningful in the radix specified by <[base]>
(for example, <<0>> through <<7>> if the value of <[base]> is 8);
and a trailing portion consisting of one or more unparseable characters,
which always includes the terminating null character. Then, it attempts
to convert the subject string into an unsigned long integer, and returns the
result.

If the value of <[base]> is zero, the subject string is expected to look
like a normal C integer constant (save that no optional sign is permitted):
a possible <<0x>> indicating hexadecimal radix, and a number.
If <[base]> is between 2 and 36, the expected form of the subject is a
sequence of digits (which may include letters, depending on the
base) representing an integer in the radix specified by <[base]>.
The letters <<a>>--<<z>> (or <<A>>--<<Z>>) are used as digits valued from
10 to 35. If <[base]> is 16, a leading <<0x>> is permitted.

The subject sequence is the longest initial sequence of the input
string that has the expected form, starting with the first
non-whitespace character.  If the string is empty or consists entirely
of whitespace, or if the first non-whitespace character is not a
permissible digit, the subject string is empty.

If the subject string is acceptable, and the value of <[base]> is zero,
<<strtoul>> attempts to determine the radix from the input string. A
string with a leading <<0x>> is treated as a hexadecimal value; a string with
a leading <<0>> and no <<x>> is treated as octal; all other strings are
treated as decimal. If <[base]> is between 2 and 36, it is used as the
conversion radix, as described above. Finally, a pointer to the first
character past the converted subject string is stored in <[ptr]>, if
<[ptr]> is not <<NULL>>.

If the subject string is empty (that is, if <<*>><[s]> does not start
with a substring in acceptable form), no conversion
is performed and the value of <[s]> is stored in <[ptr]> (if <[ptr]> is
not <<NULL>>).

The alternate function <<_strtoul_r>> is a reentrant version.  The
extra argument <[reent]> is a pointer to a reentrancy structure.


RETURNS
<<strtoul>> returns the converted value, if any. If no conversion was
made, <<0>> is returned.

<<strtoul>> returns <<ULONG_MAX>> if the magnitude of the converted
value is too large, and sets <<errno>> to <<ERANGE>>.

PORTABILITY
<<strtoul>> is ANSI.

<<strtoul>> requires no supporting OS subroutines.
*/

/*
 * Copyright (c) 1990 Regents of the University of California.
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
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
LIBC_ROM_TEXT_SECTION
_LONG_CALL_
unsigned long
//_strtoul_r(struct _reent *rptr ,
_strtoul_r(
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
	 * See strtol for comments as to the logic used.
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
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
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
		acc = ULONG_MAX;
		//rptr->_errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? (char *)s - 1 : nptr);
	return (acc);
}

LIBC_ROM_TEXT_SECTION
_LONG_CALL_
unsigned long
_strtoul(_CONST char * s ,
	char ** ptr ,
	int base)
{
	//return _strtoul_r (_REENT, s, ptr, base);
	return _strtoul_r (s, ptr, base);
}
