/*
 *  Elliptic curves over GF(p): generic functions
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * References:
 *
 * SEC1 http://www.secg.org/index.php?action=secg,docs_secg
 * GECC = Guide to Elliptic Curve Cryptography - Hankerson, Menezes, Vanstone
 * FIPS 186-3 http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf
 * RFC 4492 for the related TLS structures and constants
 *
 * [Curve25519] http://cr.yp.to/ecdh/curve25519-20060209.pdf
 *
 * [2] CORON, Jean-S'ebastien. Resistance against differential power analysis
 *     for elliptic curve cryptosystems. In : Cryptographic Hardware and
 *     Embedded Systems. Springer Berlin Heidelberg, 1999. p. 292-302.
 *     <http://link.springer.com/chapter/10.1007/3-540-48059-5_25>
 *
 * [3] HEDABOU, Mustapha, PINEL, Pierre, et B'EN'ETEAU, Lucien. A comb method to
 *     render ECC resistant against Side Channel Attacks. IACR Cryptology
 *     ePrint Archive, 2004, vol. 2004, p. 342.
 *     <http://eprint.iacr.org/2004/342.pdf>
 */

#include <section_config.h>
//#include <rom_ssl_func_rename.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wint-conversion"

#define memset _memset
#define strcmp _strcmp

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#include "mbedtls/ecp.h"

#include <string.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

/* Implementation that should never be optimized out by the compiler */
SSL_ROM_TEXT_SECTION
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#if defined(MBEDTLS_SELF_TEST)
/*
 * Counts of point addition and doubling, and field multiplications.
 * Used to test resistance of point multiplication to simple timing attacks.
 */
/* static */ unsigned long add_count, dbl_count, mul_count;
#endif
/*
 * List of supported curves:
 *  - internal ID
 *  - TLS NamedCurve ID (RFC 4492 sec. 5.1.1, RFC 7071 sec. 2)
 *  - size in bits
 *  - readable name
 *
 * Curves are listed in order: largest curves first, and for a given size,
 * fastest curves first. This provides the default order for the SSL module.
 *
 * Reminder: update profiles in x509_crt.c when adding a new curves!
 */
static const mbedtls_ecp_curve_info ecp_supported_curves[] =
{
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP521R1,    25,     521,    "secp521r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    { MBEDTLS_ECP_DP_BP512R1,      28,     512,    "brainpoolP512r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP384R1,    24,     384,    "secp384r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    { MBEDTLS_ECP_DP_BP384R1,      27,     384,    "brainpoolP384r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP256R1,    23,     256,    "secp256r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP256K1,    22,     256,    "secp256k1"         },
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    { MBEDTLS_ECP_DP_BP256R1,      26,     256,    "brainpoolP256r1"   },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP224R1,    21,     224,    "secp224r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP224K1,    20,     224,    "secp224k1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    { MBEDTLS_ECP_DP_SECP192R1,    19,     192,    "secp192r1"         },
#endif
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
    { MBEDTLS_ECP_DP_SECP192K1,    18,     192,    "secp192k1"         },
#endif
    { MBEDTLS_ECP_DP_NONE,          0,     0,      NULL                },
};

#define ECP_NB_CURVES   sizeof( ecp_supported_curves ) /    \
                        sizeof( ecp_supported_curves[0] )
static mbedtls_ecp_group_id ecp_supported_grp_id[ECP_NB_CURVES];


/*
 * List of supported curves, group ID only
 */
const mbedtls_ecp_group_id *mbedtls_ecp_grp_id_list( void )
{
    static int init_done = 0;

    if( ! init_done )
    {
        size_t i = 0;
        const mbedtls_ecp_curve_info *curve_info;

        for( curve_info = mbedtls_ecp_curve_list();
             curve_info->grp_id != MBEDTLS_ECP_DP_NONE;
             curve_info++ )
        {
            ecp_supported_grp_id[i++] = curve_info->grp_id;
        }
        ecp_supported_grp_id[i] = MBEDTLS_ECP_DP_NONE;

        init_done = 1;
    }

    return( ecp_supported_grp_id );
}

#endif /* MBEDTLS_ECP_C */
