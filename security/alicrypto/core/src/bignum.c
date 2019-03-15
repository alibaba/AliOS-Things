/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "config.h"
#include "osa.h"


#if defined(ALI_ALGO_BIGNUM_C)

#include "bignum.h"
#include "bn_mul.h"


/* Implementation that should never be optimized out by the compiler */
static void ali_algo_mpi_zeroize(ali_algo_mpi_uint *v, size_t n)
{
    volatile ali_algo_mpi_uint *p = v;
    while (n--) {
        *p++ = 0;
    }
}

#define ciL (sizeof(ali_algo_mpi_uint)) /* chars in limb  */
#define biL (ciL << 3)                 /* bits  in limb  */
#define biH (ciL << 2)                 /* half limb size */

#define MPI_SIZE_T_MAX ((size_t)-1) /* SIZE_T_MAX is not standard */

/*
 * Convert between bits/chars and number of limbs
 * Divide first in order to avoid potential overflows
 */
#define BITS_TO_LIMBS(i) ((i) / biL + ((i) % biL != 0))
#define CHARS_TO_LIMBS(i) ((i) / ciL + ((i) % ciL != 0))

/*
 * Initialize one MPI
 */
void ali_algo_mpi_init(ali_algo_mpi *X)
{
    if (X == NULL) {
        return;
    }

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

/*
 * Unallocate one MPI
 */
void ali_algo_mpi_free(ali_algo_mpi *X)
{
    if (X == NULL) {
        return;
    }

    if (X->p != NULL) {
        ali_algo_mpi_zeroize(X->p, X->n);
        osa_free(X->p);
    }

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

/*
 * Enlarge to the specified number of limbs
 */
int ali_algo_mpi_grow(ali_algo_mpi *X, size_t nblimbs)
{
    ali_algo_mpi_uint *p;

    if (nblimbs > ALI_ALGO_MPI_MAX_LIMBS) {
        return (ALI_ALGO_ERR_MPI_ALLOC_FAILED);
    }

    if (X->n < nblimbs) {
        if ((p = (ali_algo_mpi_uint *)osa_calloc(nblimbs, ciL)) == NULL) {
            return (ALI_ALGO_ERR_MPI_ALLOC_FAILED);
        }

        if (X->p != NULL) {
            osa_memcpy(p, X->p, X->n * ciL);
            ali_algo_mpi_zeroize(X->p, X->n);
            osa_free(X->p);
        }

        X->n = nblimbs;
        X->p = p;
    }

    return (0);
}

/*
 * Resize down as much as possible,
 * while keeping at least the specified number of limbs
 */
int ali_algo_mpi_shrink(ali_algo_mpi *X, size_t nblimbs)
{
    ali_algo_mpi_uint *p;
    size_t            i;

    /* Actually resize up in this case */
    if (X->n <= nblimbs) {
        return (ali_algo_mpi_grow(X, nblimbs));
    }

    for (i = X->n - 1; i > 0; i--)
        if (X->p[i] != 0) {
            break;
        }
    i++;

    if (i < nblimbs) {
        i = nblimbs;
    }

    if ((p = (ali_algo_mpi_uint *)osa_calloc(i, ciL)) == NULL) {
        return (ALI_ALGO_ERR_MPI_ALLOC_FAILED);
    }

    if (X->p != NULL) {
        osa_memcpy(p, X->p, i * ciL);
        ali_algo_mpi_zeroize(X->p, X->n);
        osa_free(X->p);
    }

    X->n = i;
    X->p = p;

    return (0);
}

/*
 * Copy the contents of Y into X
 */
int ali_algo_mpi_copy(ali_algo_mpi *X, const ali_algo_mpi *Y)
{
    int    ret;
    size_t i;

    if (X == Y) {
        return (0);
    }

    if (Y->p == NULL) {
        ali_algo_mpi_free(X);
        return (0);
    }

    for (i = Y->n - 1; i > 0; i--)
        if (Y->p[i] != 0) {
            break;
        }
    i++;

    X->s = Y->s;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, i));

    osa_memset(X->p, 0, X->n * ciL);
    osa_memcpy(X->p, Y->p, i * ciL);

cleanup:

    return (ret);
}

/*
 * Swap the contents of X and Y
 */
void ali_algo_mpi_swap(ali_algo_mpi *X, ali_algo_mpi *Y)
{
    ali_algo_mpi T;

    osa_memcpy(&T, X, sizeof(ali_algo_mpi));
    osa_memcpy(X, Y, sizeof(ali_algo_mpi));
    osa_memcpy(Y, &T, sizeof(ali_algo_mpi));
}

/*
 * Conditionally assign X = Y, without leaking information
 * about whether the assignment was made or not.
 * (Leaking information about the respective sizes of X and Y is ok however.)
 */
int ali_algo_mpi_safe_cond_assign(ali_algo_mpi *X, const ali_algo_mpi *Y,
                                 unsigned char assign)
{
    int    ret = 0;
    size_t i;

    /* make sure assign is 0 or 1 in a time-constant manner */
    assign = (assign | (unsigned char)-assign) >> 7;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, Y->n));

    X->s = X->s * (1 - assign) + Y->s * assign;

    for (i = 0; i < Y->n; i++) {
        X->p[i] = X->p[i] * (1 - assign) + Y->p[i] * assign;
    }

    for (; i < X->n; i++) {
        X->p[i] *= (1 - assign);
    }

cleanup:
    return (ret);
}

/*
 * Conditionally swap X and Y, without leaking information
 * about whether the swap was made or not.
 * Here it is not ok to simply swap the pointers, which whould lead to
 * different memory access patterns when X and Y are used afterwards.
 */
int ali_algo_mpi_safe_cond_swap(ali_algo_mpi *X, ali_algo_mpi *Y,
                               unsigned char swap)
{
    int              ret, s;
    size_t           i;
    ali_algo_mpi_uint tmp;

    if (X == Y) {
        return (0);
    }

    /* make sure swap is 0 or 1 in a time-constant manner */
    swap = (swap | (unsigned char)-swap) >> 7;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, Y->n));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(Y, X->n));

    s    = X->s;
    X->s = X->s * (1 - swap) + Y->s * swap;
    Y->s = Y->s * (1 - swap) + s * swap;


    for (i = 0; i < X->n; i++) {
        tmp     = X->p[i];
        X->p[i] = X->p[i] * (1 - swap) + Y->p[i] * swap;
        Y->p[i] = Y->p[i] * (1 - swap) + tmp * swap;
    }

cleanup:
    return (ret);
}

/*
 * Set value from integer
 */
int ali_algo_mpi_lset(ali_algo_mpi *X, ali_algo_mpi_sint z)
{
    int ret;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, 1));
    osa_memset(X->p, 0, X->n * ciL);

    X->p[0] = (z < 0) ? -z : z;
    X->s    = (z < 0) ? -1 : 1;

cleanup:

    return (ret);
}

/*
 * Get a specific bit
 */
int ali_algo_mpi_get_bit(const ali_algo_mpi *X, size_t pos)
{
    if (X->n * biL <= pos) {
        return (0);
    }

    return ((X->p[pos / biL] >> (pos % biL)) & 0x01);
}

/*
 * Set a bit to a specific value of 0 or 1
 */
int ali_algo_mpi_set_bit(ali_algo_mpi *X, size_t pos, unsigned char val)
{
    int    ret = 0;
    size_t off = pos / biL;
    size_t idx = pos % biL;

    if (val != 0 && val != 1) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    if (X->n * biL <= pos) {
        if (val == 0) {
            return (0);
        }

        ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, off + 1));
    }

    X->p[off] &= ~((ali_algo_mpi_uint)0x01 << idx);
    X->p[off] |= (ali_algo_mpi_uint)val << idx;

cleanup:

    return (ret);
}

/*
 * Return the number of less significant zero-bits
 */
size_t ali_algo_mpi_lsb(const ali_algo_mpi *X)
{
    size_t i, j, count = 0;

    for (i = 0; i < X->n; i++)
        for (j = 0; j < biL; j++, count++)
            if (((X->p[i] >> j) & 1) != 0) {
                return (count);
            }

    return (0);
}

/*
 * Count leading zero bits in a given integer
 */
static size_t ali_algo_clz(const ali_algo_mpi_uint x)
{
    size_t           j;
    ali_algo_mpi_uint mask = (ali_algo_mpi_uint)1 << (biL - 1);

    for (j = 0; j < biL; j++) {
        if (x & mask) {
            break;
        }

        mask >>= 1;
    }

    return j;
}

/*
 * Return the number of bits
 */
size_t ali_algo_mpi_bitlen(const ali_algo_mpi *X)
{
    size_t i, j;

    if (X->n == 0) {
        return (0);
    }

    for (i = X->n - 1; i > 0; i--)
        if (X->p[i] != 0) {
            break;
        }

    j = biL - ali_algo_clz(X->p[i]);

    return ((i * biL) + j);
}

/*
 * Return the total size in bytes
 */
size_t ali_algo_mpi_size(const ali_algo_mpi *X)
{
    return ((ali_algo_mpi_bitlen(X) + 7) >> 3);
}

/*
 * Convert an ASCII character to digit value
 */
static int mpi_get_digit(ali_algo_mpi_uint *d, int radix, char c)
{
    *d = 255;

    if (c >= 0x30 && c <= 0x39) {
        *d = c - 0x30;
    }
    if (c >= 0x41 && c <= 0x46) {
        *d = c - 0x37;
    }
    if (c >= 0x61 && c <= 0x66) {
        *d = c - 0x57;
    }

    if (*d >= (ali_algo_mpi_uint)radix) {
        return (ALI_ALGO_ERR_MPI_INVALID_CHARACTER);
    }

    return (0);
}

/*
 * Import from an ASCII string
 */
int ali_algo_mpi_read_string(ali_algo_mpi *X, int radix, const char *s)
{
    int              ret;
    size_t           i, j, slen, n;
    ali_algo_mpi_uint d;
    ali_algo_mpi      T;

    if (radix < 2 || radix > 16) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    ali_algo_mpi_init(&T);

    slen = osa_strlen(s);

    if (radix == 16) {
        if (slen > MPI_SIZE_T_MAX >> 2) {
            return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
        }

        n = BITS_TO_LIMBS(slen << 2);

        ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, n));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(X, 0));

        for (i = slen, j = 0; i > 0; i--, j++) {
            if (i == 1 && s[i - 1] == '-') {
                X->s = -1;
                break;
            }

            ALI_ALGO_MPI_CHK(mpi_get_digit(&d, radix, s[i - 1]));
            X->p[j / (2 * ciL)] |= d << ((j % (2 * ciL)) << 2);
        }
    } else {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(X, 0));

        for (i = 0; i < slen; i++) {
            if (i == 0 && s[i] == '-') {
                X->s = -1;
                continue;
            }

            ALI_ALGO_MPI_CHK(mpi_get_digit(&d, radix, s[i]));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_int(&T, X, radix));

            if (X->s == 1) {
                ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(X, &T, d));
            } else {
                ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(X, &T, d));
            }
        }
    }

cleanup:

    ali_algo_mpi_free(&T);

    return (ret);
}

/*
 * Helper to write the digits high-order first
 */
static int mpi_write_hlp(ali_algo_mpi *X, int radix, char **p)
{
    int              ret;
    ali_algo_mpi_uint r;

    if (radix < 2 || radix > 16) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_int(&r, X, radix));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_div_int(X, NULL, X, radix));

    if (ali_algo_mpi_cmp_int(X, 0) != 0) {
        ALI_ALGO_MPI_CHK(mpi_write_hlp(X, radix, p));
    }

    if (r < 10) {
        *(*p)++ = (char)(r + 0x30);
    } else {
        *(*p)++ = (char)(r + 0x37);
    }

cleanup:

    return (ret);
}

/*
 * Export into an ASCII string
 */
int ali_algo_mpi_write_string(const ali_algo_mpi *X, int radix, char *buf,
                             size_t buflen, size_t *olen)
{
    int         ret = 0;
    size_t      n;
    char *      p;
    ali_algo_mpi T;

    if (radix < 2 || radix > 16) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    n = ali_algo_mpi_bitlen(X);
    if (radix >= 4) {
        n >>= 1;
    }
    if (radix >= 16) {
        n >>= 1;
    }
    /*
     * Round up the buffer length to an even value to ensure that there is
     * enough room for hexadecimal values that can be represented in an odd
     * number of digits.
     */
    n += 3 + ((n + 1) & 1);

    if (buflen < n) {
        *olen = n;
        return (ALI_ALGO_ERR_MPI_BUFFER_TOO_SMALL);
    }

    p = buf;
    ali_algo_mpi_init(&T);

    if (X->s == -1) {
        *p++ = '-';
    }

    if (radix == 16) {
        int    c;
        size_t i, j, k;

        for (i = X->n, k = 0; i > 0; i--) {
            for (j = ciL; j > 0; j--) {
                c = (X->p[i - 1] >> ((j - 1) << 3)) & 0xFF;

                if (c == 0 && k == 0 && (i + j) != 2) {
                    continue;
                }

                *(p++) = "0123456789ABCDEF"[c / 16];
                *(p++) = "0123456789ABCDEF"[c % 16];
                k      = 1;
            }
        }
    } else {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&T, X));

        if (T.s == -1) {
            T.s = 1;
        }

        ALI_ALGO_MPI_CHK(mpi_write_hlp(&T, radix, &p));
    }

    *p++  = '\0';
    *olen = p - buf;

cleanup:

    ali_algo_mpi_free(&T);

    return (ret);
}


/*
 * Import X from unsigned binary data, big endian
 */
int ali_algo_mpi_read_binary(ali_algo_mpi *X, const unsigned char *buf,
                            size_t buflen)
{
    int    ret;
    size_t i, j, n;

    for (n = 0; n < buflen; n++)
        if (buf[n] != 0) {
            break;
        }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, CHARS_TO_LIMBS(buflen - n)));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(X, 0));

    for (i = buflen, j = 0; i > n; i--, j++) {
        X->p[j / ciL] |= ((ali_algo_mpi_uint)buf[i - 1]) << ((j % ciL) << 3);
    }

cleanup:

    return (ret);
}

/*
 * Export X into unsigned binary data, big endian
 */
int ali_algo_mpi_write_binary(const ali_algo_mpi *X, unsigned char *buf,
                             size_t buflen)
{
    size_t i, j, n;

    n = ali_algo_mpi_size(X);

    if (buflen < n) {
        return (ALI_ALGO_ERR_MPI_BUFFER_TOO_SMALL);
    }

    osa_memset(buf, 0, buflen);

    for (i = buflen - 1, j = 0; n > 0; i--, j++, n--) {
        buf[i] = (unsigned char)(X->p[j / ciL] >> ((j % ciL) << 3));
    }

    return (0);
}

/*
 * Left-shift: X <<= count
 */
int ali_algo_mpi_shift_l(ali_algo_mpi *X, size_t count)
{
    int              ret;
    size_t           i, v0, t1;
    ali_algo_mpi_uint r0 = 0, r1;

    v0 = count / (biL);
    t1 = count & (biL - 1);

    i = ali_algo_mpi_bitlen(X) + count;

    if (X->n * biL < i) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, BITS_TO_LIMBS(i)));
    }

    ret = 0;

    /*
     * shift by count / limb_size
     */
    if (v0 > 0) {
        for (i = X->n; i > v0; i--) {
            X->p[i - 1] = X->p[i - v0 - 1];
        }

        for (; i > 0; i--) {
            X->p[i - 1] = 0;
        }
    }

    /*
     * shift by count % limb_size
     */
    if (t1 > 0) {
        for (i = v0; i < X->n; i++) {
            r1 = X->p[i] >> (biL - t1);
            X->p[i] <<= t1;
            X->p[i] |= r0;
            r0 = r1;
        }
    }

cleanup:

    return (ret);
}

/*
 * Right-shift: X >>= count
 */
int ali_algo_mpi_shift_r(ali_algo_mpi *X, size_t count)
{
    size_t           i, v0, v1;
    ali_algo_mpi_uint r0 = 0, r1;

    v0 = count / biL;
    v1 = count & (biL - 1);

    if (v0 > X->n || (v0 == X->n && v1 > 0)) {
        return ali_algo_mpi_lset(X, 0);
    }

    /*
     * shift by count / limb_size
     */
    if (v0 > 0) {
        for (i = 0; i < X->n - v0; i++) {
            X->p[i] = X->p[i + v0];
        }

        for (; i < X->n; i++) {
            X->p[i] = 0;
        }
    }

    /*
     * shift by count % limb_size
     */
    if (v1 > 0) {
        for (i = X->n; i > 0; i--) {
            r1 = X->p[i - 1] << (biL - v1);
            X->p[i - 1] >>= v1;
            X->p[i - 1] |= r0;
            r0 = r1;
        }
    }

    return (0);
}

/*
 * Compare unsigned values
 */
int ali_algo_mpi_cmp_abs(const ali_algo_mpi *X, const ali_algo_mpi *Y)
{
    size_t i, j;

    for (i = X->n; i > 0; i--)
        if (X->p[i - 1] != 0) {
            break;
        }

    for (j = Y->n; j > 0; j--)
        if (Y->p[j - 1] != 0) {
            break;
        }

    if (i == 0 && j == 0) {
        return (0);
    }

    if (i > j) {
        return (1);
    }
    if (j > i) {
        return (-1);
    }

    for (; i > 0; i--) {
        if (X->p[i - 1] > Y->p[i - 1]) {
            return (1);
        }
        if (X->p[i - 1] < Y->p[i - 1]) {
            return (-1);
        }
    }

    return (0);
}

/*
 * Compare signed values
 */
int ali_algo_mpi_cmp_mpi(const ali_algo_mpi *X, const ali_algo_mpi *Y)
{
    size_t i, j;

    for (i = X->n; i > 0; i--)
        if (X->p[i - 1] != 0) {
            break;
        }

    for (j = Y->n; j > 0; j--)
        if (Y->p[j - 1] != 0) {
            break;
        }

    if (i == 0 && j == 0) {
        return (0);
    }

    if (i > j) {
        return (X->s);
    }
    if (j > i) {
        return (-Y->s);
    }

    if (X->s > 0 && Y->s < 0) {
        return (1);
    }
    if (Y->s > 0 && X->s < 0) {
        return (-1);
    }

    for (; i > 0; i--) {
        if (X->p[i - 1] > Y->p[i - 1]) {
            return (X->s);
        }
        if (X->p[i - 1] < Y->p[i - 1]) {
            return (-X->s);
        }
    }

    return (0);
}

/*
 * Compare signed values
 */
int ali_algo_mpi_cmp_int(const ali_algo_mpi *X, ali_algo_mpi_sint z)
{
    ali_algo_mpi      Y;
    ali_algo_mpi_uint p[1];

    *p  = (z < 0) ? -z : z;
    Y.s = (z < 0) ? -1 : 1;
    Y.n = 1;
    Y.p = p;

    return (ali_algo_mpi_cmp_mpi(X, &Y));
}

/*
 * Unsigned addition: X = |A| + |B|  (HAC 14.7)
 */
int ali_algo_mpi_add_abs(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int               ret;
    size_t            i, j;
    ali_algo_mpi_uint *o, *p, c, tmp;

    if (X == B) {
        const ali_algo_mpi *T = A;
        A                    = X;
        B                    = T;
    }

    if (X != A) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(X, A));
    }

    /*
     * X should always be positive as a result of unsigned additions.
     */
    X->s = 1;

    for (j = B->n; j > 0; j--)
        if (B->p[j - 1] != 0) {
            break;
        }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, j));

    o = B->p;
    p = X->p;
    c = 0;

    /*
     * tmp is used because it might happen that p == o
     */
    for (i = 0; i < j; i++, o++, p++) {
        tmp = *o;
        *p += c;
        c = (*p < c);
        *p += tmp;
        c += (*p < tmp);
    }

    while (c != 0) {
        if (i >= X->n) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, i + 1));
            p = X->p + i;
        }

        *p += c;
        c = (*p < c);
        i++;
        p++;
    }

cleanup:

    return (ret);
}

/*
 * Helper for ali_algo_mpi subtraction
 */
static void mpi_sub_hlp(size_t n, ali_algo_mpi_uint *s, ali_algo_mpi_uint *d)
{
    size_t           i;
    ali_algo_mpi_uint c, z;

    for (i = c = 0; i < n; i++, s++, d++) {
        z = (*d < c);
        *d -= c;
        c = (*d < *s) + z;
        *d -= *s;
    }

    while (c != 0) {
        z = (*d < c);
        *d -= c;
        c = z;
        i++;
        d++;
    }
}

/*
 * Unsigned subtraction: X = |A| - |B|  (HAC 14.9)
 */
int ali_algo_mpi_sub_abs(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    ali_algo_mpi TB;
    int         ret;
    size_t      n;

    if (ali_algo_mpi_cmp_abs(A, B) < 0) {
        return (ALI_ALGO_ERR_MPI_NEGATIVE_VALUE);
    }

    ali_algo_mpi_init(&TB);

    if (X == B) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TB, B));
        B = &TB;
    }

    if (X != A) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(X, A));
    }

    /*
     * X should always be positive as a result of unsigned subtractions.
     */
    X->s = 1;

    ret = 0;

    for (n = B->n; n > 0; n--)
        if (B->p[n - 1] != 0) {
            break;
        }

    mpi_sub_hlp(n, B->p, X->p);

cleanup:

    ali_algo_mpi_free(&TB);

    return (ret);
}

/*
 * Signed addition: X = A + B
 */
int ali_algo_mpi_add_mpi(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int ret, s = A->s;

    if (A->s * B->s < 0) {
        if (ali_algo_mpi_cmp_abs(A, B) >= 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(X, A, B));
            X->s = s;
        } else {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(X, B, A));
            X->s = -s;
        }
    } else {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_add_abs(X, A, B));
        X->s = s;
    }

cleanup:

    return (ret);
}

/*
 * Signed subtraction: X = A - B
 */
int ali_algo_mpi_sub_mpi(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int ret, s = A->s;

    if (A->s * B->s > 0) {
        if (ali_algo_mpi_cmp_abs(A, B) >= 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(X, A, B));
            X->s = s;
        } else {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(X, B, A));
            X->s = -s;
        }
    } else {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_add_abs(X, A, B));
        X->s = s;
    }

cleanup:

    return (ret);
}

/*
 * Signed addition: X = A + b
 */
int ali_algo_mpi_add_int(ali_algo_mpi *X, const ali_algo_mpi *A,
                        ali_algo_mpi_sint b)
{
    ali_algo_mpi      _B;
    ali_algo_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (ali_algo_mpi_add_mpi(X, A, &_B));
}

/*
 * Signed subtraction: X = A - b
 */
int ali_algo_mpi_sub_int(ali_algo_mpi *X, const ali_algo_mpi *A,
                        ali_algo_mpi_sint b)
{
    ali_algo_mpi      _B;
    ali_algo_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (ali_algo_mpi_sub_mpi(X, A, &_B));
}

/*
 * Helper for ali_algo_mpi multiplication
 */
static void mpi_mul_hlp(size_t i, ali_algo_mpi_uint *s, ali_algo_mpi_uint *d,
              ali_algo_mpi_uint b)
{
    ali_algo_mpi_uint c = 0, t = 0;

#if defined(MULADDC_HUIT)
    for (; i >= 8; i -= 8) {
        MULADDC_INIT
        MULADDC_HUIT
        MULADDC_STOP
    }

    for (; i > 0; i--) {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#else  /* MULADDC_HUIT */
    for (; i >= 16; i -= 16) {
        MULADDC_INIT
        MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE
          MULADDC_CORE MULADDC_CORE MULADDC_CORE

            MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE
              MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_STOP
    }

    for (; i >= 8; i -= 8) {
        MULADDC_INIT
        MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE

          MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_STOP
    }

    for (; i > 0; i--) {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }
#endif /* MULADDC_HUIT */

    t++;

    do {
        *d += c;
        c = (*d < c);
        d++;
    } while (c != 0);
}

/*
 * Baseline multiplication: X = A * B  (HAC 14.12)
 */
int ali_algo_mpi_mul_mpi(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int         ret;
    size_t      i, j;
    ali_algo_mpi TA, TB;

    ali_algo_mpi_init(&TA);
    ali_algo_mpi_init(&TB);

    if (X == A) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TA, A));
        A = &TA;
    }
    if (X == B) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TB, B));
        B = &TB;
    }

    for (i = A->n; i > 0; i--)
        if (A->p[i - 1] != 0) {
            break;
        }

    for (j = B->n; j > 0; j--)
        if (B->p[j - 1] != 0) {
            break;
        }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, i + j));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(X, 0));

    for (i++; j > 0; j--) {
        mpi_mul_hlp(i - 1, A->p, X->p + j - 1, B->p[j - 1]);
    }

    X->s = A->s * B->s;

cleanup:

    ali_algo_mpi_free(&TB);
    ali_algo_mpi_free(&TA);

    return (ret);
}

/*
 * Baseline multiplication: X = A * b
 */
int ali_algo_mpi_mul_int(ali_algo_mpi *X, const ali_algo_mpi *A,
                        ali_algo_mpi_uint b)
{
    ali_algo_mpi      _B;
    ali_algo_mpi_uint p[1];

    _B.s = 1;
    _B.n = 1;
    _B.p = p;
    p[0] = b;

    return (ali_algo_mpi_mul_mpi(X, A, &_B));
}

/*
 * Unsigned integer divide - double ali_algo_mpi_uint dividend, u1/u0, and
 * ali_algo_mpi_uint divisor, d
 */
static ali_algo_mpi_uint ali_algo_int_div_int(ali_algo_mpi_uint  u1,
                                            ali_algo_mpi_uint  u0,
                                            ali_algo_mpi_uint  d,
                                            ali_algo_mpi_uint *r)
{
#if defined(ALI_ALGO_HAVE_UDBL)
    ali_algo_t_udbl dividend, quotient;
#else
    const ali_algo_mpi_uint radix = (ali_algo_mpi_uint)1 << biH;
    const ali_algo_mpi_uint uint_halfword_mask =
      ((ali_algo_mpi_uint)1 << biH) - 1;
    ali_algo_mpi_uint d0, d1, q0, q1, rAX, r0, quotient;
    ali_algo_mpi_uint u0_msw, u0_lsw;
    size_t           s;
#endif

    /*
     * Check for overflow
     */
    if (0 == d || u1 >= d) {
        if (r != NULL) {
            *r = ~0;
        }

        return (~0);
    }

#if defined(ALI_ALGO_HAVE_UDBL)
    dividend = (ali_algo_t_udbl)u1 << biL;
    dividend |= (ali_algo_t_udbl)u0;
    quotient = dividend / d;
    if (quotient > ((ali_algo_t_udbl)1 << biL) - 1) {
        quotient = ((ali_algo_t_udbl)1 << biL) - 1;
    }

    if (r != NULL) {
        *r = (ali_algo_mpi_uint)(dividend - (quotient * d));
    }

    return (ali_algo_mpi_uint)quotient;
#else

    /*
     * Algorithm D, Section 4.3.1 - The Art of Computer Programming
     *   Vol. 2 - Seminumerical Algorithms, Knuth
     */

    /*
     * Normalize the divisor, d, and dividend, u0, u1
     */
    s = ali_algo_clz(d);
    d = d << s;

    u1 = u1 << s;
    u1 |= (u0 >> (biL - s)) & (-(ali_algo_mpi_sint)s >> (biL - 1));
    u0 = u0 << s;

    d1 = d >> biH;
    d0 = d & uint_halfword_mask;

    u0_msw = u0 >> biH;
    u0_lsw = u0 & uint_halfword_mask;

    /*
     * Find the first quotient and remainder
     */
    q1 = u1 / d1;
    r0 = u1 - d1 * q1;

    while (q1 >= radix || (q1 * d0 > radix * r0 + u0_msw)) {
        q1 -= 1;
        r0 += d1;

        if (r0 >= radix) {
            break;
        }
    }

    rAX = (u1 * radix) + (u0_msw - q1 * d);
    q0  = rAX / d1;
    r0  = rAX - q0 * d1;

    while (q0 >= radix || (q0 * d0 > radix * r0 + u0_lsw)) {
        q0 -= 1;
        r0 += d1;

        if (r0 >= radix) {
            break;
        }
    }

    if (r != NULL) {
        *r = (rAX * radix + u0_lsw - q0 * d) >> s;
    }

    quotient = q1 * radix + q0;

    return quotient;
#endif
}

/*
 * Division by ali_algo_mpi: A = Q * B + R  (HAC 14.20)
 */
int ali_algo_mpi_div_mpi(ali_algo_mpi *Q, ali_algo_mpi *R, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int         ret;
    size_t      i, n, t, k;
    ali_algo_mpi X, Y, Z, T1, T2;

    if (ali_algo_mpi_cmp_int(B, 0) == 0) {
        return (ALI_ALGO_ERR_MPI_DIVISION_BY_ZERO);
    }

    ali_algo_mpi_init(&X);
    ali_algo_mpi_init(&Y);
    ali_algo_mpi_init(&Z);
    ali_algo_mpi_init(&T1);
    ali_algo_mpi_init(&T2);

    if (ali_algo_mpi_cmp_abs(A, B) < 0) {
        if (Q != NULL) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(Q, 0));
        }
        if (R != NULL) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(R, A));
        }
        return (0);
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&X, A));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&Y, B));
    X.s = Y.s = 1;

    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&Z, A->n + 2));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&Z, 0));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&T1, 2));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&T2, 3));

    k = ali_algo_mpi_bitlen(&Y) % biL;
    if (k < biL - 1) {
        k = biL - 1 - k;
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&X, k));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&Y, k));
    } else {
        k = 0;
    }

    n = X.n - 1;
    t = Y.n - 1;
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&Y, biL * (n - t)));

    while (ali_algo_mpi_cmp_mpi(&X, &Y) >= 0) {
        Z.p[n - t]++;
        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&X, &X, &Y));
    }
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&Y, biL * (n - t)));

    for (i = n; i > t; i--) {
        if (X.p[i] >= Y.p[t]) {
            Z.p[i - t - 1] = ~0;
        } else {
            Z.p[i - t - 1] =
              ali_algo_int_div_int(X.p[i], X.p[i - 1], Y.p[t], NULL);
        }

        Z.p[i - t - 1]++;
        do {
            Z.p[i - t - 1]--;

            ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&T1, 0));
            T1.p[0] = (t < 1) ? 0 : Y.p[t - 1];
            T1.p[1] = Y.p[t];
            ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_int(&T1, &T1, Z.p[i - t - 1]));

            ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&T2, 0));
            T2.p[0] = (i < 2) ? 0 : X.p[i - 2];
            T2.p[1] = (i < 1) ? 0 : X.p[i - 1];
            T2.p[2] = X.p[i];
        } while (ali_algo_mpi_cmp_mpi(&T1, &T2) > 0);

        ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_int(&T1, &Y, Z.p[i - t - 1]));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&T1, biL * (i - t - 1)));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&X, &X, &T1));

        if (ali_algo_mpi_cmp_int(&X, 0) < 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&T1, &Y));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&T1, biL * (i - t - 1)));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&X, &X, &T1));
            Z.p[i - t - 1]--;
        }
    }

    if (Q != NULL) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(Q, &Z));
        Q->s = A->s * B->s;
    }

    if (R != NULL) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&X, k));
        X.s = A->s;
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(R, &X));

        if (ali_algo_mpi_cmp_int(R, 0) == 0) {
            R->s = 1;
        }
    }

cleanup:

    ali_algo_mpi_free(&X);
    ali_algo_mpi_free(&Y);
    ali_algo_mpi_free(&Z);
    ali_algo_mpi_free(&T1);
    ali_algo_mpi_free(&T2);

    return (ret);
}

/*
 * Division by int: A = Q * b + R
 */
int ali_algo_mpi_div_int(ali_algo_mpi *Q, ali_algo_mpi *R, const ali_algo_mpi *A,
                        ali_algo_mpi_sint b)
{
    ali_algo_mpi      _B;
    ali_algo_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (ali_algo_mpi_div_mpi(Q, R, A, &_B));
}

/*
 * Modulo: R = A mod B
 */
int ali_algo_mpi_mod_mpi(ali_algo_mpi *R, const ali_algo_mpi *A,
                        const ali_algo_mpi *B)
{
    int ret;

    if (ali_algo_mpi_cmp_int(B, 0) < 0) {
        return (ALI_ALGO_ERR_MPI_NEGATIVE_VALUE);
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_div_mpi(NULL, R, A, B));

    while (ali_algo_mpi_cmp_int(R, 0) < 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(R, R, B));
    }

    while (ali_algo_mpi_cmp_mpi(R, B) >= 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(R, R, B));
    }

cleanup:

    return (ret);
}

/*
 * Modulo: r = A mod b
 */
int ali_algo_mpi_mod_int(ali_algo_mpi_uint *r, const ali_algo_mpi *A,
                        ali_algo_mpi_sint b)
{
    size_t           i;
    ali_algo_mpi_uint x, y, z;

    if (b == 0) {
        return (ALI_ALGO_ERR_MPI_DIVISION_BY_ZERO);
    }

    if (b < 0) {
        return (ALI_ALGO_ERR_MPI_NEGATIVE_VALUE);
    }

    /*
     * handle trivial cases
     */
    if (b == 1) {
        *r = 0;
        return (0);
    }

    if (b == 2) {
        *r = A->p[0] & 1;
        return (0);
    }

    /*
     * general case
     */
    for (i = A->n, y = 0; i > 0; i--) {
        x = A->p[i - 1];
        y = (y << biH) | (x >> biH);
        z = y / b;
        y -= z * b;

        x <<= biH;
        y = (y << biH) | (x >> biH);
        z = y / b;
        y -= z * b;
    }

    /*
     * If A is negative, then the current y represents a negative value.
     * Flipping it to the positive side.
     */
    if (A->s < 0 && y != 0) {
        y = b - y;
    }

    *r = y;

    return (0);
}

/*
 * Fast Montgomery initialization (thanks to Tom St Denis)
 */
static void mpi_montg_init(ali_algo_mpi_uint *mm, const ali_algo_mpi *N)
{
    ali_algo_mpi_uint x, m0 = N->p[0];
    unsigned int     i;

    x = m0;
    x += ((m0 + 2) & 4) << 1;

    for (i = biL; i >= 8; i /= 2) {
        x *= (2 - (m0 * x));
    }

    *mm = ~x + 1;
}

/*
 * Montgomery multiplication: A = A * B * R^-1 mod N  (HAC 14.36)
 */
static int mpi_montmul(ali_algo_mpi *A, const ali_algo_mpi *B,
                       const ali_algo_mpi *N, ali_algo_mpi_uint mm,
                       const ali_algo_mpi *T)
{
    size_t           i, n, m;
    ali_algo_mpi_uint u0, u1, *d;

    if (T->n < N->n + 1 || T->p == NULL) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    osa_memset(T->p, 0, T->n * ciL);

    d = T->p;
    n = N->n;
    m = (B->n < n) ? B->n : n;

    for (i = 0; i < n; i++) {
        /*
         * T = (T + u0*B + u1*N) / 2^biL
         */
        u0 = A->p[i];
        u1 = (d[0] + u0 * B->p[0]) * mm;

        mpi_mul_hlp(m, B->p, d, u0);
        mpi_mul_hlp(n, N->p, d, u1);

        *d++     = u0;
        d[n + 1] = 0;
    }

    osa_memcpy(A->p, d, (n + 1) * ciL);

    if (ali_algo_mpi_cmp_abs(A, N) >= 0) {
        mpi_sub_hlp(n, N->p, A->p);
    } else
    /* prevent timing attacks */
    {
        mpi_sub_hlp(n, A->p, T->p);
    }

    return (0);
}

/*
 * Montgomery reduction: A = A * R^-1 mod N
 */
static int mpi_montred(ali_algo_mpi *A, const ali_algo_mpi *N,
                       ali_algo_mpi_uint mm, const ali_algo_mpi *T)
{
    ali_algo_mpi_uint z = 1;
    ali_algo_mpi      U;

    U.n = U.s = (int)z;
    U.p       = &z;

    return (mpi_montmul(A, &U, N, mm, T));
}

/*
 * Sliding-window exponentiation: X = A^E mod N  (HAC 14.85)
 */
int ali_algo_mpi_exp_mod(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *E, const ali_algo_mpi *N,
                        ali_algo_mpi *_RR)
{
    int              ret;
    size_t           wbits, wsize, one = 1;
    size_t           i, j, nblimbs;
    size_t           bufsize, nbits;
    ali_algo_mpi_uint ei, mm, state;
    ali_algo_mpi      RR, T, W[2 << ALI_ALGO_MPI_WINDOW_SIZE], Apos;
    int              neg;

    if (ali_algo_mpi_cmp_int(N, 0) < 0 || (N->p[0] & 1) == 0) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    if (ali_algo_mpi_cmp_int(E, 0) < 0) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    /*
     * Init temps and window size
     */
    mpi_montg_init(&mm, N);
    ali_algo_mpi_init(&RR);
    ali_algo_mpi_init(&T);
    ali_algo_mpi_init(&Apos);
    osa_memset(W, 0, sizeof(W));

    i = ali_algo_mpi_bitlen(E);

    wsize = (i > 671) ? 6 : (i > 239) ? 5 : (i > 79) ? 4 : (i > 23) ? 3 : 1;

    if (wsize > ALI_ALGO_MPI_WINDOW_SIZE) {
        wsize = ALI_ALGO_MPI_WINDOW_SIZE;
    }

    j = N->n + 1;
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(X, j));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&W[1], j));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&T, j * 2));

    /*
     * Compensate for negative A (and correct at the end)
     */
    neg = (A->s == -1);
    if (neg) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&Apos, A));
        Apos.s = 1;
        A      = &Apos;
    }

    /*
     * If 1st call, pre-compute R^2 mod N
     */
    if (_RR == NULL || _RR->p == NULL) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&RR, 1));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&RR, N->n * 2 * biL));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&RR, &RR, N));

        if (_RR != NULL) {
            osa_memcpy(_RR, &RR, sizeof(ali_algo_mpi));
        }
    } else {
        osa_memcpy(&RR, _RR, sizeof(ali_algo_mpi));
    }

    /*
     * W[1] = A * R^2 * R^-1 mod N = A * R mod N
     */
    if (ali_algo_mpi_cmp_mpi(A, N) >= 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&W[1], A, N));
    } else {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&W[1], A));
    }

    ALI_ALGO_MPI_CHK(mpi_montmul(&W[1], &RR, N, mm, &T));

    /*
     * X = R^2 * R^-1 mod N = R mod N
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(X, &RR));
    ALI_ALGO_MPI_CHK(mpi_montred(X, N, mm, &T));

    if (wsize > 1) {
        /*
         * W[1 << (wsize - 1)] = W[1] ^ (wsize - 1)
         */
        j = one << (wsize - 1);

        ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&W[j], N->n + 1));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&W[j], &W[1]));

        for (i = 0; i < wsize - 1; i++) {
            ALI_ALGO_MPI_CHK(mpi_montmul(&W[j], &W[j], N, mm, &T));
        }

        /*
         * W[i] = W[i - 1] * W[1]
         */
        for (i = j + 1; i < (one << wsize); i++) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_grow(&W[i], N->n + 1));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&W[i], &W[i - 1]));

            ALI_ALGO_MPI_CHK(mpi_montmul(&W[i], &W[1], N, mm, &T));
        }
    }

    nblimbs = E->n;
    bufsize = 0;
    nbits   = 0;
    wbits   = 0;
    state   = 0;

    while (1) {
        if (bufsize == 0) {
            if (nblimbs == 0) {
                break;
            }

            nblimbs--;

            bufsize = sizeof(ali_algo_mpi_uint) << 3;
        }

        bufsize--;

        ei = (E->p[nblimbs] >> bufsize) & 1;

        /*
         * skip leading 0s
         */
        if (ei == 0 && state == 0) {
            continue;
        }

        if (ei == 0 && state == 1) {
            /*
             * out of window, square X
             */
            ALI_ALGO_MPI_CHK(mpi_montmul(X, X, N, mm, &T));
            continue;
        }

        /*
         * add ei to current window
         */
        state = 2;

        nbits++;
        wbits |= (ei << (wsize - nbits));

        if (nbits == wsize) {
            /*
             * X = X^wsize R^-1 mod N
             */
            for (i = 0; i < wsize; i++) {
                ALI_ALGO_MPI_CHK(mpi_montmul(X, X, N, mm, &T));
            }

            /*
             * X = X * W[wbits] R^-1 mod N
             */
            ALI_ALGO_MPI_CHK(mpi_montmul(X, &W[wbits], N, mm, &T));

            state--;
            nbits = 0;
            wbits = 0;
        }
    }

    /*
     * process the remaining bits
     */
    for (i = 0; i < nbits; i++) {
        ALI_ALGO_MPI_CHK(mpi_montmul(X, X, N, mm, &T));

        wbits <<= 1;

        if ((wbits & (one << wsize)) != 0) {
            ALI_ALGO_MPI_CHK(mpi_montmul(X, &W[1], N, mm, &T));
        }
    }

    /*
     * X = A^E * R * R^-1 mod N = A^E mod N
     */
    ALI_ALGO_MPI_CHK(mpi_montred(X, N, mm, &T));

    if (neg) {
        X->s = -1;
        ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(X, N, X));
    }

cleanup:

    for (i = (one << (wsize - 1)); i < (one << wsize); i++) {
        ali_algo_mpi_free(&W[i]);
    }

    ali_algo_mpi_free(&W[1]);
    ali_algo_mpi_free(&T);
    ali_algo_mpi_free(&Apos);

    if (_RR == NULL || _RR->p == NULL) {
        ali_algo_mpi_free(&RR);
    }

    return (ret);
}

/*
 * Greatest common divisor: G = gcd(A, B)  (HAC 14.54)
 */
int ali_algo_mpi_gcd(ali_algo_mpi *G, const ali_algo_mpi *A, const ali_algo_mpi *B)
{
    int         ret;
    size_t      lz, lzt;
    ali_algo_mpi TG, TA, TB;

    ali_algo_mpi_init(&TG);
    ali_algo_mpi_init(&TA);
    ali_algo_mpi_init(&TB);

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TA, A));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TB, B));

    lz  = ali_algo_mpi_lsb(&TA);
    lzt = ali_algo_mpi_lsb(&TB);

    if (lzt < lz) {
        lz = lzt;
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TA, lz));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TB, lz));

    TA.s = TB.s = 1;

    while (ali_algo_mpi_cmp_int(&TA, 0) != 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TA, ali_algo_mpi_lsb(&TA)));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TB, ali_algo_mpi_lsb(&TB)));

        if (ali_algo_mpi_cmp_mpi(&TA, &TB) >= 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(&TA, &TA, &TB));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TA, 1));
        } else {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_abs(&TB, &TB, &TA));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TB, 1));
        }
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l(&TB, lz));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(G, &TB));

cleanup:

    ali_algo_mpi_free(&TG);
    ali_algo_mpi_free(&TA);
    ali_algo_mpi_free(&TB);

    return (ret);
}

/*
 * Fill X with size bytes of random.
 *
 * Use a temporary bytes representation to make sure the result is the same
 * regardless of the platform endianness (useful when f_rng is actually
 * deterministic, eg for tests).
 */
int ali_algo_mpi_fill_random(ali_algo_mpi *X, size_t size,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int           ret;
    unsigned char buf[ALI_ALGO_MPI_MAX_SIZE];

    if (size > ALI_ALGO_MPI_MAX_SIZE) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    ALI_ALGO_MPI_CHK(f_rng(p_rng, buf, size));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_binary(X, buf, size));

cleanup:
    return (ret);
}

/*
 * Modular inverse: X = A^-1 mod N  (HAC 14.61 / 14.64)
 */
int ali_algo_mpi_inv_mod(ali_algo_mpi *X, const ali_algo_mpi *A,
                        const ali_algo_mpi *N)
{
    int         ret;
    ali_algo_mpi G, TA, TU, U1, U2, TB, TV, V1, V2;

    if (ali_algo_mpi_cmp_int(N, 0) <= 0) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    ali_algo_mpi_init(&TA);
    ali_algo_mpi_init(&TU);
    ali_algo_mpi_init(&U1);
    ali_algo_mpi_init(&U2);
    ali_algo_mpi_init(&G);
    ali_algo_mpi_init(&TB);
    ali_algo_mpi_init(&TV);
    ali_algo_mpi_init(&V1);
    ali_algo_mpi_init(&V2);

    ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&G, A, N));

    if (ali_algo_mpi_cmp_int(&G, 1) != 0) {
        ret = ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&TA, A, N));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TU, &TA));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TB, N));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&TV, N));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&U1, 1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&U2, 0));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&V1, 0));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&V2, 1));

    do {
        while ((TU.p[0] & 1) == 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TU, 1));

            if ((U1.p[0] & 1) != 0 || (U2.p[0] & 1) != 0) {
                ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&U1, &U1, &TB));
                ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&U2, &U2, &TA));
            }

            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&U1, 1));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&U2, 1));
        }

        while ((TV.p[0] & 1) == 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&TV, 1));

            if ((V1.p[0] & 1) != 0 || (V2.p[0] & 1) != 0) {
                ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&V1, &V1, &TB));
                ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&V2, &V2, &TA));
            }

            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&V1, 1));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&V2, 1));
        }

        if (ali_algo_mpi_cmp_mpi(&TU, &TV) >= 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&TU, &TU, &TV));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&U1, &U1, &V1));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&U2, &U2, &V2));
        } else {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&TV, &TV, &TU));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&V1, &V1, &U1));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&V2, &V2, &U2));
        }
    } while (ali_algo_mpi_cmp_int(&TU, 0) != 0);

    while (ali_algo_mpi_cmp_int(&V1, 0) < 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&V1, &V1, N));
    }

    while (ali_algo_mpi_cmp_mpi(&V1, N) >= 0) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_mpi(&V1, &V1, N));
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(X, &V1));

cleanup:

    ali_algo_mpi_free(&TA);
    ali_algo_mpi_free(&TU);
    ali_algo_mpi_free(&U1);
    ali_algo_mpi_free(&U2);
    ali_algo_mpi_free(&G);
    ali_algo_mpi_free(&TB);
    ali_algo_mpi_free(&TV);
    ali_algo_mpi_free(&V1);
    ali_algo_mpi_free(&V2);

    return (ret);
}

#if defined(ALI_ALGO_GENPRIME)

static const int small_prime[] = {
    3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,
    53,  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107, 109,
    113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191,
    193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269,
    271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353,
    359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439,
    443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
    541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617,
    619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709,
    719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811,
    821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907,
    911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, -103
};

/*
 * Small divisors test (X must be positive)
 *
 * Return values:
 * 0: no small factor (possible prime, more tests needed)
 * 1: certain prime
 * ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE: certain non-prime
 * other negative: error
 */
static int mpi_check_small_factors(const ali_algo_mpi *X)
{
    int              ret = 0;
    size_t           i;
    ali_algo_mpi_uint r;

    if ((X->p[0] & 1) == 0) {
        return (ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE);
    }

    for (i = 0; small_prime[i] > 0; i++) {
        if (ali_algo_mpi_cmp_int(X, small_prime[i]) <= 0) {
            return (1);
        }

        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_int(&r, X, small_prime[i]));

        if (r == 0) {
            return (ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE);
        }
    }

cleanup:
    return (ret);
}

/*
 * Miller-Rabin pseudo-primality test  (HAC 4.24)
 */
static int mpi_miller_rabin(const ali_algo_mpi *X,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int         ret, count;
    size_t      i, j, k, n, s;
    ali_algo_mpi W, R, T, A, RR;

    ali_algo_mpi_init(&W);
    ali_algo_mpi_init(&R);
    ali_algo_mpi_init(&T);
    ali_algo_mpi_init(&A);
    ali_algo_mpi_init(&RR);

    /*
     * W = |X| - 1
     * R = W >> lsb( W )
     */
    ALI_ALGO_MPI_CHK(ali_algo_mpi_sub_int(&W, X, 1));
    s = ali_algo_mpi_lsb(&W);
    ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&R, &W));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&R, s));

    i = ali_algo_mpi_bitlen(X);
    /*
     * HAC, table 4.4
     */
    n = ((i >= 1300)
           ? 2
           : (i >= 850)
               ? 3
               : (i >= 650)
                   ? 4
                   : (i >= 350) ? 8 : (i >= 250) ? 12 : (i >= 150) ? 18 : 27);

    for (i = 0; i < n; i++) {
        /*
         * pick a random A, 1 < A < |X| - 1
         */
        ALI_ALGO_MPI_CHK(ali_algo_mpi_fill_random(&A, X->n * ciL, f_rng, p_rng));

        if (ali_algo_mpi_cmp_mpi(&A, &W) >= 0) {
            j = ali_algo_mpi_bitlen(&A) - ali_algo_mpi_bitlen(&W);
            ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&A, j + 1));
        }
        A.p[0] |= 3;

        count = 0;
        do {
            ALI_ALGO_MPI_CHK(
              ali_algo_mpi_fill_random(&A, X->n * ciL, f_rng, p_rng));

            j = ali_algo_mpi_bitlen(&A);
            k = ali_algo_mpi_bitlen(&W);
            if (j > k) {
                ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&A, j - k));
            }

            if (count++ > 30) {
                ret = ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE;
                goto cleanup;
            }

        } while (ali_algo_mpi_cmp_mpi(&A, &W) >= 0 ||
                 ali_algo_mpi_cmp_int(&A, 1) <= 0);

        /*
         * A = A^R mod |X|
         */
        ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&A, &A, &R, X, &RR));

        if (ali_algo_mpi_cmp_mpi(&A, &W) == 0 ||
            ali_algo_mpi_cmp_int(&A, 1) == 0) {
            continue;
        }

        j = 1;
        while (j < s && ali_algo_mpi_cmp_mpi(&A, &W) != 0) {
            /*
             * A = A * A mod |X|
             */
            ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&T, &A, &A));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&A, &T, X));

            if (ali_algo_mpi_cmp_int(&A, 1) == 0) {
                break;
            }

            j++;
        }

        /*
         * not prime if A != |X| - 1 or A == 1
         */
        if (ali_algo_mpi_cmp_mpi(&A, &W) != 0 ||
            ali_algo_mpi_cmp_int(&A, 1) == 0) {
            ret = ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE;
            break;
        }
    }

cleanup:
    ali_algo_mpi_free(&W);
    ali_algo_mpi_free(&R);
    ali_algo_mpi_free(&T);
    ali_algo_mpi_free(&A);
    ali_algo_mpi_free(&RR);

    return (ret);
}

/*
 * Pseudo-primality test: small factors, then Miller-Rabin
 */
int ali_algo_mpi_is_prime(const ali_algo_mpi *X,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng)
{
    int         ret;
    ali_algo_mpi XX;


    XX.s = 1;
    XX.n = X->n;
    XX.p = X->p;

    if (ali_algo_mpi_cmp_int(&XX, 0) == 0 || ali_algo_mpi_cmp_int(&XX, 1) == 0) {
        return (ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE);
    }

    if (ali_algo_mpi_cmp_int(&XX, 2) == 0) {
        return (0);
    }

    if ((ret = mpi_check_small_factors(&XX)) != 0) {
        if (ret == 1) {
            return (0);
        }

        return (ret);
    }
    ret = mpi_miller_rabin(&XX, f_rng, p_rng);

    return (ret);
}

/*
 * Prime number generation
 */
int ali_algo_mpi_gen_prime(ali_algo_mpi *X, size_t nbits, int dh_flag,
                          int (*f_rng)(void *, unsigned char *, size_t),
                          void *p_rng)
{
    int              ret;
    size_t           k, n;
    ali_algo_mpi_uint r;
    ali_algo_mpi      Y;

    if (nbits < 3 || nbits > ALI_ALGO_MPI_MAX_BITS) {
        return (ALI_ALGO_ERR_MPI_BAD_INPUT_DATA);
    }

    ali_algo_mpi_init(&Y);

    n = BITS_TO_LIMBS(nbits);

    ALI_ALGO_MPI_CHK(ali_algo_mpi_fill_random(X, n * ciL, f_rng, p_rng));

    k = ali_algo_mpi_bitlen(X);
    if (k > nbits) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(X, k - nbits + 1));
    }

    ali_algo_mpi_set_bit(X, nbits - 1, 1);

    X->p[0] |= 1;
    if (dh_flag == 0) {
        while ((ret = ali_algo_mpi_is_prime(X, f_rng, p_rng)) != 0) {
            if (ret != ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE) {
                goto cleanup;
            }

            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(X, X, 2));
        }
    } else {
        /*
         * An necessary condition for Y and X = 2Y + 1 to be prime
         * is X = 2 mod 3 (which is equivalent to Y = 2 mod 3).
         * Make sure it is satisfied, while keeping X = 3 mod 4
         */
        X->p[0] |= 2;

        ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_int(&r, X, 3));
        if (r == 0) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(X, X, 8));
        } else if (r == 1) {
            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(X, X, 4));
        }

        /* Set Y = (X-1) / 2, which is X / 2 because X is odd */
        ALI_ALGO_MPI_CHK(ali_algo_mpi_copy(&Y, X));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_r(&Y, 1));
        while (1) {
            /*
             * First, check small factors for X and Y
             * before doing Miller-Rabin on any of them
             */
            if ((ret = mpi_check_small_factors(X)) == 0 &&
                (ret = mpi_check_small_factors(&Y)) == 0 &&
                (ret = mpi_miller_rabin(X, f_rng, p_rng)) == 0 &&
                (ret = mpi_miller_rabin(&Y, f_rng, p_rng)) == 0) {
                break;
            }

            if (ret != ALI_ALGO_ERR_MPI_NOT_ACCEPTABLE) {
                goto cleanup;
            }

            /*
             * Next candidates. We want to preserve Y = (X-1) / 2 and
             * Y = 1 mod 2 and Y = 2 mod 3 (eq X = 3 mod 4 and X = 2 mod 3)
             * so up Y by 6 and X by 12.
             */
            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(X, X, 12));
            ALI_ALGO_MPI_CHK(ali_algo_mpi_add_int(&Y, &Y, 6));
        }
    }

cleanup:

    ali_algo_mpi_free(&Y);

    return (ret);
}

#endif /* ALI_ALGO_GENPRIME */

#if defined(ALI_ALGO_SELF_TEST)

#define GCD_PAIR_COUNT 3

static const int gcd_pairs[GCD_PAIR_COUNT][3] = { { 693, 609, 21 },
                                                  { 1764, 868, 28 },
                                                  { 768454923, 542167814, 1 } };

/*
 * Checkup routine
 */
int ali_algo_mpi_self_test(int verbose)
{
    int         ret, i;
    ali_algo_mpi A, E, N, X, Y, U, V;

    ali_algo_mpi_init(&A);
    ali_algo_mpi_init(&E);
    ali_algo_mpi_init(&N);
    ali_algo_mpi_init(&X);
    ali_algo_mpi_init(&Y);
    ali_algo_mpi_init(&U);
    ali_algo_mpi_init(&V);

    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_read_string(&A, 16,
                              "EFE021C2645FD1DC586E69184AF4A31E"
                              "D5F53E93B5F123FA41680867BA110131"
                              "944FE7952E2517337780CB0DB80E61AA"
                              "E7C8DDC6C5C6AADEB34EB38A2F40D5E6"));

    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_read_string(&E, 16,
                              "B2E7EFD37075B9F03FF989C7C5051C20"
                              "34D2A323810251127E7BF8625A4F49A5"
                              "F3E27F4DA8BD59C47D6DAABA4C8127BD"
                              "5B5C25763222FEFCCFC38B832366C29E"));

    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_read_string(&N, 16,
                              "0066A198186C18C10B2F5ED9B522752A"
                              "9830B69916E535C8F047518A889A43A5"
                              "94B6BED27A168D31D4A52F88925AA8F5"));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&X, &A, &N));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&U, 16,
                                            "602AB7ECA597A3D6B56FF9829A5E8B85"
                                            "9E857EA95A03512E2BAE7391688D264A"
                                            "A5663B0341DB9CCFD2C4C5F421FEC814"
                                            "8001B72E848A38CAE1C65F78E56ABDEF"
                                            "E12D3C039B8A02D6BE593F0BBBDA56F1"
                                            "ECF677152EF804370C1A305CAF3B5BF1"
                                            "30879B56C61DE584A0F53A2447A51E"));

    if (verbose != 0) {
        osa_printf("  MPI test #1 (mul_mpi): ");
    }

    if (ali_algo_mpi_cmp_mpi(&X, &U) != 0) {
        if (verbose != 0) {
            osa_printf("failed\n");
        }

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        osa_printf("passed\n");
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_div_mpi(&X, &Y, &A, &N));

    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_read_string(&U, 16, "256567336059E52CAE22925474705F39A94"));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&V, 16,
                                            "6613F26162223DF488E9CD48CC132C7A"
                                            "0AC93C701B001B092E4E5B9F73BCD27B"
                                            "9EE50D0657C77F374E903CDFA4C642"));

    if (verbose != 0) {
        osa_printf("  MPI test #2 (div_mpi): ");
    }

    if (ali_algo_mpi_cmp_mpi(&X, &U) != 0 || ali_algo_mpi_cmp_mpi(&Y, &V) != 0) {
        if (verbose != 0) {
            osa_printf("failed\n");
        }

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        osa_printf("passed\n");
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_exp_mod(&X, &A, &E, &N, NULL));

    ALI_ALGO_MPI_CHK(ali_algo_mpi_read_string(&U, 16,
                                            "36E139AEA55215609D2816998ED020BB"
                                            "BD96C37890F65171D948E9BC7CBAA4D9"
                                            "325D24D6A3C12710F10A09FA08AB87"));

    if (verbose != 0) {
        osa_printf("  MPI test #3 (exp_mod): ");
    }

    if (ali_algo_mpi_cmp_mpi(&X, &U) != 0) {
        if (verbose != 0) {
            osa_printf("failed\n");
        }

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        osa_printf("passed\n");
    }

    ALI_ALGO_MPI_CHK(ali_algo_mpi_inv_mod(&X, &A, &N));

    ALI_ALGO_MPI_CHK(
      ali_algo_mpi_read_string(&U, 16,
                              "003A0AAEDD7E784FC07D8F9EC6E3BFD5"
                              "C3DBA76456363A10869622EAC2DD84EC"
                              "C5B8A74DAC4D09E03B5E0BE779F2DF61"));

    if (verbose != 0) {
        osa_printf("  MPI test #4 (inv_mod): ");
    }

    if (ali_algo_mpi_cmp_mpi(&X, &U) != 0) {
        if (verbose != 0) {
            osa_printf("failed\n");
        }

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        osa_printf("passed\n");
    }

    if (verbose != 0) {
        osa_printf("  MPI test #5 (simple gcd): ");
    }

    for (i = 0; i < GCD_PAIR_COUNT; i++) {
        ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&X, gcd_pairs[i][0]));
        ALI_ALGO_MPI_CHK(ali_algo_mpi_lset(&Y, gcd_pairs[i][1]));

        ALI_ALGO_MPI_CHK(ali_algo_mpi_gcd(&A, &X, &Y));

        if (ali_algo_mpi_cmp_int(&A, gcd_pairs[i][2]) != 0) {
            if (verbose != 0) {
                osa_printf("failed at %d\n", i);
            }

            ret = 1;
            goto cleanup;
        }
    }

    if (verbose != 0) {
        osa_printf("passed\n");
    }

cleanup:

    if (ret != 0 && verbose != 0) {
        osa_printf("Unexpected error, return code = %08X\n", ret);
    }

    ali_algo_mpi_free(&A);
    ali_algo_mpi_free(&E);
    ali_algo_mpi_free(&N);
    ali_algo_mpi_free(&X);
    ali_algo_mpi_free(&Y);
    ali_algo_mpi_free(&U);
    ali_algo_mpi_free(&V);

    if (verbose != 0) {
        osa_printf("\n");
    }

    return (ret);
}

#endif /* ALI_ALGO_SELF_TEST */

#endif /* ALI_ALGO_BIGNUM_C */
