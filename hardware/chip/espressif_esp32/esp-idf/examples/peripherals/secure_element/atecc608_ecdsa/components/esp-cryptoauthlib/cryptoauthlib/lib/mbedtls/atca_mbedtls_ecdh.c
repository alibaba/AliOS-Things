/**
 * \brief Replace mbedTLS ECDH Functions with hardware acceleration &
 * hardware key security.
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

#if defined(MBEDTLS_ECDH_C)

#include "mbedtls/ecdh.h"
#include "mbedtls/platform_util.h"

/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "basic/atca_basic.h"
#include "atca_mbedtls_wrap.h"
#include <string.h>

#ifdef MBEDTLS_ECDH_GEN_PUBLIC_ALT
/** Generate ECDH keypair */
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
    int ret = 0;
    uint8_t public_key[ATCA_PUB_KEY_SIZE];
    uint8_t temp = 1;
    uint16_t slotid = atca_mbedtls_ecdh_slot_cb();

    if (!grp || !d || !Q)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id != MBEDTLS_ECP_DP_SECP256R1)
    {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (!ret)
    {
        ret = mbedtls_mpi_lset(d, slotid);
    }

    if (!ret)
    {
        ret = atcab_genkey(slotid, public_key);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(Q->X), public_key, ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(Q->Y), &public_key[ATCA_PUB_KEY_SIZE / 2], ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(Q->Z), &temp, 1);
    }

    return ret;
}
#endif /* MBEDTLS_ECDH_GEN_PUBLIC_ALT */

#ifdef MBEDTLS_ECDH_COMPUTE_SHARED_ALT
/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{

    int ret = 0;
    uint8_t public_key[ATCA_PUB_KEY_SIZE];
    uint8_t shared_key[ATCA_KEY_SIZE];
    uint16_t slotid;
    uint8_t secret[ATCA_KEY_SIZE];

    if (!grp || !z || !Q || !d)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (grp->id != MBEDTLS_ECP_DP_SECP256R1)
    {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
    }

    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(&(Q->X), public_key, ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_write_binary(&(Q->Y), &public_key[ATCA_PUB_KEY_SIZE / 2], ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        slotid = *(uint16_t*)d->p;
        if (ATECC608A == atcab_get_device()->mIface->mIfaceCFG->devtype)
        {
            ret = atca_mbedtls_ecdh_ioprot_cb(secret);
            if (!ret)
            {
                if (slotid > 15)
                {
                    ret = atcab_ecdh_tempkey_ioenc(public_key, shared_key, secret);
                }
                else
                {
                    ret = atcab_ecdh_ioenc(slotid, public_key, shared_key, secret);
                }
            }
            mbedtls_platform_zeroize(secret, ATCA_KEY_SIZE);
        }
        else
        {
            ret = atcab_ecdh(slotid, public_key, shared_key);
        }
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(z, shared_key, ATCA_KEY_SIZE);
    }

    return ret;
}
#endif /* MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

#endif /* MBEDTLS_ECDH_C */
