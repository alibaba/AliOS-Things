#ifndef _OTA_BIGNUM_H
#define _OTA_BIGNUM_H
#include "ota_pub.h"

#define OTA_ERR_MPI_FILE_IO_ERROR                     -0x0002  /**< An error occurred while reading from or writing to a file. */
#define OTA_ERR_MPI_BAD_INPUT_DATA                    -0x0004  /**< Bad input parameters to function. */
#define OTA_ERR_MPI_INVALID_CHARACTER                 -0x0006  /**< There is an invalid character in the digit string. */
#define OTA_ERR_MPI_BUFFER_TOO_SMALL                  -0x0008  /**< The buffer is too small to write to. */
#define OTA_ERR_MPI_NEGATIVE_VALUE                    -0x000A  /**< The input arguments are negative or result in illegal output. */
#define OTA_ERR_MPI_DIVISION_BY_ZERO                  -0x000C  /**< The input argument for division is zero, which is not allowed. */
#define OTA_ERR_MPI_NOT_ACCEPTABLE                    -0x000E  /**< The input arguments are not acceptable. */
#define OTA_ERR_MPI_ALLOC_FAILED                      -0x0010  /**< Memory allocation failed. */

#define OTA_MPI_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )

#define OTA_MPI_MAX_LIMBS                             10000

#if !defined(OTA_MPI_WINDOW_SIZE)

#define OTA_MPI_WINDOW_SIZE                           6        /**< Maximum windows size used. */
#endif /* !MBEDTLS_MPI_WINDOW_SIZE */

#if !defined(OTA_MPI_MAX_SIZE)

#define OTA_MPI_MAX_SIZE                              1024     /**< Maximum number of bytes for usable MPIs. */
#endif /* !MBEDTLS_MPI_MAX_SIZE */

#define OTA_MPI_MAX_BITS                              ( 8 * OTA_MPI_MAX_SIZE )    /**< Maximum number of bits for usable MPIs. */

#define OTA_MPI_MAX_BITS_SCALE100          ( 100 * OTA_MPI_MAX_BITS )
#define OTA_LN_2_DIV_LN_10_SCALE100                 332
#define OTA_MPI_RW_BUFFER_SIZE             ( ((OTA_MPI_MAX_BITS_SCALE100 + OTA_LN_2_DIV_LN_10_SCALE100 - 1) / OTA_LN_2_DIV_LN_10_SCALE100) + 10 + 6 )

#if ( ! defined(MBEDTLS_HAVE_INT32) && \
        defined(_MSC_VER) && defined(_M_AMD64) )
  #define MBEDTLS_HAVE_INT64
  typedef  int64_t ota_mpi_sint;
  typedef uint64_t ota_mpi_uint;
#else
  #if ( ! defined(MBEDTLS_HAVE_INT32) &&               \
        defined(__GNUC__) && (                          \
        defined(__amd64__) || defined(__x86_64__)    || \
        defined(__ppc64__) || defined(__powerpc64__) || \
        defined(__ia64__)  || defined(__alpha__)     || \
        (defined(__sparc__) && defined(__arch64__))  || \
        defined(__s390x__) || defined(__mips64) ) )
     #define MBEDTLS_HAVE_INT64
     typedef  int64_t ota_mpi_sint;
     typedef uint64_t ota_mpi_uint;
     /* mbedtls_t_udbl defined as 128-bit unsigned int */
     typedef unsigned int ota_t_udbl __attribute__((mode(TI)));
     #define MBEDTLS_HAVE_UDBL
  #else
     #define MBEDTLS_HAVE_INT32
     typedef int32_t ota_mpi_sint;
     typedef uint32_t ota_mpi_uint;
     typedef uint64_t ota_t_udbl;
     #define MBEDTLS_HAVE_UDBL
  #endif /* !MBEDTLS_HAVE_INT32 && __GNUC__ && 64-bit platform */
#endif /* !MBEDTLS_HAVE_INT32 && _MSC_VER && _M_AMD64 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int s;              /*!<  integer sign      */
    size_t n;           /*!<  total # of limbs  */
    ota_mpi_uint *p;          /*!<  pointer to limbs  */
}
ota_mpi;

void ota_mpi_init( ota_mpi *X );

void ota_mpi_free( ota_mpi *X );

int ota_mpi_grow( ota_mpi *X, size_t nblimbs );

int ota_mpi_shrink( ota_mpi *X, size_t nblimbs );

int ota_mpi_copy( ota_mpi *X, const ota_mpi *Y );

void ota_mpi_swap( ota_mpi *X, ota_mpi *Y );

int ota_mpi_safe_cond_assign( ota_mpi *X, const ota_mpi *Y, unsigned char assign );

int ota_mpi_safe_cond_swap( ota_mpi *X, ota_mpi *Y, unsigned char assign );

int ota_mpi_lset( ota_mpi *X, ota_mpi_sint z );

int ota_mpi_get_bit( const ota_mpi *X, size_t pos );

int ota_mpi_set_bit( ota_mpi *X, size_t pos, unsigned char val );

size_t ota_mpi_lsb( const ota_mpi *X );

size_t ota_mpi_bitlen( const ota_mpi *X );

size_t ota_mpi_size( const ota_mpi *X );

int ota_mpi_read_string( ota_mpi *X, int radix, const char *s );

int ota_mpi_write_string( const ota_mpi *X, int radix,
                              char *buf, size_t buflen, size_t *olen );

#if defined(MBEDTLS_FS_IO)

int ota_mpi_read_file( ota_mpi *X, int radix, FILE *fin );

int ota_mpi_write_file( const char *p, const ota_mpi *X, int radix, FILE *fout );
#endif /* MBEDTLS_FS_IO */

int ota_mpi_read_binary( ota_mpi *X, const unsigned char *buf, size_t buflen );

int ota_mpi_write_binary( const ota_mpi *X, unsigned char *buf, size_t buflen );

int ota_mpi_shift_l( ota_mpi *X, size_t count );

int ota_mpi_shift_r( ota_mpi *X, size_t count );

int ota_mpi_cmp_abs( const ota_mpi *X, const ota_mpi *Y );

int ota_mpi_cmp_mpi( const ota_mpi *X, const ota_mpi *Y );

int ota_mpi_cmp_int( const ota_mpi *X, ota_mpi_sint z );

int ota_mpi_add_abs( ota_mpi *X, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_sub_abs( ota_mpi *X, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_add_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_sub_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_add_int( ota_mpi *X, const ota_mpi *A, ota_mpi_sint b );

int ota_mpi_sub_int( ota_mpi *X, const ota_mpi *A, ota_mpi_sint b );

int ota_mpi_mul_mpi( ota_mpi *X, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_mul_int( ota_mpi *X, const ota_mpi *A, ota_mpi_uint b );

int ota_mpi_div_mpi( ota_mpi *Q, ota_mpi *R, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_div_int( ota_mpi *Q, ota_mpi *R, const ota_mpi *A, ota_mpi_sint b );

int ota_mpi_mod_mpi( ota_mpi *R, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_mod_int( ota_mpi_uint *r, const ota_mpi *A, ota_mpi_sint b );

int ota_mpi_exp_mod( ota_mpi *X, const ota_mpi *A, const ota_mpi *E, const ota_mpi *N, ota_mpi *_RR );

int ota_mpi_fill_random( ota_mpi *X, size_t size,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

int ota_mpi_gcd(ota_mpi *G, const ota_mpi *A, const ota_mpi *B );

int ota_mpi_inv_mod( ota_mpi *X, const ota_mpi *A, const ota_mpi *N );

int ota_mpi_is_prime( const ota_mpi *X,
                  int (*f_rng)(void *, unsigned char *, size_t),
                  void *p_rng );

int ota_mpi_gen_prime( ota_mpi *X, size_t nbits, int dh_flag,
                   int (*f_rng)(void *, unsigned char *, size_t),
                   void *p_rng );


int ota_mpi_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif
