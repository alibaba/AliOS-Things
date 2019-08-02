/**
* Copyright (C) 2017  Alibaba Group Holding Limited.
**/

#ifndef __BIGNUM_H
#define __BIGNUM_H
#include <stddef.h>
#include <stdint.h>


#define HAL_ERR_MPI_FILE_IO_ERROR                     -0x0002  /**< An error occurred while reading from or writing to a file. */
#define HAL_ERR_MPI_BAD_INPUT_DATA                    -0x0004  /**< Bad input parameters to function. */
#define HAL_ERR_MPI_INVALID_CHARACTER                 -0x0006  /**< There is an invalid character in the digit string. */
#define HAL_ERR_MPI_BUFFER_TOO_SMALL                  -0x0008  /**< The buffer is too small to write to. */
#define HAL_ERR_MPI_NEGATIVE_VALUE                    -0x000A  /**< The input arguments are negative or result in illegal output. */
#define HAL_ERR_MPI_DIVISION_BY_ZERO                  -0x000C  /**< The input argument for division is zero, which is not allowed. */
#define HAL_ERR_MPI_NOT_ACCEPTABLE                    -0x000E  /**< The input arguments are not acceptable. */
#define HAL_ERR_MPI_ALLOC_FAILED                      -0x0010  /**< Memory allocation failed. */

#define HAL_MPI_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )

/*
 * Maximum size MPIs are allowed to grow to in number of limbs.
 */
#define HAL_MPI_MAX_LIMBS                             10000

#if !defined(HAL_MPI_WINDOW_SIZE)
/*
 * Maximum window size used for modular exponentiation. Default: 6
 * Minimum value: 1. Maximum value: 6.
 *
 * Result is an array of ( 2 << HAL_MPI_WINDOW_SIZE ) MPIs used
 * for the sliding window calculation. (So 64 by default)
 *
 * Reduction in size, reduces speed.
 */
#define HAL_MPI_WINDOW_SIZE                           6        /**< Maximum windows size used. */
#endif /* !HAL_MPI_WINDOW_SIZE */

#if !defined(HAL_MPI_MAX_SIZE)
/*
 * Maximum size of MPIs allowed in bits and bytes for user-MPIs.
 * ( Default: 512 bytes => 4096 bits, Maximum tested: 2048 bytes => 16384 bits )
 *
 * Note: Calculations can results temporarily in larger MPIs. So the number
 * of limbs required (HAL_MPI_MAX_LIMBS) is higher.
 */
#define HAL_MPI_MAX_SIZE                              1024     /**< Maximum number of bytes for usable MPIs. */
#endif /* !HAL_MPI_MAX_SIZE */

#define HAL_MPI_MAX_BITS                              ( 8 * HAL_MPI_MAX_SIZE )    /**< Maximum number of bits for usable MPIs. */

/*
 * When reading from files with impl_mpi_read_file() and writing to files with
 * impl_mpi_write_file() the buffer should have space
 * for a (short) label, the MPI (in the provided radix), the newline
 * characters and the '\0'.
 *
 * By default we assume at least a 10 char label, a minimum radix of 10
 * (decimal) and a maximum of 4096 bit numbers (1234 decimal chars).
 * Autosized at compile time for at least a 10 char label, a minimum radix
 * of 10 (decimal) for a number of HAL_MPI_MAX_BITS size.
 *
 * This used to be statically sized to 1250 for a maximum of 4096 bit
 * numbers (1234 decimal chars).
 *
 * Calculate using the formula:
 *  HAL_MPI_RW_BUFFER_SIZE = ceil(HAL_MPI_MAX_BITS / ln(10) * ln(2)) +
 *                                LabelSize + 6
 */
#define HAL_MPI_MAX_BITS_SCALE100          ( 100 * HAL_MPI_MAX_BITS )
#define HAL_LN_2_DIV_LN_10_SCALE100                 332
#define HAL_MPI_RW_BUFFER_SIZE             ( ((HAL_MPI_MAX_BITS_SCALE100 + ALI_ALGO_LN_2_DIV_LN_10_SCALE100 - 1) / ALI_ALGO_LN_2_DIV_LN_10_SCALE100) + 10 + 6 )

/*
 * Define the base integer type, architecture-wise.
 *
 * 32-bit integers can be forced on 64-bit arches (eg. for testing purposes)
 * by defining HAL_HAVE_INT32 and undefining HAL_HAVE_ASM
 */
#if ( ! defined(HAL_HAVE_INT32) && \
        defined(_MSC_VER) && defined(_M_AMD64) )
  #define HAL_HAVE_INT64
  typedef  int64_t impl_mpi_sint;
  typedef uint64_t impl_mpi_uint;
#else
  #if ( ! defined(HAL_HAVE_INT32) &&               \
        defined(__GNUC__) && (                          \
        defined(__amd64__) || defined(__x86_64__)    || \
        defined(__ppc64__) || defined(__powerpc64__) || \
        defined(__ia64__)  || defined(__alpha__)     || \
        (defined(__sparc__) && defined(__arch64__))  || \
        defined(__s390x__) || defined(__mips64) ) )
     #define HAL_HAVE_INT64
     typedef  int64_t impl_mpi_sint;
     typedef uint64_t impl_mpi_uint;
     /* impl_t_udbl defined as 128-bit unsigned int */
     typedef unsigned int impl_t_udbl __attribute__((mode(TI)));
     #define HAL_HAVE_UDBL
  #else
     #define HAL_HAVE_INT32
     typedef  int32_t impl_mpi_sint;
     typedef uint32_t impl_mpi_uint;
     typedef uint64_t impl_t_udbl;
     #define HAL_HAVE_UDBL
  #endif /* !HAL_HAVE_INT32 && __GNUC__ && 64-bit platform */
#endif /* !HAL_HAVE_INT32 && _MSC_VER && _M_AMD64 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MPI structure
 */
typedef struct
{
    int s;              /*!<  integer sign      */
    size_t n;           /*!<  total # of limbs  */
    impl_mpi_uint *p;          /*!<  pointer to limbs  */
}
impl_mpi;

/**
 * \brief           Initialize one MPI (make internal references valid)
 *                  This just makes it ready to be set or freed,
 *                  but does not define a value for the MPI.
 *
 * \param X         One MPI to initialize.
 */
void impl_mpi_init( impl_mpi *X );

/**
 * \brief          Unallocate one MPI
 *
 * \param X        One MPI to unallocate.
 */
void impl_mpi_free( impl_mpi *X );

/**
 * \brief          Enlarge to the specified number of limbs
 *
 * \param X        MPI to grow
 * \param nblimbs  The target number of limbs
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_grow( impl_mpi *X, size_t nblimbs );

/**
 * \brief          Resize down, keeping at least the specified number of limbs
 *
 * \param X        MPI to shrink
 * \param nblimbs  The minimum number of limbs to keep
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_shrink( impl_mpi *X, size_t nblimbs );

/**
 * \brief          Copy the contents of Y into X
 *
 * \param X        Destination MPI
 * \param Y        Source MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_copy( impl_mpi *X, const impl_mpi *Y );

/**
 * \brief          Swap the contents of X and Y
 *
 * \param X        First MPI value
 * \param Y        Second MPI value
 */
void impl_mpi_swap( impl_mpi *X, impl_mpi *Y );

/**
 * \brief          Safe conditional assignement X = Y if assign is 1
 *
 * \param X        MPI to conditionally assign to
 * \param Y        Value to be assigned
 * \param assign   1: perform the assignment, 0: keep X's original value
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *
 * \note           This function is equivalent to
 *                      if( assign ) impl_mpi_copy( X, Y );
 *                 except that it avoids leaking any information about whether
 *                 the assignment was done or not (the above code may leak
 *                 information through branch prediction and/or memory access
 *                 patterns analysis).
 */
int impl_mpi_safe_cond_assign( impl_mpi *X, const impl_mpi *Y, unsigned char assign );

/**
 * \brief          Safe conditional swap X <-> Y if swap is 1
 *
 * \param X        First impl_mpi value
 * \param Y        Second impl_mpi value
 * \param assign   1: perform the swap, 0: keep X and Y's original values
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *
 * \note           This function is equivalent to
 *                      if( assign ) impl_mpi_swap( X, Y );
 *                 except that it avoids leaking any information about whether
 *                 the assignment was done or not (the above code may leak
 *                 information through branch prediction and/or memory access
 *                 patterns analysis).
 */
int impl_mpi_safe_cond_swap( impl_mpi *X, impl_mpi *Y, unsigned char assign );

/**
 * \brief          Set value from integer
 *
 * \param X        MPI to set
 * \param z        Value to use
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_lset( impl_mpi *X, impl_mpi_sint z );

/**
 * \brief          Get a specific bit from X
 *
 * \param X        MPI to use
 * \param pos      Zero-based index of the bit in X
 *
 * \return         Either a 0 or a 1
 */
int impl_mpi_get_bit( const impl_mpi *X, size_t pos );

/**
 * \brief          Set a bit of X to a specific value of 0 or 1
 *
 * \note           Will grow X if necessary to set a bit to 1 in a not yet
 *                 existing limb. Will not grow if bit should be set to 0
 *
 * \param X        MPI to use
 * \param pos      Zero-based index of the bit in X
 * \param val      The value to set the bit to (0 or 1)
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_BAD_INPUT_DATA if val is not 0 or 1
 */
int impl_mpi_set_bit( impl_mpi *X, size_t pos, unsigned char val );

/**
 * \brief          Return the number of zero-bits before the least significant
 *                 '1' bit
 *
 * Note: Thus also the zero-based index of the least significant '1' bit
 *
 * \param X        MPI to use
 */
size_t impl_mpi_lsb( const impl_mpi *X );

/**
 * \brief          Return the number of bits up to and including the most
 *                 significant '1' bit'
 *
 * Note: Thus also the one-based index of the most significant '1' bit
 *
 * \param X        MPI to use
 */
size_t impl_mpi_bitlen( const impl_mpi *X );

/**
 * \brief          Return the total size in bytes
 *
 * \param X        MPI to use
 */
size_t impl_mpi_size( const impl_mpi *X );

/**
 * \brief          Import from an ASCII string
 *
 * \param X        Destination MPI
 * \param radix    Input numeric base
 * \param s        Null-terminated string buffer
 *
 * \return         0 if successful, or a HAL_ERR_MPI_XXX error code
 */
int impl_mpi_read_string( impl_mpi *X, int radix, const char *s );

/**
 * \brief          Export into an ASCII string
 *
 * \param X        Source MPI
 * \param radix    Output numeric base
 * \param buf      Buffer to write the string to
 * \param buflen   Length of buf
 * \param olen     Length of the string written, including final NUL byte
 *
 * \return         0 if successful, or a HAL_ERR_MPI_XXX error code.
 *                 *olen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *
 * \note           Call this function with buflen = 0 to obtain the
 *                 minimum required buffer size in *olen.
 */
int impl_mpi_write_string( const impl_mpi *X, int radix,
                              char *buf, size_t buflen, size_t *olen );

#if defined(HAL_FS_IO)
/**
 * \brief          Read X from an opened file
 *
 * \param X        Destination MPI
 * \param radix    Input numeric base
 * \param fin      Input file handle
 *
 * \return         0 if successful, HAL_ERR_MPI_BUFFER_TOO_SMALL if
 *                 the file read buffer is too small or a
 *                 HAL_ERR_MPI_XXX error code
 */
int impl_mpi_read_file( impl_mpi *X, int radix, FILE *fin );

/**
 * \brief          Write X into an opened file, or stdout if fout is NULL
 *
 * \param p        Prefix, can be NULL
 * \param X        Source MPI
 * \param radix    Output numeric base
 * \param fout     Output file handle (can be NULL)
 *
 * \return         0 if successful, or a HAL_ERR_MPI_XXX error code
 *
 * \note           Set fout == NULL to print X on the console.
 */
int impl_mpi_write_file( const char *p, const impl_mpi *X, int radix, FILE *fout );
#endif /* HAL_FS_IO */

/**
 * \brief          Import X from unsigned binary data, big endian
 *
 * \param X        Destination MPI
 * \param buf      Input buffer
 * \param buflen   Input buffer size
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_read_binary( impl_mpi *X, const unsigned char *buf, size_t buflen );

/**
 * \brief          Export X into unsigned binary data, big endian.
 *                 Always fills the whole buffer, which will start with zeros
 *                 if the number is smaller.
 *
 * \param X        Source MPI
 * \param buf      Output buffer
 * \param buflen   Output buffer size
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_BUFFER_TOO_SMALL if buf isn't large enough
 */
int impl_mpi_write_binary( const impl_mpi *X, unsigned char *buf, size_t buflen );

/**
 * \brief          Left-shift: X <<= count
 *
 * \param X        MPI to shift
 * \param count    Amount to shift
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_shift_l( impl_mpi *X, size_t count );

/**
 * \brief          Right-shift: X >>= count
 *
 * \param X        MPI to shift
 * \param count    Amount to shift
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_shift_r( impl_mpi *X, size_t count );

/**
 * \brief          Compare unsigned values
 *
 * \param X        Left-hand MPI
 * \param Y        Right-hand MPI
 *
 * \return         1 if |X| is greater than |Y|,
 *                -1 if |X| is lesser  than |Y| or
 *                 0 if |X| is equal to |Y|
 */
int impl_mpi_cmp_abs( const impl_mpi *X, const impl_mpi *Y );

/**
 * \brief          Compare signed values
 *
 * \param X        Left-hand MPI
 * \param Y        Right-hand MPI
 *
 * \return         1 if X is greater than Y,
 *                -1 if X is lesser  than Y or
 *                 0 if X is equal to Y
 */
int impl_mpi_cmp_mpi( const impl_mpi *X, const impl_mpi *Y );

/**
 * \brief          Compare signed values
 *
 * \param X        Left-hand MPI
 * \param z        The integer value to compare to
 *
 * \return         1 if X is greater than z,
 *                -1 if X is lesser  than z or
 *                 0 if X is equal to z
 */
int impl_mpi_cmp_int( const impl_mpi *X, impl_mpi_sint z );

/**
 * \brief          Unsigned addition: X = |A| + |B|
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_add_abs( impl_mpi *X, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Unsigned subtraction: X = |A| - |B|
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_NEGATIVE_VALUE if B is greater than A
 */
int impl_mpi_sub_abs( impl_mpi *X, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Signed addition: X = A + B
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_add_mpi( impl_mpi *X, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Signed subtraction: X = A - B
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_sub_mpi( impl_mpi *X, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Signed addition: X = A + b
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param b        The integer value to add
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_add_int( impl_mpi *X, const impl_mpi *A, impl_mpi_sint b );

/**
 * \brief          Signed subtraction: X = A - b
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param b        The integer value to subtract
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_sub_int( impl_mpi *X, const impl_mpi *A, impl_mpi_sint b );

/**
 * \brief          Baseline multiplication: X = A * B
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_mul_mpi( impl_mpi *X, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Baseline multiplication: X = A * b
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param b        The unsigned integer value to multiply with
 *
 * \note           b is unsigned
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_mul_int( impl_mpi *X, const impl_mpi *A, impl_mpi_uint b );

/**
 * \brief          Division by impl_mpi: A = Q * B + R
 *
 * \param Q        Destination MPI for the quotient
 * \param R        Destination MPI for the rest value
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_DIVISION_BY_ZERO if B == 0
 *
 * \note           Either Q or R can be NULL.
 */
int impl_mpi_div_mpi( impl_mpi *Q, impl_mpi *R, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Division by int: A = Q * b + R
 *
 * \param Q        Destination MPI for the quotient
 * \param R        Destination MPI for the rest value
 * \param A        Left-hand MPI
 * \param b        Integer to divide by
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_DIVISION_BY_ZERO if b == 0
 *
 * \note           Either Q or R can be NULL.
 */
int impl_mpi_div_int( impl_mpi *Q, impl_mpi *R, const impl_mpi *A, impl_mpi_sint b );

/**
 * \brief          Modulo: R = A mod B
 *
 * \param R        Destination MPI for the rest value
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_DIVISION_BY_ZERO if B == 0,
 *                 HAL_ERR_MPI_NEGATIVE_VALUE if B < 0
 */
int impl_mpi_mod_mpi( impl_mpi *R, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Modulo: r = A mod b
 *
 * \param r        Destination impl_mpi_uint
 * \param A        Left-hand MPI
 * \param b        Integer to divide by
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_DIVISION_BY_ZERO if b == 0,
 *                 HAL_ERR_MPI_NEGATIVE_VALUE if b < 0
 */
int impl_mpi_mod_int( impl_mpi_uint *r, const impl_mpi *A, impl_mpi_sint b );

/**
 * \brief          Sliding-window exponentiation: X = A^E mod N
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param E        Exponent MPI
 * \param N        Modular MPI
 * \param _RR      Speed-up MPI used for recalculations
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_BAD_INPUT_DATA if N is negative or even or
 *                 if E is negative
 *
 * \note           _RR is used to avoid re-computing R*R mod N across
 *                 multiple calls, which speeds up things a bit. It can
 *                 be set to NULL if the extra performance is unneeded.
 */
int impl_mpi_exp_mod( impl_mpi *X, const impl_mpi *A, const impl_mpi *E, const impl_mpi *N, impl_mpi *_RR );

/**
 * \brief          Fill an MPI X with size bytes of random
 *
 * \param X        Destination MPI
 * \param size     Size in bytes
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_fill_random( impl_mpi *X, size_t size,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

/**
 * \brief          Greatest common divisor: G = gcd(A, B)
 *
 * \param G        Destination MPI
 * \param A        Left-hand MPI
 * \param B        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int impl_mpi_gcd( impl_mpi *G, const impl_mpi *A, const impl_mpi *B );

/**
 * \brief          Modular inverse: X = A^-1 mod N
 *
 * \param X        Destination MPI
 * \param A        Left-hand MPI
 * \param N        Right-hand MPI
 *
 * \return         0 if successful,
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_BAD_INPUT_DATA if N is negative or nil
                   HAL_ERR_MPI_NOT_ACCEPTABLE if A has no inverse mod N
 */
int impl_mpi_inv_mod( impl_mpi *X, const impl_mpi *A, const impl_mpi *N );

/**
 * \brief          Miller-Rabin primality test
 *
 * \param X        MPI to check
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 *
 * \return         0 if successful (probably prime),
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_NOT_ACCEPTABLE if X is not prime
 */
int impl_mpi_is_prime( const impl_mpi *X,
                  int (*f_rng)(void *, unsigned char *, size_t),
                  void *p_rng );

/**
 * \brief          Prime number generation
 *
 * \param X        Destination MPI
 * \param nbits    Required size of X in bits
 *                 ( 3 <= nbits <= HAL_MPI_MAX_BITS )
 * \param dh_flag  If 1, then (X-1)/2 will be prime too
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 *
 * \return         0 if successful (probably prime),
 *                 HAL_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                 HAL_ERR_MPI_BAD_INPUT_DATA if nbits is < 3
 */
int impl_mpi_gen_prime( impl_mpi *X, size_t nbits, int dh_flag,
                   int (*f_rng)(void *, unsigned char *, size_t),
                   void *p_rng );

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int impl_mpi_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* bignum.h */
