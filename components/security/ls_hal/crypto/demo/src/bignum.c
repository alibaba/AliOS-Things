/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#include "ls_osa.h"
#include "bignum.h"
#include "bn_mul.h"

/* Implementation that should never be optimized out by the compiler */
static void impl_mpi_zeroize(impl_mpi_uint *v, size_t n)
{
    volatile impl_mpi_uint *p = v;
    while (n--) {
        *p++ = 0;
    }
}

#define ciL (sizeof(impl_mpi_uint)) /* chars in limb  */
#define biL (ciL << 3)                 /* bits  in limb  */
#define biH (ciL << 2)                 /* implf limb size */

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
void impl_mpi_init(impl_mpi *X)
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
void impl_mpi_free(impl_mpi *X)
{
    if (X == NULL) {
        return;
    }

    if (X->p != NULL) {
        impl_mpi_zeroize(X->p, X->n);
        ls_osa_free(X->p);
    }

    X->s = 1;
    X->n = 0;
    X->p = NULL;
}

/*
 * Enlarge to the specified number of limbs
 */
int impl_mpi_grow(impl_mpi *X, size_t nblimbs)
{
    impl_mpi_uint *p;

    if (nblimbs > HAL_MPI_MAX_LIMBS) {
        return (HAL_ERR_MPI_ALLOC_FAILED);
    }

    if (X->n < nblimbs) {
        if ((p = (impl_mpi_uint *)ls_osa_calloc(nblimbs, ciL)) == NULL) {
            return (HAL_ERR_MPI_ALLOC_FAILED);
        }

        if (X->p != NULL) {
            memcpy(p, X->p, X->n * ciL);
            impl_mpi_zeroize(X->p, X->n);
            ls_osa_free(X->p);
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
int impl_mpi_shrink(impl_mpi *X, size_t nblimbs)
{
    impl_mpi_uint *p;
    size_t            i;

    /* Actually resize up in this case */
    if (X->n <= nblimbs) {
        return (impl_mpi_grow(X, nblimbs));
    }

    for (i = X->n - 1; i > 0; i--)
        if (X->p[i] != 0) {
            break;
        }
    i++;

    if (i < nblimbs) {
        i = nblimbs;
    }

    if ((p = (impl_mpi_uint *)ls_osa_calloc(i, ciL)) == NULL) {
        return (HAL_ERR_MPI_ALLOC_FAILED);
    }

    if (X->p != NULL) {
        memcpy(p, X->p, i * ciL);
        impl_mpi_zeroize(X->p, X->n);
        ls_osa_free(X->p);
    }

    X->n = i;
    X->p = p;

    return (0);
}

/*
 * Copy the contents of Y into X
 */
int impl_mpi_copy(impl_mpi *X, const impl_mpi *Y)
{
    int    ret;
    size_t i;

    if (X == Y) {
        return (0);
    }

    if (Y->p == NULL) {
        impl_mpi_free(X);
        return (0);
    }

    for (i = Y->n - 1; i > 0; i--)
        if (Y->p[i] != 0) {
            break;
        }
    i++;

    X->s = Y->s;

    HAL_MPI_CHK(impl_mpi_grow(X, i));

    memset(X->p, 0, X->n * ciL);
    memcpy(X->p, Y->p, i * ciL);

cleanup:

    return (ret);
}

/*
 * Swap the contents of X and Y
 */
void impl_mpi_swap(impl_mpi *X, impl_mpi *Y)
{
    impl_mpi T;

    memcpy(&T, X, sizeof(impl_mpi));
    memcpy(X, Y, sizeof(impl_mpi));
    memcpy(Y, &T, sizeof(impl_mpi));
}

/*
 * Conditionally assign X = Y, without leaking information
 * about whether the assignment was made or not.
 * (Leaking information about the respective sizes of X and Y is ok however.)
 */
int impl_mpi_safe_cond_assign(impl_mpi *X, const impl_mpi *Y,
                                 unsigned char assign)
{
    int    ret = 0;
    size_t i;

    /* make sure assign is 0 or 1 in a time-constant manner */
    assign = (assign | (unsigned char)-assign) >> 7;

    HAL_MPI_CHK(impl_mpi_grow(X, Y->n));

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
int impl_mpi_safe_cond_swap(impl_mpi *X, impl_mpi *Y,
                               unsigned char swap)
{
    int              ret, s;
    size_t           i;
    impl_mpi_uint tmp;

    if (X == Y) {
        return (0);
    }

    /* make sure swap is 0 or 1 in a time-constant manner */
    swap = (swap | (unsigned char)-swap) >> 7;

    HAL_MPI_CHK(impl_mpi_grow(X, Y->n));
    HAL_MPI_CHK(impl_mpi_grow(Y, X->n));

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
int impl_mpi_lset(impl_mpi *X, impl_mpi_sint z)
{
    int ret;

    HAL_MPI_CHK(impl_mpi_grow(X, 1));
    memset(X->p, 0, X->n * ciL);

    X->p[0] = (z < 0) ? -z : z;
    X->s    = (z < 0) ? -1 : 1;

cleanup:

    return (ret);
}

/*
 * Get a specific bit
 */
int impl_mpi_get_bit(const impl_mpi *X, size_t pos)
{
    if (X->n * biL <= pos) {
        return (0);
    }

    return ((X->p[pos / biL] >> (pos % biL)) & 0x01);
}

/*
 * Set a bit to a specific value of 0 or 1
 */
int impl_mpi_set_bit(impl_mpi *X, size_t pos, unsigned char val)
{
    int    ret = 0;
    size_t off = pos / biL;
    size_t idx = pos % biL;

    if (val != 0 && val != 1) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    if (X->n * biL <= pos) {
        if (val == 0) {
            return (0);
        }

        HAL_MPI_CHK(impl_mpi_grow(X, off + 1));
    }

    X->p[off] &= ~((impl_mpi_uint)0x01 << idx);
    X->p[off] |= (impl_mpi_uint)val << idx;

cleanup:

    return (ret);
}

/*
 * Return the number of less significant zero-bits
 */
size_t impl_mpi_lsb(const impl_mpi *X)
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
static size_t impl_clz(const impl_mpi_uint x)
{
    size_t           j;
    impl_mpi_uint mask = (impl_mpi_uint)1 << (biL - 1);

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
size_t impl_mpi_bitlen(const impl_mpi *X)
{
    size_t i, j;

    if (X->n == 0) {
        return (0);
    }

    for (i = X->n - 1; i > 0; i--)
        if (X->p[i] != 0) {
            break;
        }

    j = biL - impl_clz(X->p[i]);

    return ((i * biL) + j);
}

/*
 * Return the total size in bytes
 */
size_t impl_mpi_size(const impl_mpi *X)
{
    return ((impl_mpi_bitlen(X) + 7) >> 3);
}

/*
 * Convert an ASCII character to digit value
 */
static int mpi_get_digit(impl_mpi_uint *d, int radix, char c)
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

    if (*d >= (impl_mpi_uint)radix) {
        return (HAL_ERR_MPI_INVALID_CHARACTER);
    }

    return (0);
}

/*
 * Import from an ASCII string
 */
int impl_mpi_read_string(impl_mpi *X, int radix, const char *s)
{
    int              ret;
    size_t           i, j, slen, n;
    impl_mpi_uint d;
    impl_mpi      T;

    if (radix < 2 || radix > 16) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    impl_mpi_init(&T);

    slen = strlen(s);

    if (radix == 16) {
        if (slen > MPI_SIZE_T_MAX >> 2) {
            return (HAL_ERR_MPI_BAD_INPUT_DATA);
        }

        n = BITS_TO_LIMBS(slen << 2);

        HAL_MPI_CHK(impl_mpi_grow(X, n));
        HAL_MPI_CHK(impl_mpi_lset(X, 0));

        for (i = slen, j = 0; i > 0; i--, j++) {
            if (i == 1 && s[i - 1] == '-') {
                X->s = -1;
                break;
            }

            HAL_MPI_CHK(mpi_get_digit(&d, radix, s[i - 1]));
            X->p[j / (2 * ciL)] |= d << ((j % (2 * ciL)) << 2);
        }
    } else {
        HAL_MPI_CHK(impl_mpi_lset(X, 0));

        for (i = 0; i < slen; i++) {
            if (i == 0 && s[i] == '-') {
                X->s = -1;
                continue;
            }

            HAL_MPI_CHK(mpi_get_digit(&d, radix, s[i]));
            HAL_MPI_CHK(impl_mpi_mul_int(&T, X, radix));

            if (X->s == 1) {
                HAL_MPI_CHK(impl_mpi_add_int(X, &T, d));
            } else {
                HAL_MPI_CHK(impl_mpi_sub_int(X, &T, d));
            }
        }
    }

cleanup:

    impl_mpi_free(&T);

    return (ret);
}

/*
 * Helper to write the digits high-order first
 */
static int mpi_write_hlp(impl_mpi *X, int radix, char **p)
{
    int              ret;
    impl_mpi_uint r;

    if (radix < 2 || radix > 16) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    HAL_MPI_CHK(impl_mpi_mod_int(&r, X, radix));
    HAL_MPI_CHK(impl_mpi_div_int(X, NULL, X, radix));

    if (impl_mpi_cmp_int(X, 0) != 0) {
        HAL_MPI_CHK(mpi_write_hlp(X, radix, p));
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
int impl_mpi_write_string(const impl_mpi *X, int radix, char *buf,
                             size_t buflen, size_t *olen)
{
    int         ret = 0;
    size_t      n;
    char *      p;
    impl_mpi T;

    if (radix < 2 || radix > 16) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    n = impl_mpi_bitlen(X);
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
        return (HAL_ERR_MPI_BUFFER_TOO_SMALL);
    }

    p = buf;
    impl_mpi_init(&T);

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
        HAL_MPI_CHK(impl_mpi_copy(&T, X));

        if (T.s == -1) {
            T.s = 1;
        }

        HAL_MPI_CHK(mpi_write_hlp(&T, radix, &p));
    }

    *p++  = '\0';
    *olen = p - buf;

cleanup:

    impl_mpi_free(&T);

    return (ret);
}


/*
 * Import X from unsigned binary data, big endian
 */
int impl_mpi_read_binary(impl_mpi *X, const unsigned char *buf,
                            size_t buflen)
{
    int    ret;
    size_t i, j, n;

    for (n = 0; n < buflen; n++)
        if (buf[n] != 0) {
            break;
        }

    HAL_MPI_CHK(impl_mpi_grow(X, CHARS_TO_LIMBS(buflen - n)));
    HAL_MPI_CHK(impl_mpi_lset(X, 0));

    for (i = buflen, j = 0; i > n; i--, j++) {
        X->p[j / ciL] |= ((impl_mpi_uint)buf[i - 1]) << ((j % ciL) << 3);
    }

cleanup:

    return (ret);
}

/*
 * Export X into unsigned binary data, big endian
 */
int impl_mpi_write_binary(const impl_mpi *X, unsigned char *buf,
                             size_t buflen)
{
    size_t i, j, n;

    n = impl_mpi_size(X);

    if (buflen < n) {
        return (HAL_ERR_MPI_BUFFER_TOO_SMALL);
    }

    memset(buf, 0, buflen);

    for (i = buflen - 1, j = 0; n > 0; i--, j++, n--) {
        buf[i] = (unsigned char)(X->p[j / ciL] >> ((j % ciL) << 3));
    }

    return (0);
}

/*
 * Left-shift: X <<= count
 */
int impl_mpi_shift_l(impl_mpi *X, size_t count)
{
    int              ret;
    size_t           i, v0, t1;
    impl_mpi_uint r0 = 0, r1;

    v0 = count / (biL);
    t1 = count & (biL - 1);

    i = impl_mpi_bitlen(X) + count;

    if (X->n * biL < i) {
        HAL_MPI_CHK(impl_mpi_grow(X, BITS_TO_LIMBS(i)));
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
int impl_mpi_shift_r(impl_mpi *X, size_t count)
{
    size_t           i, v0, v1;
    impl_mpi_uint r0 = 0, r1;

    v0 = count / biL;
    v1 = count & (biL - 1);

    if (v0 > X->n || (v0 == X->n && v1 > 0)) {
        return impl_mpi_lset(X, 0);
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
int impl_mpi_cmp_abs(const impl_mpi *X, const impl_mpi *Y)
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
int impl_mpi_cmp_mpi(const impl_mpi *X, const impl_mpi *Y)
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
int impl_mpi_cmp_int(const impl_mpi *X, impl_mpi_sint z)
{
    impl_mpi      Y;
    impl_mpi_uint p[1];

    *p  = (z < 0) ? -z : z;
    Y.s = (z < 0) ? -1 : 1;
    Y.n = 1;
    Y.p = p;

    return (impl_mpi_cmp_mpi(X, &Y));
}

/*
 * Unsigned addition: X = |A| + |B|  (HAC 14.7)
 */
int impl_mpi_add_abs(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *B)
{
    int               ret;
    size_t            i, j;
    impl_mpi_uint *o, *p, c, tmp;

    if (X == B) {
        const impl_mpi *T = A;
        A                    = X;
        B                    = T;
    }

    if (X != A) {
        HAL_MPI_CHK(impl_mpi_copy(X, A));
    }

    /*
     * X should always be positive as a result of unsigned additions.
     */
    X->s = 1;

    for (j = B->n; j > 0; j--)
        if (B->p[j - 1] != 0) {
            break;
        }

    HAL_MPI_CHK(impl_mpi_grow(X, j));

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
            HAL_MPI_CHK(impl_mpi_grow(X, i + 1));
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
 * Helper for impl_mpi subtraction
 */
static void mpi_sub_hlp(size_t n, impl_mpi_uint *s, impl_mpi_uint *d)
{
    size_t           i;
    impl_mpi_uint c, z;

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
int impl_mpi_sub_abs(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *B)
{
    impl_mpi TB;
    int         ret;
    size_t      n;

    if (impl_mpi_cmp_abs(A, B) < 0) {
        return (HAL_ERR_MPI_NEGATIVE_VALUE);
    }

    impl_mpi_init(&TB);

    if (X == B) {
        HAL_MPI_CHK(impl_mpi_copy(&TB, B));
        B = &TB;
    }

    if (X != A) {
        HAL_MPI_CHK(impl_mpi_copy(X, A));
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

    impl_mpi_free(&TB);

    return (ret);
}

/*
 * Signed addition: X = A + B
 */
int impl_mpi_add_mpi(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *B)
{
    int ret, s = A->s;

    if (A->s * B->s < 0) {
        if (impl_mpi_cmp_abs(A, B) >= 0) {
            HAL_MPI_CHK(impl_mpi_sub_abs(X, A, B));
            X->s = s;
        } else {
            HAL_MPI_CHK(impl_mpi_sub_abs(X, B, A));
            X->s = -s;
        }
    } else {
        HAL_MPI_CHK(impl_mpi_add_abs(X, A, B));
        X->s = s;
    }

cleanup:

    return (ret);
}

/*
 * Signed subtraction: X = A - B
 */
int impl_mpi_sub_mpi(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *B)
{
    int ret, s = A->s;

    if (A->s * B->s > 0) {
        if (impl_mpi_cmp_abs(A, B) >= 0) {
            HAL_MPI_CHK(impl_mpi_sub_abs(X, A, B));
            X->s = s;
        } else {
            HAL_MPI_CHK(impl_mpi_sub_abs(X, B, A));
            X->s = -s;
        }
    } else {
        HAL_MPI_CHK(impl_mpi_add_abs(X, A, B));
        X->s = s;
    }

cleanup:

    return (ret);
}

/*
 * Signed addition: X = A + b
 */
int impl_mpi_add_int(impl_mpi *X, const impl_mpi *A,
                        impl_mpi_sint b)
{
    impl_mpi      _B;
    impl_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (impl_mpi_add_mpi(X, A, &_B));
}

/*
 * Signed subtraction: X = A - b
 */
int impl_mpi_sub_int(impl_mpi *X, const impl_mpi *A,
                        impl_mpi_sint b)
{
    impl_mpi      _B;
    impl_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (impl_mpi_sub_mpi(X, A, &_B));
}

/*
 * Helper for impl_mpi multiplication
 */
static void mpi_mul_hlp(size_t i, impl_mpi_uint *s, impl_mpi_uint *d,
              impl_mpi_uint b)
{
    impl_mpi_uint c = 0, t = 0;

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
int impl_mpi_mul_mpi(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *B)
{
    int         ret;
    size_t      i, j;
    impl_mpi TA, TB;

    impl_mpi_init(&TA);
    impl_mpi_init(&TB);

    if (X == A) {
        HAL_MPI_CHK(impl_mpi_copy(&TA, A));
        A = &TA;
    }
    if (X == B) {
        HAL_MPI_CHK(impl_mpi_copy(&TB, B));
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

    HAL_MPI_CHK(impl_mpi_grow(X, i + j));
    HAL_MPI_CHK(impl_mpi_lset(X, 0));

    for (i++; j > 0; j--) {
        mpi_mul_hlp(i - 1, A->p, X->p + j - 1, B->p[j - 1]);
    }

    X->s = A->s * B->s;

cleanup:

    impl_mpi_free(&TB);
    impl_mpi_free(&TA);

    return (ret);
}

/*
 * Baseline multiplication: X = A * b
 */
int impl_mpi_mul_int(impl_mpi *X, const impl_mpi *A,
                        impl_mpi_uint b)
{
    impl_mpi      _B;
    impl_mpi_uint p[1];

    _B.s = 1;
    _B.n = 1;
    _B.p = p;
    p[0] = b;

    return (impl_mpi_mul_mpi(X, A, &_B));
}

/*
 * Unsigned integer divide - double impl_mpi_uint dividend, u1/u0, and
 * impl_mpi_uint divisor, d
 */
static impl_mpi_uint impl_int_div_int(impl_mpi_uint  u1,
                                            impl_mpi_uint  u0,
                                            impl_mpi_uint  d,
                                            impl_mpi_uint *r)
{
#if defined(HAL_HAVE_UDBL)
    impl_t_udbl dividend, quotient;
#else
    const impl_mpi_uint radix = (impl_mpi_uint)1 << biH;
    const impl_mpi_uint uint_implfword_mask =
      ((impl_mpi_uint)1 << biH) - 1;
    impl_mpi_uint d0, d1, q0, q1, rAX, r0, quotient;
    impl_mpi_uint u0_msw, u0_lsw;
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

#if defined(HAL_HAVE_UDBL)
    dividend = (impl_t_udbl)u1 << biL;
    dividend |= (impl_t_udbl)u0;
    quotient = dividend / d;
    if (quotient > ((impl_t_udbl)1 << biL) - 1) {
        quotient = ((impl_t_udbl)1 << biL) - 1;
    }

    if (r != NULL) {
        *r = (impl_mpi_uint)(dividend - (quotient * d));
    }

    return (impl_mpi_uint)quotient;
#else

    /*
     * Algorithm D, Section 4.3.1 - The Art of Computer Programming
     *   Vol. 2 - Seminumerical Algorithms, Knuth
     */

    /*
     * Normalize the divisor, d, and dividend, u0, u1
     */
    s = impl_clz(d);
    d = d << s;

    u1 = u1 << s;
    u1 |= (u0 >> (biL - s)) & (-(impl_mpi_sint)s >> (biL - 1));
    u0 = u0 << s;

    d1 = d >> biH;
    d0 = d & uint_implfword_mask;

    u0_msw = u0 >> biH;
    u0_lsw = u0 & uint_implfword_mask;

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
 * Division by impl_mpi: A = Q * B + R  (HAC 14.20)
 */
int impl_mpi_div_mpi(impl_mpi *Q, impl_mpi *R, const impl_mpi *A,
                        const impl_mpi *B)
{
    int         ret;
    size_t      i, n, t, k;
    impl_mpi X, Y, Z, T1, T2;

    if (impl_mpi_cmp_int(B, 0) == 0) {
        return (HAL_ERR_MPI_DIVISION_BY_ZERO);
    }

    impl_mpi_init(&X);
    impl_mpi_init(&Y);
    impl_mpi_init(&Z);
    impl_mpi_init(&T1);
    impl_mpi_init(&T2);

    if (impl_mpi_cmp_abs(A, B) < 0) {
        if (Q != NULL) {
            HAL_MPI_CHK(impl_mpi_lset(Q, 0));
        }
        if (R != NULL) {
            HAL_MPI_CHK(impl_mpi_copy(R, A));
        }
        return (0);
    }

    HAL_MPI_CHK(impl_mpi_copy(&X, A));
    HAL_MPI_CHK(impl_mpi_copy(&Y, B));
    X.s = Y.s = 1;

    HAL_MPI_CHK(impl_mpi_grow(&Z, A->n + 2));
    HAL_MPI_CHK(impl_mpi_lset(&Z, 0));
    HAL_MPI_CHK(impl_mpi_grow(&T1, 2));
    HAL_MPI_CHK(impl_mpi_grow(&T2, 3));

    k = impl_mpi_bitlen(&Y) % biL;
    if (k < biL - 1) {
        k = biL - 1 - k;
        HAL_MPI_CHK(impl_mpi_shift_l(&X, k));
        HAL_MPI_CHK(impl_mpi_shift_l(&Y, k));
    } else {
        k = 0;
    }

    n = X.n - 1;
    t = Y.n - 1;
    HAL_MPI_CHK(impl_mpi_shift_l(&Y, biL * (n - t)));

    while (impl_mpi_cmp_mpi(&X, &Y) >= 0) {
        Z.p[n - t]++;
        HAL_MPI_CHK(impl_mpi_sub_mpi(&X, &X, &Y));
    }
    HAL_MPI_CHK(impl_mpi_shift_r(&Y, biL * (n - t)));

    for (i = n; i > t; i--) {
        if (X.p[i] >= Y.p[t]) {
            Z.p[i - t - 1] = ~0;
        } else {
            Z.p[i - t - 1] =
              impl_int_div_int(X.p[i], X.p[i - 1], Y.p[t], NULL);
        }

        Z.p[i - t - 1]++;
        do {
            Z.p[i - t - 1]--;

            HAL_MPI_CHK(impl_mpi_lset(&T1, 0));
            T1.p[0] = (t < 1) ? 0 : Y.p[t - 1];
            T1.p[1] = Y.p[t];
            HAL_MPI_CHK(impl_mpi_mul_int(&T1, &T1, Z.p[i - t - 1]));

            HAL_MPI_CHK(impl_mpi_lset(&T2, 0));
            T2.p[0] = (i < 2) ? 0 : X.p[i - 2];
            T2.p[1] = (i < 1) ? 0 : X.p[i - 1];
            T2.p[2] = X.p[i];
        } while (impl_mpi_cmp_mpi(&T1, &T2) > 0);

        HAL_MPI_CHK(impl_mpi_mul_int(&T1, &Y, Z.p[i - t - 1]));
        HAL_MPI_CHK(impl_mpi_shift_l(&T1, biL * (i - t - 1)));
        HAL_MPI_CHK(impl_mpi_sub_mpi(&X, &X, &T1));

        if (impl_mpi_cmp_int(&X, 0) < 0) {
            HAL_MPI_CHK(impl_mpi_copy(&T1, &Y));
            HAL_MPI_CHK(impl_mpi_shift_l(&T1, biL * (i - t - 1)));
            HAL_MPI_CHK(impl_mpi_add_mpi(&X, &X, &T1));
            Z.p[i - t - 1]--;
        }
    }

    if (Q != NULL) {
        HAL_MPI_CHK(impl_mpi_copy(Q, &Z));
        Q->s = A->s * B->s;
    }

    if (R != NULL) {
        HAL_MPI_CHK(impl_mpi_shift_r(&X, k));
        X.s = A->s;
        HAL_MPI_CHK(impl_mpi_copy(R, &X));

        if (impl_mpi_cmp_int(R, 0) == 0) {
            R->s = 1;
        }
    }

cleanup:

    impl_mpi_free(&X);
    impl_mpi_free(&Y);
    impl_mpi_free(&Z);
    impl_mpi_free(&T1);
    impl_mpi_free(&T2);

    return (ret);
}

/*
 * Division by int: A = Q * b + R
 */
int impl_mpi_div_int(impl_mpi *Q, impl_mpi *R, const impl_mpi *A,
                        impl_mpi_sint b)
{
    impl_mpi      _B;
    impl_mpi_uint p[1];

    p[0] = (b < 0) ? -b : b;
    _B.s = (b < 0) ? -1 : 1;
    _B.n = 1;
    _B.p = p;

    return (impl_mpi_div_mpi(Q, R, A, &_B));
}

/*
 * Modulo: R = A mod B
 */
int impl_mpi_mod_mpi(impl_mpi *R, const impl_mpi *A,
                        const impl_mpi *B)
{
    int ret;

    if (impl_mpi_cmp_int(B, 0) < 0) {
        return (HAL_ERR_MPI_NEGATIVE_VALUE);
    }

    HAL_MPI_CHK(impl_mpi_div_mpi(NULL, R, A, B));

    while (impl_mpi_cmp_int(R, 0) < 0) {
        HAL_MPI_CHK(impl_mpi_add_mpi(R, R, B));
    }

    while (impl_mpi_cmp_mpi(R, B) >= 0) {
        HAL_MPI_CHK(impl_mpi_sub_mpi(R, R, B));
    }

cleanup:

    return (ret);
}

/*
 * Modulo: r = A mod b
 */
int impl_mpi_mod_int(impl_mpi_uint *r, const impl_mpi *A,
                        impl_mpi_sint b)
{
    size_t           i;
    impl_mpi_uint x, y, z;

    if (b == 0) {
        return (HAL_ERR_MPI_DIVISION_BY_ZERO);
    }

    if (b < 0) {
        return (HAL_ERR_MPI_NEGATIVE_VALUE);
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
static void mpi_montg_init(impl_mpi_uint *mm, const impl_mpi *N)
{
    impl_mpi_uint x, m0 = N->p[0];
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
static int mpi_montmul(impl_mpi *A, const impl_mpi *B,
                       const impl_mpi *N, impl_mpi_uint mm,
                       const impl_mpi *T)
{
    size_t           i, n, m;
    impl_mpi_uint u0, u1, *d;

    if (T->n < N->n + 1 || T->p == NULL) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    memset(T->p, 0, T->n * ciL);

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

    memcpy(A->p, d, (n + 1) * ciL);

    if (impl_mpi_cmp_abs(A, N) >= 0) {
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
static int mpi_montred(impl_mpi *A, const impl_mpi *N,
                       impl_mpi_uint mm, const impl_mpi *T)
{
    impl_mpi_uint z = 1;
    impl_mpi      U;

    U.n = U.s = (int)z;
    U.p       = &z;

    return (mpi_montmul(A, &U, N, mm, T));
}

/*
 * Sliding-window exponentiation: X = A^E mod N  (HAC 14.85)
 */
int impl_mpi_exp_mod(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *E, const impl_mpi *N,
                        impl_mpi *_RR)
{
    int              ret;
    size_t           wbits, wsize, one = 1;
    size_t           i, j, nblimbs;
    size_t           bufsize, nbits;
    impl_mpi_uint ei, mm, state;
    impl_mpi      RR, T, W[2 << HAL_MPI_WINDOW_SIZE], Apos;
    int              neg;

    if (impl_mpi_cmp_int(N, 0) < 0 || (N->p[0] & 1) == 0) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    if (impl_mpi_cmp_int(E, 0) < 0) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    /*
     * Init temps and window size
     */
    mpi_montg_init(&mm, N);
    impl_mpi_init(&RR);
    impl_mpi_init(&T);
    impl_mpi_init(&Apos);
    memset(W, 0, sizeof(W));

    i = impl_mpi_bitlen(E);

    wsize = (i > 671) ? 6 : (i > 239) ? 5 : (i > 79) ? 4 : (i > 23) ? 3 : 1;

#if( HAL_MPI_WINDOW_SIZE < 6 )
    if (wsize > HAL_MPI_WINDOW_SIZE) {
        wsize = HAL_MPI_WINDOW_SIZE;
    }
#endif

    j = N->n + 1;
    HAL_MPI_CHK(impl_mpi_grow(X, j));
    HAL_MPI_CHK(impl_mpi_grow(&W[1], j));
    HAL_MPI_CHK(impl_mpi_grow(&T, j * 2));

    /*
     * Compensate for negative A (and correct at the end)
     */
    neg = (A->s == -1);
    if (neg) {
        HAL_MPI_CHK(impl_mpi_copy(&Apos, A));
        Apos.s = 1;
        A      = &Apos;
    }

    /*
     * If 1st call, pre-compute R^2 mod N
     */
    if (_RR == NULL || _RR->p == NULL) {
        HAL_MPI_CHK(impl_mpi_lset(&RR, 1));
        HAL_MPI_CHK(impl_mpi_shift_l(&RR, N->n * 2 * biL));
        HAL_MPI_CHK(impl_mpi_mod_mpi(&RR, &RR, N));

        if (_RR != NULL) {
            memcpy(_RR, &RR, sizeof(impl_mpi));
        }
    } else {
        memcpy(&RR, _RR, sizeof(impl_mpi));
    }

    /*
     * W[1] = A * R^2 * R^-1 mod N = A * R mod N
     */
    if (impl_mpi_cmp_mpi(A, N) >= 0) {
        HAL_MPI_CHK(impl_mpi_mod_mpi(&W[1], A, N));
    } else {
        HAL_MPI_CHK(impl_mpi_copy(&W[1], A));
    }

    HAL_MPI_CHK(mpi_montmul(&W[1], &RR, N, mm, &T));

    /*
     * X = R^2 * R^-1 mod N = R mod N
     */
    HAL_MPI_CHK(impl_mpi_copy(X, &RR));
    HAL_MPI_CHK(mpi_montred(X, N, mm, &T));

    if (wsize > 1) {
        /*
         * W[1 << (wsize - 1)] = W[1] ^ (wsize - 1)
         */
        j = one << (wsize - 1);

        HAL_MPI_CHK(impl_mpi_grow(&W[j], N->n + 1));
        HAL_MPI_CHK(impl_mpi_copy(&W[j], &W[1]));

        for (i = 0; i < wsize - 1; i++) {
            HAL_MPI_CHK(mpi_montmul(&W[j], &W[j], N, mm, &T));
        }

        /*
         * W[i] = W[i - 1] * W[1]
         */
        for (i = j + 1; i < (one << wsize); i++) {
            HAL_MPI_CHK(impl_mpi_grow(&W[i], N->n + 1));
            HAL_MPI_CHK(impl_mpi_copy(&W[i], &W[i - 1]));

            HAL_MPI_CHK(mpi_montmul(&W[i], &W[1], N, mm, &T));
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

            bufsize = sizeof(impl_mpi_uint) << 3;
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
            HAL_MPI_CHK(mpi_montmul(X, X, N, mm, &T));
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
                HAL_MPI_CHK(mpi_montmul(X, X, N, mm, &T));
            }

            /*
             * X = X * W[wbits] R^-1 mod N
             */
            HAL_MPI_CHK(mpi_montmul(X, &W[wbits], N, mm, &T));

            state--;
            nbits = 0;
            wbits = 0;
        }
    }

    /*
     * process the remaining bits
     */
    for (i = 0; i < nbits; i++) {
        HAL_MPI_CHK(mpi_montmul(X, X, N, mm, &T));

        wbits <<= 1;

        if ((wbits & (one << wsize)) != 0) {
            HAL_MPI_CHK(mpi_montmul(X, &W[1], N, mm, &T));
        }
    }

    /*
     * X = A^E * R * R^-1 mod N = A^E mod N
     */
    HAL_MPI_CHK(mpi_montred(X, N, mm, &T));

    if (neg) {
        X->s = -1;
        HAL_MPI_CHK(impl_mpi_add_mpi(X, N, X));
    }

cleanup:

    for (i = (one << (wsize - 1)); i < (one << wsize); i++) {
        impl_mpi_free(&W[i]);
    }

    impl_mpi_free(&W[1]);
    impl_mpi_free(&T);
    impl_mpi_free(&Apos);

    if (_RR == NULL || _RR->p == NULL) {
        impl_mpi_free(&RR);
    }

    return (ret);
}

/*
 * Greatest common divisor: G = gcd(A, B)  (HAC 14.54)
 */
int impl_mpi_gcd(impl_mpi *G, const impl_mpi *A, const impl_mpi *B)
{
    int         ret;
    size_t      lz, lzt;
    impl_mpi TG, TA, TB;

    impl_mpi_init(&TG);
    impl_mpi_init(&TA);
    impl_mpi_init(&TB);

    HAL_MPI_CHK(impl_mpi_copy(&TA, A));
    HAL_MPI_CHK(impl_mpi_copy(&TB, B));

    lz  = impl_mpi_lsb(&TA);
    lzt = impl_mpi_lsb(&TB);

    if (lzt < lz) {
        lz = lzt;
    }

    HAL_MPI_CHK(impl_mpi_shift_r(&TA, lz));
    HAL_MPI_CHK(impl_mpi_shift_r(&TB, lz));

    TA.s = TB.s = 1;

    while (impl_mpi_cmp_int(&TA, 0) != 0) {
        HAL_MPI_CHK(impl_mpi_shift_r(&TA, impl_mpi_lsb(&TA)));
        HAL_MPI_CHK(impl_mpi_shift_r(&TB, impl_mpi_lsb(&TB)));

        if (impl_mpi_cmp_mpi(&TA, &TB) >= 0) {
            HAL_MPI_CHK(impl_mpi_sub_abs(&TA, &TA, &TB));
            HAL_MPI_CHK(impl_mpi_shift_r(&TA, 1));
        } else {
            HAL_MPI_CHK(impl_mpi_sub_abs(&TB, &TB, &TA));
            HAL_MPI_CHK(impl_mpi_shift_r(&TB, 1));
        }
    }

    HAL_MPI_CHK(impl_mpi_shift_l(&TB, lz));
    HAL_MPI_CHK(impl_mpi_copy(G, &TB));

cleanup:

    impl_mpi_free(&TG);
    impl_mpi_free(&TA);
    impl_mpi_free(&TB);

    return (ret);
}

/*
 * Fill X with size bytes of random.
 *
 * Use a temporary bytes representation to make sure the result is the same
 * regardless of the platform endianness (useful when f_rng is actually
 * deterministic, eg for tests).
 */
int impl_mpi_fill_random(impl_mpi *X, size_t size,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int           ret;
    unsigned char buf[HAL_MPI_MAX_SIZE];

    if (size > HAL_MPI_MAX_SIZE) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    HAL_MPI_CHK(f_rng(p_rng, buf, size));
    HAL_MPI_CHK(impl_mpi_read_binary(X, buf, size));

cleanup:
    return (ret);
}

/*
 * Modular inverse: X = A^-1 mod N  (HAC 14.61 / 14.64)
 */
int impl_mpi_inv_mod(impl_mpi *X, const impl_mpi *A,
                        const impl_mpi *N)
{
    int         ret;
    impl_mpi G, TA, TU, U1, U2, TB, TV, V1, V2;

    if (impl_mpi_cmp_int(N, 0) <= 0) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    impl_mpi_init(&TA);
    impl_mpi_init(&TU);
    impl_mpi_init(&U1);
    impl_mpi_init(&U2);
    impl_mpi_init(&G);
    impl_mpi_init(&TB);
    impl_mpi_init(&TV);
    impl_mpi_init(&V1);
    impl_mpi_init(&V2);

    HAL_MPI_CHK(impl_mpi_gcd(&G, A, N));

    if (impl_mpi_cmp_int(&G, 1) != 0) {
        ret = HAL_ERR_MPI_NOT_ACCEPTABLE;
        goto cleanup;
    }

    HAL_MPI_CHK(impl_mpi_mod_mpi(&TA, A, N));
    HAL_MPI_CHK(impl_mpi_copy(&TU, &TA));
    HAL_MPI_CHK(impl_mpi_copy(&TB, N));
    HAL_MPI_CHK(impl_mpi_copy(&TV, N));

    HAL_MPI_CHK(impl_mpi_lset(&U1, 1));
    HAL_MPI_CHK(impl_mpi_lset(&U2, 0));
    HAL_MPI_CHK(impl_mpi_lset(&V1, 0));
    HAL_MPI_CHK(impl_mpi_lset(&V2, 1));

    do {
        while ((TU.p[0] & 1) == 0) {
            HAL_MPI_CHK(impl_mpi_shift_r(&TU, 1));

            if ((U1.p[0] & 1) != 0 || (U2.p[0] & 1) != 0) {
                HAL_MPI_CHK(impl_mpi_add_mpi(&U1, &U1, &TB));
                HAL_MPI_CHK(impl_mpi_sub_mpi(&U2, &U2, &TA));
            }

            HAL_MPI_CHK(impl_mpi_shift_r(&U1, 1));
            HAL_MPI_CHK(impl_mpi_shift_r(&U2, 1));
        }

        while ((TV.p[0] & 1) == 0) {
            HAL_MPI_CHK(impl_mpi_shift_r(&TV, 1));

            if ((V1.p[0] & 1) != 0 || (V2.p[0] & 1) != 0) {
                HAL_MPI_CHK(impl_mpi_add_mpi(&V1, &V1, &TB));
                HAL_MPI_CHK(impl_mpi_sub_mpi(&V2, &V2, &TA));
            }

            HAL_MPI_CHK(impl_mpi_shift_r(&V1, 1));
            HAL_MPI_CHK(impl_mpi_shift_r(&V2, 1));
        }

        if (impl_mpi_cmp_mpi(&TU, &TV) >= 0) {
            HAL_MPI_CHK(impl_mpi_sub_mpi(&TU, &TU, &TV));
            HAL_MPI_CHK(impl_mpi_sub_mpi(&U1, &U1, &V1));
            HAL_MPI_CHK(impl_mpi_sub_mpi(&U2, &U2, &V2));
        } else {
            HAL_MPI_CHK(impl_mpi_sub_mpi(&TV, &TV, &TU));
            HAL_MPI_CHK(impl_mpi_sub_mpi(&V1, &V1, &U1));
            HAL_MPI_CHK(impl_mpi_sub_mpi(&V2, &V2, &U2));
        }
    } while (impl_mpi_cmp_int(&TU, 0) != 0);

    while (impl_mpi_cmp_int(&V1, 0) < 0) {
        HAL_MPI_CHK(impl_mpi_add_mpi(&V1, &V1, N));
    }

    while (impl_mpi_cmp_mpi(&V1, N) >= 0) {
        HAL_MPI_CHK(impl_mpi_sub_mpi(&V1, &V1, N));
    }

    HAL_MPI_CHK(impl_mpi_copy(X, &V1));

cleanup:

    impl_mpi_free(&TA);
    impl_mpi_free(&TU);
    impl_mpi_free(&U1);
    impl_mpi_free(&U2);
    impl_mpi_free(&G);
    impl_mpi_free(&TB);
    impl_mpi_free(&TV);
    impl_mpi_free(&V1);
    impl_mpi_free(&V2);

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
 * HAL_ERR_MPI_NOT_ACCEPTABLE: certain non-prime
 * other negative: error
 */
static int mpi_check_small_factors(const impl_mpi *X)
{
    int              ret = 0;
    size_t           i;
    impl_mpi_uint r;

    if ((X->p[0] & 1) == 0) {
        return (HAL_ERR_MPI_NOT_ACCEPTABLE);
    }

    for (i = 0; small_prime[i] > 0; i++) {
        if (impl_mpi_cmp_int(X, small_prime[i]) <= 0) {
            return (1);
        }

        HAL_MPI_CHK(impl_mpi_mod_int(&r, X, small_prime[i]));

        if (r == 0) {
            return (HAL_ERR_MPI_NOT_ACCEPTABLE);
        }
    }

cleanup:
    return (ret);
}

/*
 * Miller-Rabin pseudo-primality test  (HAC 4.24)
 */
static int mpi_miller_rabin(const impl_mpi *X,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int         ret, count;
    size_t      i, j, k, n, s;
    impl_mpi W, R, T, A, RR;

    impl_mpi_init(&W);
    impl_mpi_init(&R);
    impl_mpi_init(&T);
    impl_mpi_init(&A);
    impl_mpi_init(&RR);

    /*
     * W = |X| - 1
     * R = W >> lsb( W )
     */
    HAL_MPI_CHK(impl_mpi_sub_int(&W, X, 1));
    s = impl_mpi_lsb(&W);
    HAL_MPI_CHK(impl_mpi_copy(&R, &W));
    HAL_MPI_CHK(impl_mpi_shift_r(&R, s));

    i = impl_mpi_bitlen(X);
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
        HAL_MPI_CHK(impl_mpi_fill_random(&A, X->n * ciL, f_rng, p_rng));

        if (impl_mpi_cmp_mpi(&A, &W) >= 0) {
            j = impl_mpi_bitlen(&A) - impl_mpi_bitlen(&W);
            HAL_MPI_CHK(impl_mpi_shift_r(&A, j + 1));
        }
        A.p[0] |= 3;

        count = 0;
        do {
            HAL_MPI_CHK(
              impl_mpi_fill_random(&A, X->n * ciL, f_rng, p_rng));

            j = impl_mpi_bitlen(&A);
            k = impl_mpi_bitlen(&W);
            if (j > k) {
                HAL_MPI_CHK(impl_mpi_shift_r(&A, j - k));
            }

            if (count++ > 30) {
                ret = HAL_ERR_MPI_NOT_ACCEPTABLE;
                goto cleanup;
            }

        } while (impl_mpi_cmp_mpi(&A, &W) >= 0 ||
                 impl_mpi_cmp_int(&A, 1) <= 0);

        /*
         * A = A^R mod |X|
         */
        HAL_MPI_CHK(impl_mpi_exp_mod(&A, &A, &R, X, &RR));

        if (impl_mpi_cmp_mpi(&A, &W) == 0 ||
            impl_mpi_cmp_int(&A, 1) == 0) {
            continue;
        }

        j = 1;
        while (j < s && impl_mpi_cmp_mpi(&A, &W) != 0) {
            /*
             * A = A * A mod |X|
             */
            HAL_MPI_CHK(impl_mpi_mul_mpi(&T, &A, &A));
            HAL_MPI_CHK(impl_mpi_mod_mpi(&A, &T, X));

            if (impl_mpi_cmp_int(&A, 1) == 0) {
                break;
            }

            j++;
        }

        /*
         * not prime if A != |X| - 1 or A == 1
         */
        if (impl_mpi_cmp_mpi(&A, &W) != 0 ||
            impl_mpi_cmp_int(&A, 1) == 0) {
            ret = HAL_ERR_MPI_NOT_ACCEPTABLE;
            break;
        }
    }

cleanup:
    impl_mpi_free(&W);
    impl_mpi_free(&R);
    impl_mpi_free(&T);
    impl_mpi_free(&A);
    impl_mpi_free(&RR);

    return (ret);
}

/*
 * Pseudo-primality test: small factors, then Miller-Rabin
 */
int impl_mpi_is_prime(const impl_mpi *X,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng)
{
    int         ret;
    impl_mpi XX;


    XX.s = 1;
    XX.n = X->n;
    XX.p = X->p;

    if (impl_mpi_cmp_int(&XX, 0) == 0 || impl_mpi_cmp_int(&XX, 1) == 0) {
        return (HAL_ERR_MPI_NOT_ACCEPTABLE);
    }

    if (impl_mpi_cmp_int(&XX, 2) == 0) {
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
int impl_mpi_gen_prime(impl_mpi *X, size_t nbits, int dh_flag,
                          int (*f_rng)(void *, unsigned char *, size_t),
                          void *p_rng)
{
    int              ret;
    size_t           k, n;
    impl_mpi_uint r;
    impl_mpi      Y;

    if (nbits < 3 || nbits > HAL_MPI_MAX_BITS) {
        return (HAL_ERR_MPI_BAD_INPUT_DATA);
    }

    impl_mpi_init(&Y);

    n = BITS_TO_LIMBS(nbits);

    HAL_MPI_CHK(impl_mpi_fill_random(X, n * ciL, f_rng, p_rng));

    k = impl_mpi_bitlen(X);
    if (k > nbits) {
        HAL_MPI_CHK(impl_mpi_shift_r(X, k - nbits + 1));
    }

    impl_mpi_set_bit(X, nbits - 1, 1);

    X->p[0] |= 1;
    if (dh_flag == 0) {
        while ((ret = impl_mpi_is_prime(X, f_rng, p_rng)) != 0) {
            if (ret != HAL_ERR_MPI_NOT_ACCEPTABLE) {
                goto cleanup;
            }

            HAL_MPI_CHK(impl_mpi_add_int(X, X, 2));
        }
    } else {
        /*
         * An necessary condition for Y and X = 2Y + 1 to be prime
         * is X = 2 mod 3 (which is equivalent to Y = 2 mod 3).
         * Make sure it is satisfied, while keeping X = 3 mod 4
         */
        X->p[0] |= 2;

        HAL_MPI_CHK(impl_mpi_mod_int(&r, X, 3));
        if (r == 0) {
            HAL_MPI_CHK(impl_mpi_add_int(X, X, 8));
        } else if (r == 1) {
            HAL_MPI_CHK(impl_mpi_add_int(X, X, 4));
        }

        /* Set Y = (X-1) / 2, which is X / 2 because X is odd */
        HAL_MPI_CHK(impl_mpi_copy(&Y, X));
        HAL_MPI_CHK(impl_mpi_shift_r(&Y, 1));
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

            if (ret != HAL_ERR_MPI_NOT_ACCEPTABLE) {
                goto cleanup;
            }

            /*
             * Next candidates. We want to preserve Y = (X-1) / 2 and
             * Y = 1 mod 2 and Y = 2 mod 3 (eq X = 3 mod 4 and X = 2 mod 3)
             * so up Y by 6 and X by 12.
             */
            HAL_MPI_CHK(impl_mpi_add_int(X, X, 12));
            HAL_MPI_CHK(impl_mpi_add_int(&Y, &Y, 6));
        }
    }

cleanup:

    impl_mpi_free(&Y);

    return (ret);
}

#endif /* ALI_ALGO_GENPRIME */
