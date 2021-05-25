/*
FUNCTION
        <<strtod>>, <<strtof>>---string to double or float

INDEX
	strtod
INDEX
	_strtod_r
INDEX
	strtof

ANSI_SYNOPSIS
        #include <stdlib.h>
        double strtod(const char *restrict <[str]>, char **restrict <[tail]>);
        float strtof(const char *restrict <[str]>, char **restrict <[tail]>);

        double _strtod_r(void *<[reent]>,
                         const char *restrict <[str]>, char **restrict <[tail]>);

TRAD_SYNOPSIS
        #include <stdlib.h>
        double strtod(<[str]>,<[tail]>)
        char *<[str]>;
        char **<[tail]>;

        float strtof(<[str]>,<[tail]>)
        char *<[str]>;
        char **<[tail]>;

        double _strtod_r(<[reent]>,<[str]>,<[tail]>)
	char *<[reent]>;
        char *<[str]>;
        char **<[tail]>;

DESCRIPTION
	The function <<strtod>> parses the character string <[str]>,
	producing a substring which can be converted to a double
	value.  The substring converted is the longest initial
	subsequence of <[str]>, beginning with the first
	non-whitespace character, that has one of these formats:
	.[+|-]<[digits]>[.[<[digits]>]][(e|E)[+|-]<[digits]>]
	.[+|-].<[digits]>[(e|E)[+|-]<[digits]>]
	.[+|-](i|I)(n|N)(f|F)[(i|I)(n|N)(i|I)(t|T)(y|Y)]
	.[+|-](n|N)(a|A)(n|N)[<(>[<[hexdigits]>]<)>]
	.[+|-]0(x|X)<[hexdigits]>[.[<[hexdigits]>]][(p|P)[+|-]<[digits]>]
	.[+|-]0(x|X).<[hexdigits]>[(p|P)[+|-]<[digits]>]
	The substring contains no characters if <[str]> is empty, consists
	entirely of whitespace, or if the first non-whitespace
	character is something other than <<+>>, <<->>, <<.>>, or a
	digit, and cannot be parsed as infinity or NaN. If the platform
	does not support NaN, then NaN is treated as an empty substring.
	If the substring is empty, no conversion is done, and
	the value of <[str]> is stored in <<*<[tail]>>>.  Otherwise,
	the substring is converted, and a pointer to the final string
	(which will contain at least the terminating null character of
	<[str]>) is stored in <<*<[tail]>>>.  If you want no
	assignment to <<*<[tail]>>>, pass a null pointer as <[tail]>.
	<<strtof>> is identical to <<strtod>> except for its return type.

	This implementation returns the nearest machine number to the
	input decimal string.  Ties are broken by using the IEEE
	round-even rule.  However, <<strtof>> is currently subject to
	double rounding errors.

	The alternate function <<_strtod_r>> is a reentrant version.
	The extra argument <[reent]> is a pointer to a reentrancy structure.

RETURNS
	<<strtod>> returns the converted substring value, if any.  If
	no conversion could be performed, 0 is returned.  If the
	correct value is out of the range of representable values,
	plus or minus <<HUGE_VAL>> is returned, and <<ERANGE>> is
	stored in errno. If the correct value would cause underflow, 0
	is returned and <<ERANGE>> is stored in errno.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998-2001 by Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

****************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to ".").	*/

/* Original file gdtoa-strtod.c Modified 06-21-2006 by Jeff Johnston to work within newlib.  */

#include <_ansi.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "mprec.h"
#include "gdtoa.h"
#include "gd_qnan.h"

/* #ifndef NO_FENV_H */
/* #include <fenv.h> */
/* #endif */

#include "locale.h"


#ifdef IEEE_Arith
#ifndef NO_IEEE_Scale
#define Avoid_Underflow
#undef tinytens
/* The factor of 2^106 in tinytens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
static _CONST double tinytens[] = { 1e-16, 1e-32,
#ifdef _DOUBLE_IS_32BITS
				    0.0, 0.0, 0.0
#else
				    1e-64, 1e-128,
				    9007199254740992. * 9007199254740992.e-256
#endif
				  };

#endif
#endif

#ifdef Honor_FLT_ROUNDS
#define Rounding rounding
#undef Check_FLT_ROUNDS
#define Check_FLT_ROUNDS
#else
#define Rounding Flt_Rounds
#endif

#ifdef Avoid_Underflow /*{*/
 static double
_DEFUN (sulp, (x, scale),
       	U x _AND
	int scale)
{
        U u;
        double rv;
        int i;

        rv = ulp(dval(x));
        if (!scale || (i = 2*P + 1 - ((dword0(x) & Exp_mask) >> Exp_shift)) <= 0)
                return rv; /* Is there an example where i <= 0 ? */
        dword0(u) = Exp_1 + (i << Exp_shift);
#ifndef _DOUBLE_IS_32BITS
        dword1(u) = 0;
#endif
        return rv * u.d;
        }
#endif /*}*/


#ifndef NO_HEX_FP

HAL_ROM_TEXT_SECTION _LONG_CALL_ 
static void
ULtod(__ULong *L ,
	__ULong *bits ,
	Long exp ,
	int k)
{
	switch(k & STRTOG_Retmask) {
	  case STRTOG_NoNumber:
	  case STRTOG_Zero:
		L[0] = L[1] = 0;
		break;

	  case STRTOG_Denormal:
		L[_1] = bits[0];
		L[_0] = bits[1];
		break;

	  case STRTOG_Normal:
	  case STRTOG_NaNbits:
		L[_1] = bits[0];
		L[_0] = (bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20);
		break;

	  case STRTOG_Infinite:
		L[_0] = 0x7ff00000;
		L[_1] = 0;
		break;

	  case STRTOG_NaN:
		L[_0] = 0x7fffffff;
		L[_1] = (__ULong)-1;
	  }
	if (k & STRTOG_Neg)
		L[_0] |= 0x80000000L;
}
#endif /* !NO_HEX_FP */

#ifdef INFNAN_CHECK
HAL_ROM_TEXT_SECTION _LONG_CALL_ 
static int
match(
	_CONST char **sp ,
	char *t)
{
	int c, d;
	_CONST char *s = *sp;

	while( (d = *t++) !=0) {
		if ((c = *++s) >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		if (c != d)
			return 0;
		}
	*sp = s + 1;
	return 1;
}
#endif /* INFNAN_CHECK */

HAL_ROM_TEXT_SECTION _LONG_CALL_ 
double
_strtod_r(
	struct _reent *ptr ,
	_CONST char *__restrict s00 ,
	char **__restrict se)
{
#ifdef Avoid_Underflow
	int scale;
#endif
	int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, dsign,
		 e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
	_CONST char *s, *s0, *s1;
	double aadj, adj;
	U aadj1, rv, rv0;
	Long L;
	__ULong y, z;
	_Bigint *bb = NULL, *bb1, *bd = NULL, *bd0, *bs = NULL, *delta = NULL;
#ifdef Avoid_Underflow
	__ULong Lsb, Lsb1;
#endif
#ifdef SET_INEXACT
	int inexact, oldinexact;
#endif
#ifdef Honor_FLT_ROUNDS
	int rounding;
#endif

	delta = bs = bd = NULL;
	sign = nz0 = nz = decpt = 0;
	dval(rv) = 0.;
	for(s = s00;;s++) switch(*s) {
		case '-':
			sign = 1;
			/* no break */
		case '+':
			if (*++s)
				goto break2;
			/* no break */
		case 0:
			goto ret0;
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
			continue;
		default:
			goto break2;
		}
 break2:
	if (*s == '0') {
#ifndef NO_HEX_FP
		{
		static _CONST FPI fpi = { 53, 1-1023-53+1, 2046-1023-53+1, 1, SI };
		Long exp;
		__ULong bits[2];
		switch(s[1]) {
		  case 'x':
		  case 'X':
			/* If the number is not hex, then the parse of
                           0 is still valid.  */
			s00 = s + 1;
			{
#if defined(FE_DOWNWARD) && defined(FE_TONEAREST) && defined(FE_TOWARDZERO) && defined(FE_UPWARD)
			FPI fpi1 = fpi;
			switch(fegetround()) {
			  case FE_TOWARDZERO:	fpi1.rounding = 0; break;
			  case FE_UPWARD:	fpi1.rounding = 2; break;
			  case FE_DOWNWARD:	fpi1.rounding = 3;
			  }
#else
#define fpi1 fpi
#endif
			switch((i = gethex(ptr, &s, &fpi1, &exp, &bb, sign)) & STRTOG_Retmask) {
			  case STRTOG_NoNumber:
				s = s00;
				sign = 0;
				/* FALLTHROUGH */
			  case STRTOG_Zero:
				break;
			  default:
				if (bb) {
					copybits(bits, fpi.nbits, bb);
					Bfree(ptr,bb);
					}
				ULtod(rv.i, bits, exp, i);
			  }}
			goto ret;
		  }
		}
#endif
		nz0 = 1;
		while(*++s == '0') ;
		if (!*s)
			goto ret;
		}
	s0 = s;
	y = z = 0;
	for(nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++)
		if (nd < 9)
			y = 10*y + c - '0';
		else
			z = 10*z + c - '0';
	nd0 = nd;
	if (strncmp (s, _localeconv_r (ptr)->decimal_point,
		     strlen (_localeconv_r (ptr)->decimal_point)) == 0)
		{
		decpt = 1;
		c = *(s += strlen (_localeconv_r (ptr)->decimal_point));
		if (!nd) {
			for(; c == '0'; c = *++s)
				nz++;
			if (c > '0' && c <= '9') {
				s0 = s;
				nf += nz;
				nz = 0;
				goto have_dig;
				}
			goto dig_done;
			}
		for(; c >= '0' && c <= '9'; c = *++s) {
 have_dig:
			nz++;
			if (c -= '0') {
				nf += nz;
				for(i = 1; i < nz; i++)
					if (nd++ < 9)
						y *= 10;
					else if (nd <= DBL_DIG + 1)
						z *= 10;
				if (nd++ < 9)
					y = 10*y + c;
				else if (nd <= DBL_DIG + 1)
					z = 10*z + c;
				nz = 0;
				}
			}
		}
 dig_done:
	e = 0;
	if (c == 'e' || c == 'E') {
		if (!nd && !nz && !nz0) {
			goto ret0;
			}
		s00 = s;
		esign = 0;
		switch(c = *++s) {
			case '-':
				esign = 1;
			case '+':
				c = *++s;
			}
		if (c >= '0' && c <= '9') {
			while(c == '0')
				c = *++s;
			if (c > '0' && c <= '9') {
				L = c - '0';
				s1 = s;
				while((c = *++s) >= '0' && c <= '9')
					L = 10*L + c - '0';
				if (s - s1 > 8 || L > 19999)
					/* Avoid confusion from exponents
					 * so large that e might overflow.
					 */
					e = 19999; /* safe for 16 bit ints */
				else
					e = (int)L;
				if (esign)
					e = -e;
				}
			else
				e = 0;
			}
		else
			s = s00;
		}
	if (!nd) {
		if (!nz && !nz0) {
#ifdef INFNAN_CHECK
			/* Check for Nan and Infinity */
			__ULong bits[2];
			static _CONST FPI fpinan =	/* only 52 explicit bits */
				{ 52, 1-1023-53+1, 2046-1023-53+1, 1, SI };
			if (!decpt)
			 switch(c) {
			  case 'i':
			  case 'I':
				if (match(&s,"nf")) {
					--s;
					if (!match(&s,"inity"))
						++s;
					dword0(rv) = 0x7ff00000;
#ifndef _DOUBLE_IS_32BITS
					dword1(rv) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
					goto ret;
					}
				break;
			  case 'n':
			  case 'N':
				if (match(&s, "an")) {
#ifndef No_Hex_NaN
					if (*s == '(' /*)*/
					 && hexnan(&s, &fpinan, bits)
							== STRTOG_NaNbits) {
						dword0(rv) = 0x7ff00000 | bits[1];
#ifndef _DOUBLE_IS_32BITS
						dword1(rv) = bits[0];
#endif /*!_DOUBLE_IS_32BITS*/
						}
					else {
#endif
						dword0(rv) = NAN_WORD0;
#ifndef _DOUBLE_IS_32BITS
						dword1(rv) = NAN_WORD1;
#endif /*!_DOUBLE_IS_32BITS*/
#ifndef No_Hex_NaN
						}
#endif
					goto ret;
					}
			  }
#endif /* INFNAN_CHECK */
 ret0:
			s = s00;
			sign = 0;
			}
		goto ret;
		}
	e1 = e -= nf;

	/* Now we have nd0 digits, starting at s0, followed by a
	 * decimal point, followed by nd-nd0 digits.  The number we're
	 * after is the integer represented by those digits times
	 * 10**e */

	if (!nd0)
		nd0 = nd;
	k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
	dval(rv) = y;
	if (k > 9) {
#ifdef SET_INEXACT
		if (k > DBL_DIG)
			oldinexact = get_inexact();
#endif
		dval(rv) = tens[k - 9] * dval(rv) + z;
		}
	bd0 = 0;
	if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
#ifndef Honor_FLT_ROUNDS
		&& Flt_Rounds == 1
#endif
#endif
			) {
		if (!e)
			goto ret;
		if (e > 0) {
			if (e <= Ten_pmax) {
#ifdef VAX
				goto vax_ovfl_check;
#else
#ifdef Honor_FLT_ROUNDS
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					dval(rv) = -dval(rv);
					sign = 0;
					}
#endif
				/* rv = */ rounded_product(dval(rv), tens[e]);
				goto ret;
#endif
				}
			i = DBL_DIG - nd;
			if (e <= Ten_pmax + i) {
				/* A fancier test would sometimes let us do
				 * this for larger i values.
				 */
#ifdef Honor_FLT_ROUNDS
				/* round correctly FLT_ROUNDS = 2 or 3 */
				if (sign) {
					dval(rv) = -dval(rv);
					sign = 0;
					}
#endif
				e -= i;
				dval(rv) *= tens[i];
#ifdef VAX
				/* VAX exponent range is so narrow we must
				 * worry about overflow here...
				 */
 vax_ovfl_check:
				dword0(rv) -= P*Exp_msk1;
				/* rv = */ rounded_product(dval(rv), tens[e]);
				if ((dword0(rv) & Exp_mask)
				 > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
					goto ovfl;
				dword0(rv) += P*Exp_msk1;
#else
				/* rv = */ rounded_product(dval(rv), tens[e]);
#endif
				goto ret;
				}
			}
#ifndef Inaccurate_Divide
		else if (e >= -Ten_pmax) {
#ifdef Honor_FLT_ROUNDS
			/* round correctly FLT_ROUNDS = 2 or 3 */
			if (sign) {
				dval(rv) = -dval(rv);
				sign = 0;
				}
#endif
			/* rv = */ rounded_quotient(dval(rv), tens[-e]);
			goto ret;
			}
#endif
		}
	e1 += nd - k;

#ifdef IEEE_Arith
#ifdef SET_INEXACT
	inexact = 1;
	if (k <= DBL_DIG)
		oldinexact = get_inexact();
#endif
#ifdef Avoid_Underflow
	scale = 0;
#endif
#ifdef Honor_FLT_ROUNDS
	if ((rounding = Flt_Rounds) >= 2) {
		if (sign)
			rounding = rounding == 2 ? 0 : 2;
		else
			if (rounding != 2)
				rounding = 0;
		}
#endif
#endif /*IEEE_Arith*/

	/* Get starting approximation = rv * 10**e1 */

	if (e1 > 0) {
		if ( (i = e1 & 15) !=0)
			dval(rv) *= tens[i];
		if (e1 &= ~15) {
			if (e1 > DBL_MAX_10_EXP) {
 ovfl:
#ifndef NO_ERRNO
				ptr->_errno = ERANGE;
#endif
				/* Can't trust HUGE_VAL */
#ifdef IEEE_Arith
#ifdef Honor_FLT_ROUNDS
				switch(rounding) {
				  case 0: /* toward 0 */
				  case 3: /* toward -infinity */
					dword0(rv) = Big0;
#ifndef _DOUBLE_IS_32BITS
					dword1(rv) = Big1;
#endif /*!_DOUBLE_IS_32BITS*/
					break;
				  default:
					dword0(rv) = Exp_mask;
#ifndef _DOUBLE_IS_32BITS
					dword1(rv) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
				  }
#else /*Honor_FLT_ROUNDS*/
				dword0(rv) = Exp_mask;
#ifndef _DOUBLE_IS_32BITS
				dword1(rv) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
#endif /*Honor_FLT_ROUNDS*/
#ifdef SET_INEXACT
				/* set overflow bit */
				dval(rv0) = 1e300;
				dval(rv0) *= dval(rv0);
#endif
#else /*IEEE_Arith*/
				dword0(rv) = Big0;
#ifndef _DOUBLE_IS_32BITS
				dword1(rv) = Big1;
#endif /*!_DOUBLE_IS_32BITS*/
#endif /*IEEE_Arith*/
				if (bd0)
					goto retfree;
				goto ret;
				}
			e1 >>= 4;
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					dval(rv) *= bigtens[j];
		/* The last multiplication could overflow. */
			dword0(rv) -= P*Exp_msk1;
			dval(rv) *= bigtens[j];
			if ((z = dword0(rv) & Exp_mask)
			 > Exp_msk1*(DBL_MAX_EXP+Bias-P))
				goto ovfl;
			if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) {
				/* set to largest number */
				/* (Can't trust DBL_MAX) */
				dword0(rv) = Big0;
#ifndef _DOUBLE_IS_32BITS
				dword1(rv) = Big1;
#endif /*!_DOUBLE_IS_32BITS*/
				}
			else
				dword0(rv) += P*Exp_msk1;
			}
		}
	else if (e1 < 0) {
		e1 = -e1;
		if ( (i = e1 & 15) !=0)
			dval(rv) /= tens[i];
		if (e1 >>= 4) {
			if (e1 >= 1 << n_bigtens)
				goto undfl;
#ifdef Avoid_Underflow
			if (e1 & Scale_Bit)
				scale = 2*P;
			for(j = 0; e1 > 0; j++, e1 >>= 1)
				if (e1 & 1)
					dval(rv) *= tinytens[j];
			if (scale && (j = 2*P + 1 - ((dword0(rv) & Exp_mask)
						>> Exp_shift)) > 0) {
				/* scaled rv is denormal; zap j low bits */
				if (j >= 32) {
#ifndef _DOUBLE_IS_32BITS
					dword1(rv) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
					if (j >= 53)
					 dword0(rv) = (P+2)*Exp_msk1;
					else
					 dword0(rv) &= 0xffffffff << (j-32);
					}
#ifndef _DOUBLE_IS_32BITS
				else
					dword1(rv) &= 0xffffffff << j;
#endif /*!_DOUBLE_IS_32BITS*/
				}
#else
			for(j = 0; e1 > 1; j++, e1 >>= 1)
				if (e1 & 1)
					dval(rv) *= tinytens[j];
			/* The last multiplication could underflow. */
			dval(rv0) = dval(rv);
			dval(rv) *= tinytens[j];
			if (!dval(rv)) {
				dval(rv) = 2.*dval(rv0);
				dval(rv) *= tinytens[j];
#endif
				if (!dval(rv)) {
 undfl:
					dval(rv) = 0.;
#ifndef NO_ERRNO
					ptr->_errno = ERANGE;
#endif
					if (bd0)
						goto retfree;
					goto ret;
					}
#ifndef Avoid_Underflow
#ifndef _DOUBLE_IS_32BITS
				dword0(rv) = Tiny0;
				dword1(rv) = Tiny1;
#else
				dword0(rv) = Tiny1;
#endif /*_DOUBLE_IS_32BITS*/
				/* The refinement below will clean
				 * this approximation up.
				 */
				}
#endif
			}
		}

	/* Now the hard part -- adjusting rv to the correct value.*/

	/* Put digits into bd: true value = bd * 10^e */

	bd0 = s2b(ptr, s0, nd0, nd, y);
	if (bd0 == NULL)
		goto ovfl;

	for(;;) {
		bd = Balloc(ptr,bd0->_k);
		if (bd == NULL)
			goto ovfl;
		Bcopy(bd, bd0);
		bb = d2b(ptr,dval(rv), &bbe, &bbbits);	/* rv = bb * 2^bbe */
		if (bb == NULL)
			goto ovfl;
		bs = i2b(ptr,1);
		if (bs == NULL)
			goto ovfl;

		if (e >= 0) {
			bb2 = bb5 = 0;
			bd2 = bd5 = e;
			}
		else {
			bb2 = bb5 = -e;
			bd2 = bd5 = 0;
			}
		if (bbe >= 0)
			bb2 += bbe;
		else
			bd2 -= bbe;
		bs2 = bb2;
#ifdef Honor_FLT_ROUNDS
		if (rounding != 1)
			bs2++;
#endif
#ifdef Avoid_Underflow
		Lsb = LSB;
		Lsb1 = 0;
		j = bbe - scale;
		i = j + bbbits - 1;	/* logb(rv) */
		j = P + 1 - bbbits;
		if (i < Emin) {	/* denormal */
			i = Emin - i;
			j -= i;
			if (i < 32)
				Lsb <<= i;
			else
				Lsb1 = Lsb << (i-32);
			}
#else /*Avoid_Underflow*/
#ifdef Sudden_Underflow
#ifdef IBM
		j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
		j = P + 1 - bbbits;
#endif
#else /*Sudden_Underflow*/
		j = bbe;
		i = j + bbbits - 1;	/* logb(rv) */
		if (i < Emin)	/* denormal */
			j += P - Emin;
		else
			j = P + 1 - bbbits;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
		bb2 += j;
		bd2 += j;
#ifdef Avoid_Underflow
		bd2 += scale;
#endif
		i = bb2 < bd2 ? bb2 : bd2;
		if (i > bs2)
			i = bs2;
		if (i > 0) {
			bb2 -= i;
			bd2 -= i;
			bs2 -= i;
			}
		if (bb5 > 0) {
			bs = pow5mult(ptr, bs, bb5);
			if (bs == NULL)
				goto ovfl;
			bb1 = mult(ptr, bs, bb);
			if (bb1 == NULL)
				goto ovfl;
			Bfree(ptr, bb);
			bb = bb1;
			}
		if (bb2 > 0) {
			bb = lshift(ptr, bb, bb2);
			if (bb == NULL)
				goto ovfl;
			}
		if (bd5 > 0) {
			bd = pow5mult(ptr, bd, bd5);
			if (bd == NULL)
				goto ovfl;
			}
		if (bd2 > 0) {
			bd = lshift(ptr, bd, bd2);
			if (bd == NULL)
				goto ovfl;
			}
		if (bs2 > 0) {
			bs = lshift(ptr, bs, bs2);
			if (bs == NULL)
				goto ovfl;
			}
		delta = diff(ptr, bb, bd);
		if (delta == NULL)
			goto ovfl;
		dsign = delta->_sign;
		delta->_sign = 0;
		i = cmp(delta, bs);
#ifdef Honor_FLT_ROUNDS
		if (rounding != 1) {
			if (i < 0) {
				/* Error is less than an ulp */
				if (!delta->_x[0] && delta->_wds <= 1) {
					/* exact */
#ifdef SET_INEXACT
					inexact = 0;
#endif
					break;
					}
				if (rounding) {
					if (dsign) {
						adj = 1.;
						goto apply_adj;
						}
					}
				else if (!dsign) {
					adj = -1.;
					if (!dword1(rv)
					    && !(dword0(rv) & Frac_mask)) {
						y = dword0(rv) & Exp_mask;
#ifdef Avoid_Underflow
						if (!scale || y > 2*P*Exp_msk1)
#else
						if (y)
#endif
						  {
						  delta = lshift(ptr, delta,Log2P);
						  if (cmp(delta, bs) <= 0)
							adj = -0.5;
						  }
						}
 apply_adj:
#ifdef Avoid_Underflow
					if (scale && (y = dword0(rv) & Exp_mask)
						<= 2*P*Exp_msk1)
					  dword0(adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
					if ((dword0(rv) & Exp_mask) <=
							P*Exp_msk1) {
						dword0(rv) += P*Exp_msk1;
						dval(rv) += adj*ulp(dval(rv));
						dword0(rv) -= P*Exp_msk1;
						}
					else
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
					dval(rv) += adj*ulp(dval(rv));
					}
				break;
				}
			adj = ratio(delta, bs);
			if (adj < 1.)
				adj = 1.;
			if (adj <= 0x7ffffffe) {
				/* adj = rounding ? ceil(adj) : floor(adj); */
				y = adj;
				if (y != adj) {
					if (!((rounding>>1) ^ dsign))
						y++;
					adj = y;
					}
				}
#ifdef Avoid_Underflow
			if (scale && (y = dword0(rv) & Exp_mask) <= 2*P*Exp_msk1)
				dword0(adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
			if ((dword0(rv) & Exp_mask) <= P*Exp_msk1) {
				dword0(rv) += P*Exp_msk1;
				adj *= ulp(dval(rv));
				if (dsign)
					dval(rv) += adj;
				else
					dval(rv) -= adj;
				dword0(rv) -= P*Exp_msk1;
				goto cont;
				}
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
			adj *= ulp(dval(rv));
			if (dsign) {
				if (dword0(rv) == Big0 && dword1(rv) == Big1)
					goto ovfl;
				dval(rv) += adj;
			else
				dval(rv) -= adj;
			goto cont;
			}
#endif /*Honor_FLT_ROUNDS*/

		if (i < 0) {
			/* Error is less than half an ulp -- check for
			 * special case of mantissa a power of two.
			 */
			if (dsign || dword1(rv) || dword0(rv) & Bndry_mask
#ifdef IEEE_Arith
#ifdef Avoid_Underflow
			 || (dword0(rv) & Exp_mask) <= (2*P+1)*Exp_msk1
#else
			 || (dword0(rv) & Exp_mask) <= Exp_msk1
#endif
#endif
				) {
#ifdef SET_INEXACT
				if (!delta->x[0] && delta->wds <= 1)
					inexact = 0;
#endif
				break;
				}
			if (!delta->_x[0] && delta->_wds <= 1) {
				/* exact result */
#ifdef SET_INEXACT
				inexact = 0;
#endif
				break;
				}
			delta = lshift(ptr,delta,Log2P);
			if (cmp(delta, bs) > 0)
				goto drop_down;
			break;
			}
		if (i == 0) {
			/* exactly half-way between */
			if (dsign) {
				if ((dword0(rv) & Bndry_mask1) == Bndry_mask1
				 &&  dword1(rv) == (
#ifdef Avoid_Underflow
			(scale && (y = dword0(rv) & Exp_mask) <= 2*P*Exp_msk1)
		? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
						   0xffffffff)) {
					/*boundary case -- increment exponent*/
					if (dword0(rv) == Big0 && dword1(rv) == Big1)
						goto ovfl;
					dword0(rv) = (dword0(rv) & Exp_mask)
						+ Exp_msk1
#ifdef IBM
						| Exp_msk1 >> 4
#endif
						;
#ifndef _DOUBLE_IS_32BITS
					dword1(rv) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
#ifdef Avoid_Underflow
					dsign = 0;
#endif
					break;
					}
				}
			else if (!(dword0(rv) & Bndry_mask) && !dword1(rv)) {
 drop_down:
				/* boundary case -- decrement exponent */
#ifdef Sudden_Underflow /*{{*/
				L = dword0(rv) & Exp_mask;
#ifdef IBM
				if (L <  Exp_msk1)
#else
#ifdef Avoid_Underflow
				if (L <= (scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
				if (L <= Exp_msk1)
#endif /*Avoid_Underflow*/
#endif /*IBM*/
					goto undfl;
				L -= Exp_msk1;
#else /*Sudden_Underflow}{*/
#ifdef Avoid_Underflow
				if (scale) {
					L = dword0(rv) & Exp_mask;
					if (L <= (2*P+1)*Exp_msk1) {
						if (L > (P+2)*Exp_msk1)
							/* round even ==> */
							/* accept rv */
							break;
						/* rv = smallest denormal */
						goto undfl;
						}
					}
#endif /*Avoid_Underflow*/
				L = (dword0(rv) & Exp_mask) - Exp_msk1;
#endif /*Sudden_Underflow}*/
				dword0(rv) = L | Bndry_mask1;
#ifndef _DOUBLE_IS_32BITS
				dword1(rv) = 0xffffffff;
#endif /*!_DOUBLE_IS_32BITS*/
#ifdef IBM
				goto cont;
#else
				break;
#endif
				}
#ifndef ROUND_BIASED
#ifdef Avoid_Underflow
			if (Lsb1) {
				if (!(dword0(rv) & Lsb1))
					break;
				}
			else if (!(dword1(rv) & Lsb))
				break;
#else
			if (!(dword1(rv) & LSB))
				break;
#endif
#endif
			if (dsign)
#ifdef Avoid_Underflow
				dval(rv) += sulp(rv, scale);
#else
				dval(rv) += ulp(dval(rv));
#endif
#ifndef ROUND_BIASED
			else {
#ifdef Avoid_Underflow
				dval(rv) -= sulp(rv, scale);
#else
				dval(rv) -= ulp(dval(rv));
#endif
#ifndef Sudden_Underflow
				if (!dval(rv))
					goto undfl;
#endif
				}
#ifdef Avoid_Underflow
			dsign = 1 - dsign;
#endif
#endif
			break;
			}
		if ((aadj = ratio(delta, bs)) <= 2.) {
			if (dsign)
				aadj = dval(aadj1) = 1.;
			else if (dword1(rv) || dword0(rv) & Bndry_mask) {
#ifndef Sudden_Underflow
				if (dword1(rv) == Tiny1 && !dword0(rv))
					goto undfl;
#endif
				aadj = 1.;
				dval(aadj1) = -1.;
				}
			else {
				/* special case -- power of FLT_RADIX to be */
				/* rounded down... */

				if (aadj < 2./FLT_RADIX)
					aadj = 1./FLT_RADIX;
				else
					aadj *= 0.5;
				dval(aadj1) = -aadj;
				}
			}
		else {
			aadj *= 0.5;
			dval(aadj1) = dsign ? aadj : -aadj;
#ifdef Check_FLT_ROUNDS
			switch(Rounding) {
				case 2: /* towards +infinity */
					dval(aadj1) -= 0.5;
					break;
				case 0: /* towards 0 */
				case 3: /* towards -infinity */
					dval(aadj1) += 0.5;
				}
#else
			if (Flt_Rounds == 0)
				dval(aadj1) += 0.5;
#endif /*Check_FLT_ROUNDS*/
			}
		y = dword0(rv) & Exp_mask;

		/* Check for overflow */

		if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1)) {
			dval(rv0) = dval(rv);
			dword0(rv) -= P*Exp_msk1;
			adj = dval(aadj1) * ulp(dval(rv));
			dval(rv) += adj;
			if ((dword0(rv) & Exp_mask) >=
					Exp_msk1*(DBL_MAX_EXP+Bias-P)) {
				if (dword0(rv0) == Big0 && dword1(rv0) == Big1)
					goto ovfl;
				dword0(rv) = Big0;
#ifndef _DOUBLE_IS_32BITS
				dword1(rv) = Big1;
#endif /*!_DOUBLE_IS_32BITS*/
				goto cont;
				}
			else
				dword0(rv) += P*Exp_msk1;
			}
		else {
#ifdef Avoid_Underflow
			if (scale && y <= 2*P*Exp_msk1) {
				if (aadj <= 0x7fffffff) {
					if ((z = aadj) == 0)
						z = 1;
					aadj = z;
					dval(aadj1) = dsign ? aadj : -aadj;
					}
				dword0(aadj1) += (2*P+1)*Exp_msk1 - y;
				}
			adj = dval(aadj1) * ulp(dval(rv));
			dval(rv) += adj;
#else
#ifdef Sudden_Underflow
			if ((dword0(rv) & Exp_mask) <= P*Exp_msk1) {
				dval(rv0) = dval(rv);
				dword0(rv) += P*Exp_msk1;
				adj = dval(aadj1) * ulp(dval(rv));
				dval(rv) += adj;
#ifdef IBM
				if ((dword0(rv) & Exp_mask) <  P*Exp_msk1)
#else
				if ((dword0(rv) & Exp_mask) <= P*Exp_msk1)
#endif
					{
					if (dword0(rv0) == Tiny0
					 && dword1(rv0) == Tiny1)
						goto undfl;
#ifndef _DOUBLE_IS_32BITS
					dword0(rv) = Tiny0;
					dword1(rv) = Tiny1;
#else
					dword0(rv) = Tiny1;
#endif /*_DOUBLE_IS_32BITS*/
					goto cont;
					}
				else
					dword0(rv) -= P*Exp_msk1;
				}
			else {
				adj = dval(aadj1) * ulp(dval(rv));
				dval(rv) += adj;
				}
#else /*Sudden_Underflow*/
			/* Compute adj so that the IEEE rounding rules will
			 * correctly round rv + adj in some half-way cases.
			 * If rv * ulp(rv) is denormalized (i.e.,
			 * y <= (P-1)*Exp_msk1), we must adjust aadj to avoid
			 * trouble from bits lost to denormalization;
			 * example: 1.2e-307 .
			 */
			if (y <= (P-1)*Exp_msk1 && aadj > 1.) {
				dval(aadj1) = (double)(int)(aadj + 0.5);
				if (!dsign)
					dval(aadj1) = -dval(aadj1);
				}
			adj = dval(aadj1) * ulp(dval(rv));
			dval(rv) += adj;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
			}
		z = dword0(rv) & Exp_mask;
#ifndef SET_INEXACT
#ifdef Avoid_Underflow
		if (!scale)
#endif
		if (y == z) {
			/* Can we stop now? */
			L = (Long)aadj;
			aadj -= L;
			/* The tolerances below are conservative. */
			if (dsign || dword1(rv) || dword0(rv) & Bndry_mask) {
				if (aadj < .4999999 || aadj > .5000001)
					break;
				}
			else if (aadj < .4999999/FLT_RADIX)
				break;
			}
#endif
 cont:
		Bfree(ptr,bb);
		Bfree(ptr,bd);
		Bfree(ptr,bs);
		Bfree(ptr,delta);
		}
#ifdef SET_INEXACT
	if (inexact) {
		if (!oldinexact) {
			dword0(rv0) = Exp_1 + (70 << Exp_shift);
#ifndef _DOUBLE_IS_32BITS
			dword1(rv0) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
			dval(rv0) += 1.;
			}
		}
	else if (!oldinexact)
		clear_inexact();
#endif
#ifdef Avoid_Underflow
	if (scale) {
		dword0(rv0) = Exp_1 - 2*P*Exp_msk1;
#ifndef _DOUBLE_IS_32BITS
		dword1(rv0) = 0;
#endif /*!_DOUBLE_IS_32BITS*/
		dval(rv) *= dval(rv0);
#ifndef NO_ERRNO
		/* try to avoid the bug of testing an 8087 register value */
		if (dword0(rv) == 0 && dword1(rv) == 0)
			ptr->_errno = ERANGE;
#endif
		}
#endif /* Avoid_Underflow */
#ifdef SET_INEXACT
	if (inexact && !(dword0(rv) & Exp_mask)) {
		/* set underflow bit */
		dval(rv0) = 1e-300;
		dval(rv0) *= dval(rv0);
		}
#endif
 retfree:
	Bfree(ptr,bb);
	Bfree(ptr,bd);
	Bfree(ptr,bs);
	Bfree(ptr,bd0);
	Bfree(ptr,delta);
 ret:
	if (se)
		*se = (char *)s;
	return sign ? -dval(rv) : dval(rv);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_ 
double
strtod(_CONST char *__restrict s00 , char **__restrict se)
{
  return _strtod_r (_REENT, s00, se);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_ 
float
strtof(_CONST char *__restrict s00 ,
	char **__restrict se)
{
  double retval = _strtod_r (_REENT, s00, se);
  if (isnan (retval)) {
    return rtl_nanf ();
  }
  return (float)retval;
}
