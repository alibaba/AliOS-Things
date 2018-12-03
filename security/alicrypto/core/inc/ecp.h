/**
 *  Copyright (C) 2018  Alibaba Group Holding Limited.
 *
 *  Elliptic curves over GF(p)
 *
 */
#ifndef ALI_ALGO_ECP_H
#define ALI_ALGO_ECP_H

#include "bignum.h"

/*
 * ECP error codes
 */
#define ALI_ALGO_ERR_ECP_BAD_INPUT_DATA                    -0x4F80  /**< Bad input parameters to function. */
#define ALI_ALGO_ERR_ECP_BUFFER_TOO_SMALL                  -0x4F00  /**< The buffer is too small to write to. */
#define ALI_ALGO_ERR_ECP_FEATURE_UNAVAILABLE               -0x4E80  /**< Requested curve not available. */
#define ALI_ALGO_ERR_ECP_VERIFY_FAILED                     -0x4E00  /**< The signature is not valid. */
#define ALI_ALGO_ERR_ECP_DECRYPT_FAILED                    -0x4E08  /**< The decryption failed. */
#define ALI_ALGO_ERR_ECP_DH_FAILED                         -0x4E88  /**< The Diffie-Hellman key exchange failed. */
#define ALI_ALGO_ERR_ECP_ALLOC_FAILED                      -0x4D80  /**< Memory allocation failed. */
#define ALI_ALGO_ERR_ECP_RANDOM_FAILED                     -0x4D00  /**< Generation of random value, such as (ephemeral) key, failed. */
#define ALI_ALGO_ERR_ECP_INVALID_KEY                       -0x4C80  /**< Invalid private or public key. */
#define ALI_ALGO_ERR_ECP_SIG_LEN_MISMATCH                  -0x4C00  /**< Signature is valid but shorter than the user-supplied length. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Domain parameters (curve, subgroup and generator) identifiers.
 *
 * Only curves over prime fields are supported.
 *
 * \warning This library does not support validation of arbitrary domain
 * parameters. Therefore, only well-known domain parameters from trusted
 * sources should be used. See ali_algo_ecp_group_load().
 */
typedef enum
{
    ALI_ALGO_ECP_DP_NONE = 0,
    ALI_ALGO_ECP_DP_SECP192R1,      /*!< 192-bits NIST curve  */
    ALI_ALGO_ECP_DP_SECP224R1,      /*!< 224-bits NIST curve  */
    ALI_ALGO_ECP_DP_SECP256R1,      /*!< 256-bits NIST curve  */
    ALI_ALGO_ECP_DP_SECP384R1,      /*!< 384-bits NIST curve  */
    ALI_ALGO_ECP_DP_SECP521R1,      /*!< 521-bits NIST curve  */
    ALI_ALGO_ECP_DP_BP256R1,        /*!< 256-bits Brainpool curve */
    ALI_ALGO_ECP_DP_BP384R1,        /*!< 384-bits Brainpool curve */
    ALI_ALGO_ECP_DP_BP512R1,        /*!< 512-bits Brainpool curve */
    ALI_ALGO_ECP_DP_CURVE25519,           /*!< Curve25519               */
    ALI_ALGO_ECP_DP_SECP192K1,      /*!< 192-bits "Koblitz" curve */
    ALI_ALGO_ECP_DP_SECP224K1,      /*!< 224-bits "Koblitz" curve */
    ALI_ALGO_ECP_DP_SECP256K1,      /*!< 256-bits "Koblitz" curve */

    ALI_ALGO_ECP_DP_SMP256R1,      /*!< 256-bits SM2 curve */
    ALI_ALGO_ECP_DP_SMP256R2,      /*!< 256-bits SM2 test curve */
} ali_algo_ecp_group_id;

/**
 * Number of supported curves (plus one for NONE).
 *
 * (Montgomery curves excluded for now.)
 */
#define ALI_ALGO_ECP_DP_MAX     12

/**
 * Curve information for use by other modules
 */
typedef struct
{
    ali_algo_ecp_group_id grp_id;    /*!< Internal identifier        */
    uint16_t tls_id;                /*!< TLS NamedCurve identifier  */
    uint16_t bit_size;              /*!< Curve size in bits         */
    const char *name;               /*!< Human-friendly name        */
} ali_algo_ecp_curve_info;

/**
 * \brief           ECP point structure (jacobian coordinates)
 *
 * \note            All functions expect and return points satisfying
 *                  the following condition: Z == 0 or Z == 1. (Other
 *                  values of Z are used by internal functions only.)
 *                  The point is zero, or "at infinity", if Z == 0.
 *                  Otherwise, X and Y are its standard (affine) coordinates.
 */
typedef struct
{
    ali_algo_mpi X;          /*!<  the point's X coordinate  */
    ali_algo_mpi Y;          /*!<  the point's Y coordinate  */
    ali_algo_mpi Z;          /*!<  the point's Z coordinate  */
}
ali_algo_ecp_point;

/**
 * \brief           ECP group structure
 *
 * We consider two types of curves equations:
 * 1. Short Weierstrass y^2 = x^3 + A x + B     mod P   (SEC1 + RFC 4492)
 * 2. Montgomery,       y^2 = x^3 + A x^2 + x   mod P   (Curve25519 + draft)
 * In both cases, a generator G for a prime-order subgroup is fixed. In the
 * short weierstrass, this subgroup is actually the whole curve, and its
 * cardinal is denoted by N.
 *
 * In the case of Short Weierstrass curves, our code requires that N is an odd
 * prime. (Use odd in ali_algo_ecp_mul() and prime in ali_algo_ecdsa_sign() for blinding.)
 *
 * In the case of Montgomery curves, we don't store A but (A + 2) / 4 which is
 * the quantity actually used in the formulas. Also, nbits is not the size of N
 * but the required size for private keys.
 *
 * If modp is NULL, reduction modulo P is done using a generic algorithm.
 * Otherwise, it must point to a function that takes an ali_algo_mpi in the range
 * 0..2^(2*pbits)-1 and transforms it in-place in an integer of little more
 * than pbits, so that the integer may be efficiently brought in the 0..P-1
 * range by a few additions or substractions. It must return 0 on success and
 * non-zero on failure.
 */
typedef struct
{
    ali_algo_ecp_group_id id;    /*!<  internal group identifier                     */
    ali_algo_mpi P;              /*!<  prime modulus of the base field               */
    ali_algo_mpi A;              /*!<  1. A in the equation, or 2. (A + 2) / 4       */
    ali_algo_mpi B;              /*!<  1. B in the equation, or 2. unused            */
    ali_algo_ecp_point G;        /*!<  generator of the (sub)group used              */
    ali_algo_mpi N;              /*!<  1. the order of G, or 2. unused               */
    size_t pbits;       /*!<  number of bits in P                           */
    size_t nbits;       /*!<  number of bits in 1. P, or 2. private keys    */
    unsigned int h;     /*!<  internal: 1 if the constants are static       */
    int (*modp)(ali_algo_mpi *); /*!<  function for fast reduction mod P             */
    int (*t_pre)(ali_algo_ecp_point *, void *);  /*!< unused                         */
    int (*t_post)(ali_algo_ecp_point *, void *); /*!< unused                         */
    void *t_data;                       /*!< unused                         */
    ali_algo_ecp_point *T;       /*!<  pre-computed points for ecp_mul_comb()        */
    size_t T_size;      /*!<  number for pre-computed points                */
}
ali_algo_ecp_group;

/**
 * \brief           ECP key pair structure
 *
 * A generic key pair that could be used for ECDSA, fixed ECDH, etc.
 *
 * \note Members purposefully in the same order as struc ali_algo_ecdsa_context.
 */
typedef struct
{
    ali_algo_ecp_group grp;      /*!<  Elliptic curve and base point     */
    ali_algo_mpi d;              /*!<  our secret value                  */
    ali_algo_ecp_point Q;        /*!<  our public value                  */
}
ali_algo_ecp_keypair;

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(ALI_ALGO_ECP_MAX_BITS)
/**
 * Maximum size of the groups (that is, of N and P)
 */
#define ALI_ALGO_ECP_MAX_BITS     521   /**< Maximum bit size of groups */
#endif

#define ALI_ALGO_ECP_MAX_BYTES    ( ( ALI_ALGO_ECP_MAX_BITS + 7 ) / 8 )
#define ALI_ALGO_ECP_MAX_PT_LEN   ( 2 * ALI_ALGO_ECP_MAX_BYTES + 1 )

#if !defined(ALI_ALGO_ECP_WINDOW_SIZE)
/*
 * Maximum "window" size used for point multiplication.
 * Default: 6.
 * Minimum value: 2. Maximum value: 7.
 *
 * Result is an array of at most ( 1 << ( ALI_ALGO_ECP_WINDOW_SIZE - 1 ) )
 * points used for point multiplication. This value is directly tied to EC
 * peak memory usage, so decreasing it by one should roughly cut memory usage
 * by two (if large curves are in use).
 *
 * Reduction in size may reduce speed, but larger curves are impacted first.
 * Sample performances (in ECDHE handshakes/s, with FIXED_POINT_OPTIM = 1):
 *      w-size:     6       5       4       3       2
 *      521       145     141     135     120      97
 *      384       214     209     198     177     146
 *      256       320     320     303     262     226

 *      224       475     475     453     398     342
 *      192       640     640     633     587     476
 */
#define ALI_ALGO_ECP_WINDOW_SIZE    6   /**< Maximum window size used */
#endif /* ALI_ALGO_ECP_WINDOW_SIZE */

#if !defined(ALI_ALGO_ECP_FIXED_POINT_OPTIM)
/*
 * Trade memory for speed on fixed-point multiplication.
 *
 * This speeds up repeated multiplication of the generator (that is, the
 * multiplication in ECDSA signatures, and half of the multiplications in
 * ECDSA verification and ECDHE) by a factor roughly 3 to 4.
 *
 * The cost is increasing EC peak memory usage by a factor roughly 2.
 *
 * Change this value to 0 to reduce peak memory usage.
 */
#define ALI_ALGO_ECP_FIXED_POINT_OPTIM  1   /**< Enable fixed-point speed-up */
#endif /* ALI_ALGO_ECP_FIXED_POINT_OPTIM */

/* \} name SECTION: Module settings */

/*
 * Point formats, from RFC 4492's enum ECPointFormat
 */
#define ALI_ALGO_ECP_PF_UNCOMPRESSED    0   /**< Uncompressed point format */
#define ALI_ALGO_ECP_PF_COMPRESSED      1   /**< Compressed point format */

/*
 * Some other constants from RFC 4492
 */
#define ALI_ALGO_ECP_TLS_NAMED_CURVE    3   /**< ECCurveType's named_curve */

/**
 * \brief           Get the list of supported curves in order of preferrence
 *                  (full information)
 *
 * \return          A statically allocated array, the last entry is 0.
 */
const ali_algo_ecp_curve_info *ali_algo_ecp_curve_list( void );

/**
 * \brief           Get the list of supported curves in order of preferrence
 *                  (grp_id only)
 *
 * \return          A statically allocated array,
 *                  terminated with ALI_ALGO_ECP_DP_NONE.
 */
const ali_algo_ecp_group_id *ali_algo_ecp_grp_id_list( void );

/**
 * \brief           Get curve information from an internal group identifier
 *
 * \param grp_id    A ALI_ALGO_ECP_DP_XXX value
 *
 * \return          The associated curve information or NULL
 */
const ali_algo_ecp_curve_info *ali_algo_ecp_curve_info_from_grp_id( ali_algo_ecp_group_id grp_id );

/**
 * \brief           Get curve information from a TLS NamedCurve value
 *
 * \param tls_id    A ALI_ALGO_ECP_DP_XXX value
 *
 * \return          The associated curve information or NULL
 */
const ali_algo_ecp_curve_info *ali_algo_ecp_curve_info_from_tls_id( uint16_t tls_id );

/**
 * \brief           Get curve information from a human-readable name
 *
 * \param name      The name
 *
 * \return          The associated curve information or NULL
 */
const ali_algo_ecp_curve_info *ali_algo_ecp_curve_info_from_name( const char *name );

/**
 * \brief           Initialize a point (as zero)
 */
void ali_algo_ecp_point_init( ali_algo_ecp_point *pt );

/**
 * \brief           Initialize a group (to something meaningless)
 */
void ali_algo_ecp_group_init( ali_algo_ecp_group *grp );

/**
 * \brief           Initialize a key pair (as an invalid one)
 */
void ali_algo_ecp_keypair_init( ali_algo_ecp_keypair *key );

/**
 * \brief           Free the components of a point
 */
void ali_algo_ecp_point_free( ali_algo_ecp_point *pt );

/**
 * \brief           Free the components of an ECP group
 */
void ali_algo_ecp_group_free( ali_algo_ecp_group *grp );

/**
 * \brief           Free the components of a key pair
 */
void ali_algo_ecp_keypair_free( ali_algo_ecp_keypair *key );

/**
 * \brief           Copy the contents of point Q into P
 *
 * \param P         Destination point
 * \param Q         Source point
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int ali_algo_ecp_copy( ali_algo_ecp_point *P, const ali_algo_ecp_point *Q );

/**
 * \brief           Copy the contents of a group object
 *
 * \param dst       Destination group
 * \param src       Source group
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int ali_algo_ecp_group_copy( ali_algo_ecp_group *dst, const ali_algo_ecp_group *src );

/**
 * \brief           Set a point to zero
 *
 * \param pt        Destination point
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int ali_algo_ecp_set_zero( ali_algo_ecp_point *pt );

/**
 * \brief           Tell if a point is zero
 *
 * \param pt        Point to test
 *
 * \return          1 if point is zero, 0 otherwise
 */
int ali_algo_ecp_is_zero( ali_algo_ecp_point *pt );

/**
 * \brief           Compare two points
 *
 * \note            This assumes the points are normalized. Otherwise,
 *                  they may compare as "not equal" even if they are.
 *
 * \param P         First point to compare
 * \param Q         Second point to compare
 *
 * \return          0 if the points are equal,
 *                  ALI_ALGO_ERR_ECP_BAD_INPUT_DATA otherwise
 */
int ali_algo_ecp_point_cmp( const ali_algo_ecp_point *P,
                           const ali_algo_ecp_point *Q );

/**
 * \brief           Import a non-zero point from two ASCII strings
 *
 * \param P         Destination point
 * \param radix     Input numeric base
 * \param x         First affine coordinate as a null-terminated string
 * \param y         Second affine coordinate as a null-terminated string
 *
 * \return          0 if successful, or a ALI_ALGO_ERR_MPI_XXX error code
 */
int ali_algo_ecp_point_read_string( ali_algo_ecp_point *P, int radix,
                           const char *x, const char *y );

/**
 * \brief           Export a point into unsigned binary data
 *
 * \param grp       Group to which the point should belong
 * \param P         Point to export
 * \param format    Point format, should be a ALI_ALGO_ECP_PF_XXX macro
 * \param olen      Length of the actual output
 * \param buf       Output buffer
 * \param buflen    Length of the output buffer
 *
 * \return          0 if successful,
 *                  or ALI_ALGO_ERR_ECP_BAD_INPUT_DATA
 *                  or ALI_ALGO_ERR_ECP_BUFFER_TOO_SMALL
 */
int ali_algo_ecp_point_write_binary( const ali_algo_ecp_group *grp, const ali_algo_ecp_point *P,
                            int format, size_t *olen,
                            unsigned char *buf, size_t buflen );

/**
 * \brief           Import a point from unsigned binary data
 *
 * \param grp       Group to which the point should belong
 * \param P         Point to import
 * \param buf       Input buffer
 * \param ilen      Actual length of input
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_ECP_BAD_INPUT_DATA if input is invalid,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed,
 *                  ALI_ALGO_ERR_ECP_FEATURE_UNAVAILABLE if the point format
 *                  is not implemented.
 *
 * \note            This function does NOT check that the point actually
 *                  belongs to the given group, see ali_algo_ecp_check_pubkey() for
 *                  that.
 */
int ali_algo_ecp_point_read_binary( const ali_algo_ecp_group *grp, ali_algo_ecp_point *P,
                           const unsigned char *buf, size_t ilen );

/**
 * \brief           Import a point from a TLS ECPoint record
 *
 * \param grp       ECP group used
 * \param pt        Destination point
 * \param buf       $(Start of input buffer)
 * \param len       Buffer length
 *
 * \note            buf is updated to point right after the ECPoint on exit
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_XXX if initialization failed
 *                  ALI_ALGO_ERR_ECP_BAD_INPUT_DATA if input is invalid
 */
int ali_algo_ecp_tls_read_point( const ali_algo_ecp_group *grp, ali_algo_ecp_point *pt,
                        const unsigned char **buf, size_t len );

/**
 * \brief           Export a point as a TLS ECPoint record
 *
 * \param grp       ECP group used
 * \param pt        Point to export
 * \param format    Export format
 * \param olen      length of data written
 * \param buf       Buffer to write to
 * \param blen      Buffer length
 *
 * \return          0 if successful,
 *                  or ALI_ALGO_ERR_ECP_BAD_INPUT_DATA
 *                  or ALI_ALGO_ERR_ECP_BUFFER_TOO_SMALL
 */
int ali_algo_ecp_tls_write_point( const ali_algo_ecp_group *grp, const ali_algo_ecp_point *pt,
                         int format, size_t *olen,
                         unsigned char *buf, size_t blen );

/**
 * \brief           Set a group using well-known domain parameters
 *
 * \param grp       Destination group
 * \param index     Index in the list of well-known domain parameters
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_XXX if initialization failed
 *                  ALI_ALGO_ERR_ECP_FEATURE_UNAVAILABLE for unkownn groups
 *
 * \note            Index should be a value of RFC 4492's enum NamedCurve,
 *                  usually in the form of a ALI_ALGO_ECP_DP_XXX macro.
 */
int ali_algo_ecp_group_load( ali_algo_ecp_group *grp, ali_algo_ecp_group_id index );

/**
 * \brief           Set a group from a TLS ECParameters record
 *
 * \param grp       Destination group
 * \param buf       &(Start of input buffer)
 * \param len       Buffer length
 *
 * \note            buf is updated to point right after ECParameters on exit
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_MPI_XXX if initialization failed
 *                  ALI_ALGO_ERR_ECP_BAD_INPUT_DATA if input is invalid
 */
int ali_algo_ecp_tls_read_group( ali_algo_ecp_group *grp, const unsigned char **buf, size_t len );

/**
 * \brief           Write the TLS ECParameters record for a group
 *
 * \param grp       ECP group used
 * \param olen      Number of bytes actually written
 * \param buf       Buffer to write to
 * \param blen      Buffer length
 *
 * \return          0 if successful,
 *                  or ALI_ALGO_ERR_ECP_BUFFER_TOO_SMALL
 */
int ali_algo_ecp_tls_write_group( const ali_algo_ecp_group *grp, size_t *olen,
                         unsigned char *buf, size_t blen );

/**
 * \brief           Multiplication by an integer: R = m * P
 *                  (Not thread-safe to use same group in multiple threads)
 *
 * \note            In order to prevent timing attacks, this function
 *                  executes the exact same sequence of (base field)
 *                  operations for any valid m. It avoids any if-branch or
 *                  array index depending on the value of m.
 *
 * \note            If f_rng is not NULL, it is used to randomize intermediate
 *                  results in order to prevent potential timing attacks
 *                  targeting these results. It is recommended to always
 *                  provide a non-NULL f_rng (the overhead is negligible).
 *
 * \param grp       ECP group
 * \param R         Destination point
 * \param m         Integer by which to multiply
 * \param P         Point to multiply
 * \param f_rng     RNG function (see notes)
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_ECP_INVALID_KEY if m is not a valid privkey
 *                  or P is not a valid pubkey,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int ali_algo_ecp_mul( ali_algo_ecp_group *grp, ali_algo_ecp_point *R,
             const ali_algo_mpi *m, const ali_algo_ecp_point *P,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Multiplication and addition of two points by integers:
 *                  R = m * P + n * Q
 *                  (Not thread-safe to use same group in multiple threads)
 *
 * \note            In contrast to ali_algo_ecp_mul(), this function does not guarantee
 *                  a constant execution flow and timing.
 *
 * \param grp       ECP group
 * \param R         Destination point
 * \param m         Integer by which to multiply P
 * \param P         Point to multiply by m
 * \param n         Integer by which to multiply Q
 * \param Q         Point to be multiplied by n
 *
 * \return          0 if successful,
 *                  ALI_ALGO_ERR_ECP_INVALID_KEY if m or n is not a valid privkey
 *                  or P or Q is not a valid pubkey,
 *                  ALI_ALGO_ERR_MPI_ALLOC_FAILED if memory allocation failed
 */
int ali_algo_ecp_muladd( ali_algo_ecp_group *grp, ali_algo_ecp_point *R,
             const ali_algo_mpi *m, const ali_algo_ecp_point *P,
             const ali_algo_mpi *n, const ali_algo_ecp_point *Q );

/**
 * \brief           Check that a point is a valid public key on this curve
 *
 * \param grp       Curve/group the point should belong to
 * \param pt        Point to check
 *
 * \return          0 if point is a valid public key,
 *                  ALI_ALGO_ERR_ECP_INVALID_KEY otherwise.
 *
 * \note            This function only checks the point is non-zero, has valid
 *                  coordinates and lies on the curve, but not that it is
 *                  indeed a multiple of G. This is additional check is more
 *                  expensive, isn't required by standards, and shouldn't be
 *                  necessary if the group used has a small cofactor. In
 *                  particular, it is useless for the NIST groups which all
 *                  have a cofactor of 1.
 *
 * \note            Uses bare components rather than an ali_algo_ecp_keypair structure
 *                  in order to ease use with other structures such as
 *                  ali_algo_ecdh_context of ali_algo_ecdsa_context.
 */
int ali_algo_ecp_check_pubkey( const ali_algo_ecp_group *grp, const ali_algo_ecp_point *pt );

/**
 * \brief           Check that an ali_algo_mpi is a valid private key for this curve
 *
 * \param grp       Group used
 * \param d         Integer to check
 *
 * \return          0 if point is a valid private key,
 *                  ALI_ALGO_ERR_ECP_INVALID_KEY otherwise.
 *
 * \note            Uses bare components rather than an ali_algo_ecp_keypair structure
 *                  in order to ease use with other structures such as
 *                  ali_algo_ecdh_context of ali_algo_ecdsa_context.
 */
int ali_algo_ecp_check_privkey( const ali_algo_ecp_group *grp, const ali_algo_mpi *d );

/**
 * \brief           Generate a keypair with configurable base point
 *
 * \param grp       ECP group
 * \param G         Chosen base point
 * \param d         Destination MPI (secret part)
 * \param Q         Destination point (public part)
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a ALI_ALGO_ERR_ECP_XXX or ALI_ALGO_MPI_XXX error code
 *
 * \note            Uses bare components rather than an ali_algo_ecp_keypair structure
 *                  in order to ease use with other structures such as
 *                  ali_algo_ecdh_context of ali_algo_ecdsa_context.
 */
int ali_algo_ecp_gen_keypair_base( ali_algo_ecp_group *grp,
                     const ali_algo_ecp_point *G,
                     ali_algo_mpi *d, ali_algo_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

/**
 * \brief           Generate a keypair
 *
 * \param grp       ECP group
 * \param d         Destination MPI (secret part)
 * \param Q         Destination point (public part)
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a ALI_ALGO_ERR_ECP_XXX or ALI_ALGO_MPI_XXX error code
 *
 * \note            Uses bare components rather than an ali_algo_ecp_keypair structure
 *                  in order to ease use with other structures such as
 *                  ali_algo_ecdh_context of ali_algo_ecdsa_context.
 */
int ali_algo_ecp_gen_keypair( ali_algo_ecp_group *grp, ali_algo_mpi *d, ali_algo_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng );

/**
 * \brief           Generate a keypair
 *
 * \param grp_id    ECP group identifier
 * \param key       Destination keypair
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful,
 *                  or a ALI_ALGO_ERR_ECP_XXX or ALI_ALGO_MPI_XXX error code
 */
int ali_algo_ecp_gen_key( ali_algo_ecp_group_id grp_id, ali_algo_ecp_keypair *key,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

/**
 * \brief           Check a public-private key pair
 *
 * \param pub       Keypair structure holding a public key
 * \param prv       Keypair structure holding a private (plus public) key
 *
 * \return          0 if successful (keys are valid and match), or
 *                  ALI_ALGO_ERR_ECP_BAD_INPUT_DATA, or
 *                  a ALI_ALGO_ERR_ECP_XXX or ALI_ALGO_ERR_MPI_XXX code.
 */
int ali_algo_ecp_check_pub_priv( const ali_algo_ecp_keypair *pub, const ali_algo_ecp_keypair *prv );

#if defined(ALI_ALGO_SELF_TEST)
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if a test failed
 */
int ali_algo_ecp_self_test( int verbose );
#endif

#ifdef __cplusplus
}
#endif

#endif /* ecp.h */
