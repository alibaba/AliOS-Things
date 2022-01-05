/**
 * \brief Replace mbedTLS ECDSA Functions with hardware acceleration &
 * hardware key security
 *
 * \copyright (c) 2015-2019 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

/* mbedTLS boilerplate includes */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECDSA_C)

#include "mbedtls/ecdsa.h"

/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "basic/atca_basic.h"
#include <string.h>


#ifdef MBEDTLS_ECDSA_SIGN_ALT
/*
 * Compute ECDSA signature of a hashed message (SEC1 4.1.3)
 * Obviously, compared to SEC1 4.1.3, we skip step 4 (hash message)
 */
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = 0;
    uint8_t raw_sig[ATCA_SIG_SIZE];

    ((void)f_rng);
    ((void)p_rng);

    if (ATCA_KEY_SIZE > blen || !d || !d->p)
    {
        ret = MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if (!ret)
    {
        ret = atcab_sign(*(uint16_t*)d->p, buf, raw_sig);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(r, raw_sig, ATCA_SIG_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(s, &raw_sig[ATCA_SIG_SIZE / 2], ATCA_SIG_SIZE / 2);
    }

    return ret;
}

#endif /* MBEDTLS_ECDSA_SIGN_ALT */

#ifdef MBEDTLS_ECDSA_VERIFY_ALT
/*
 * Verify ECDSA signature of hashed message
 */
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s)
{
    int ret = 0;
    uint8_t raw_sig[ATCA_SIG_SIZE];
    uint8_t public_key[ATCA_PUB_KEY_SIZE];
    bool verified = false;

    if (!grp || !buf || !Q || !r || !s)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id != MBEDTLS_ECP_DP_SECP256R1)
    {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    /* Convert the public key to it's uncompressed binary */
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(&(Q->X), public_key, ATCA_PUB_KEY_SIZE / 2);
    }
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(&(Q->Y), &public_key[ATCA_PUB_KEY_SIZE / 2], ATCA_PUB_KEY_SIZE / 2);
    }

    /* Convert the signature to binary */
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(r, raw_sig, ATCA_SIG_SIZE / 2);
    }
    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(s, &raw_sig[ATCA_SIG_SIZE / 2], ATCA_SIG_SIZE / 2);
    }

    if (!ret)
    {
        ret = atcab_verify_extern(buf, raw_sig, public_key, &verified);

        if (!ret && !verified)
        {
            ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        }
    }

    return ret;
}
#endif /* !MBEDTLS_ECDSA_VERIFY_ALT */

#endif /* MBEDTLS_ECDSA_C */
