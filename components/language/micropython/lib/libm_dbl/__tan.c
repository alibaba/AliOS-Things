/* origin: FreeBSD /usr/src/lib/msun/src/k_tan.c */
/*
 * ====================================================
 * Copyright 2004 Sun Microsystems, Inc.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/* __tan( x, y, k )
 * kernel tan function on ~[-pi/4, pi/4] (except on -0), pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input odd indicates whether tan (if odd = 0) or -1/tan (if odd = 1) is returned.
 *
 * Algorithm
 *      1. Since tan(-x) = -tan(x), we need only to consider positive x.
 *      2. Callers must return tan(-0) = -0 without calling here since our
 *         odd polynomial is not evaluated in a way that preserves -0.
 *         Callers may do the optimization tan(x) ~ x for tiny x.
 *      3. tan(x) is approximated by a odd polynomial of degree 27 on
 *         [0,0.67434]
 *                               3             27
 *              tan(x) ~ x + T1*x + ... + T13*x
 *         where
 *
 *              |tan(x)         2     4            26   |     -59.2
 *              |----- - (1+T1*x +T2*x +.... +T13*x    )| <= 2
 *              |  x                                    |
 *
 *         Note: tan(x+y) = tan(x) + tan'(x)*y
 *                        ~ tan(x) + (1+x*x)*y
 *         Therefore, for better accuracy in computing tan(x+y), let
 *                   3      2      2       2       2
 *              r = x *(T2+x *(T3+x *(...+x *(T12+x *T13))))
 *         then
 *                                  3    2
 *              tan(x+y) = x + (T1*x + (x *(r+y)+y))
 *
 *      4. For x in [0.67434,pi/4],  let y = pi/4 - x, then
 *              tan(x) = tan(pi/4-y) = (1-tan(y))/(1+tan(y))
 *                     = 1 - 2*(tan(y) - (tan(y)^2)/(1+tan(y)))
 */

#include "libm.h"

static const double T[] = {
             3.33333333333334091986e-01, /* 3FD55555, 55555563 */
             1.33333333333201242699e-01, /* 3FC11111, 1110FE7A */
             5.39682539762260521377e-02, /* 3FABA1BA, 1BB341FE */
             2.18694882948595424599e-02, /* 3F9664F4, 8406D637 */
             8.86323982359930005737e-03, /* 3F8226E3, E96E8493 */
             3.59207910759131235356e-03, /* 3F6D6D22, C9560328 */
             1.45620945432529025516e-03, /* 3F57DBC8, FEE08315 */
             5.88041240820264096874e-04, /* 3F4344D8, F2F26501 */
             2.46463134818469906812e-04, /* 3F3026F7, 1A8D1068 */
             7.81794442939557092300e-05, /* 3F147E88, A03792A6 */
             7.14072491382608190305e-05, /* 3F12B80F, 32F0A7E9 */
            -1.85586374855275456654e-05, /* BEF375CB, DB605373 */
             2.59073051863633712884e-05, /* 3EFB2A70, 74BF7AD4 */
},
pio4 =       7.85398163397448278999e-01, /* 3FE921FB, 54442D18 */
pio4lo =     3.06161699786838301793e-17; /* 3C81A626, 33145C07 */

double __tan(double x, double y, int odd)
{
	double_t z, r, v, w, s, a;
	double w0, a0;
	uint32_t hx;
	int big, sign;

	GET_HIGH_WORD(hx,x);
	big = (hx&0x7fffffff) >= 0x3FE59428; /* |x| >= 0.6744 */
	if (big) {
		sign = hx>>31;
		if (sign) {
			x = -x;
			y = -y;
		}
		x = (pio4 - x) + (pio4lo - y);
		y = 0.0;
	}
	z = x * x;
	w = z * z;
	/*
	 * Break x^5*(T[1]+x^2*T[2]+...) into
	 * x^5(T[1]+x^4*T[3]+...+x^20*T[11]) +
	 * x^5(x^2*(T[2]+x^4*T[4]+...+x^22*[T12]))
	 */
	r = T[1] + w*(T[3] + w*(T[5] + w*(T[7] + w*(T[9] + w*T[11]))));
	v = z*(T[2] + w*(T[4] + w*(T[6] + w*(T[8] + w*(T[10] + w*T[12])))));
	s = z * x;
	r = y + z*(s*(r + v) + y) + s*T[0];
	w = x + r;
	if (big) {
		s = 1 - 2*odd;
		v = s - 2.0 * (x + (r - w*w/(w + s)));
		return sign ? -v : v;
	}
	if (!odd)
		return w;
	/* -1.0/(x+r) has up to 2ulp error, so compute it accurately */
	w0 = w;
	SET_LOW_WORD(w0, 0);
	v = r - (w0 - x);       /* w0+v = r+x */
	a0 = a = -1.0 / w;
	SET_LOW_WORD(a0, 0);
	return a0 + a*(1.0 + a0*w0 + a0*v);
}
