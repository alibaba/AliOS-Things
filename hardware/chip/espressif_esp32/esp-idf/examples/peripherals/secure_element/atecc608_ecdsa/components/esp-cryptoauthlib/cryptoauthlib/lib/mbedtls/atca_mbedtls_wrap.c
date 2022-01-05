/**
 * \file
 * \brief Wrapper functions to replace cryptoauthlib software crypto functions
 *        with the mbedTLS equivalent
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

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#include "mbedtls/pk.h"
#include "mbedtls/ecp.h"
#include "mbedtls/x509_crt.h"

/* Cryptoauthlib Includes */
#include "cryptoauthlib.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_def.h"

#if 0

#include "crypto/atca_crypto_sw_sha1.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "crypto/atca_crypto_sw_ecdsa.h"
#include "crypto/atca_crypto_sw_rand.h"

int atcac_sw_sha1(const uint8_t * data, size_t data_size, uint8_t digest[ATCA_SHA1_DIGEST_SIZE])
{

}

int atcac_sw_sha2_256_init(atcac_sha2_256_ctx* ctx)
{

}

int atcac_sw_sha2_256_update(atcac_sha2_256_ctx* ctx, const uint8_t* data, size_t data_size)
{

}

int atcac_sw_sha2_256_finish(atcac_sha2_256_ctx * ctx, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE])
{

}

int atcac_sw_sha2_256(const uint8_t * data, size_t data_size, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE])
{

}

int atcac_sw_ecdsa_verify_p256(const uint8_t msg[ATCA_ECC_P256_FIELD_SIZE],
                               const uint8_t signature[ATCA_ECC_P256_SIGNATURE_SIZE],
                               const uint8_t public_key[ATCA_ECC_P256_PUBLIC_KEY_SIZE])
{

}

int atcac_sw_random(uint8_t* data, size_t data_size)
{

}

#endif

/** \brief Initializes an mbedtls pk context for use with EC operations
 * \param[in,out] pkey ptr to space to receive version string
 * \param[in] slotid Associated with this key
 * \return 0 on success, otherwise an error code.
 */
int atca_mbedtls_pk_init(mbedtls_pk_context * pkey, const uint16_t slotid)
{
    int ret = 0;
    uint8_t public_key[ATCA_PUB_KEY_SIZE];
    mbedtls_ecp_keypair * ecp = NULL;
    uint8_t temp = 1;

    if (!pkey)
    {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    if (!ret)
    {
        mbedtls_pk_init(pkey);
        ret = mbedtls_pk_setup(pkey, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    }

    if (!ret)
    {
        ecp = mbedtls_pk_ec(*pkey);
        ret = mbedtls_ecp_group_load(&ecp->grp, MBEDTLS_ECP_DP_SECP256R1);
    }

    if (!ret)
    {
        ret = atcab_get_pubkey(slotid, public_key);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(ecp->Q.X), public_key, ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(ecp->Q.Y), &public_key[ATCA_PUB_KEY_SIZE / 2], ATCA_PUB_KEY_SIZE / 2);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_read_binary(&(ecp->Q.Z), &temp, 1);
    }

    if (!ret)
    {
        ret = mbedtls_mpi_lset(&ecp->d, slotid);
    }

    return ret;
}

/** \brief Rebuild a certificate from an atcacert_def_t structure, and then add
 * it to an mbedtls cert chain.
 * \param[in,out] cert mbedtls cert chain. Must have already been initialized
 * \param[in] cert_def Certificate definition that will be rebuilt and added
 * \return 0 on success, otherwise an error code.
 */
int atca_mbedtls_cert_add(mbedtls_x509_crt * cert, const atcacert_def_t * cert_def)
{
    uint8_t ca_key[64];
    int ret = ATCA_SUCCESS;
    size_t cert_len;
    uint8_t * cert_buf = NULL;

    if (cert_def->ca_cert_def)
    {
        const atcacert_device_loc_t * ca_key_cfg = &cert_def->ca_cert_def->public_key_dev_loc;

        if (ca_key_cfg->is_genkey)
        {
            ret = atcab_get_pubkey(ca_key_cfg->slot, ca_key);
        }
        else
        {
            ret = atcab_read_pubkey(ca_key_cfg->slot, ca_key);
        }
    }

    cert_len = cert_def->cert_template_size + 8;
    if (NULL == (cert_buf = mbedtls_calloc(1, cert_len)))
    {
        ret = -1;
    }

    if (0 == ret)
    {
        ret = atcacert_read_cert(cert_def, cert_def->ca_cert_def ? ca_key : NULL, cert_buf, &cert_len);
    }

    if (0 == ret)
    {
        ret = mbedtls_x509_crt_parse(cert, (const unsigned char*)cert_buf, cert_len);
    }

    if (cert_buf)
    {
        mbedtls_free(cert_buf);
    }

    return ret;
}

