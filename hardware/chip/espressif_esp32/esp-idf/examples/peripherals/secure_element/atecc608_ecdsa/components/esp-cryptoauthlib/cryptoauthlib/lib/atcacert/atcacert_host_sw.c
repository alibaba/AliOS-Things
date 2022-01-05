/**
 * \file
 * \brief host side methods using software implementations
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "atcacert_host_sw.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "crypto/atca_crypto_sw_ecdsa.h"
#include "crypto/atca_crypto_sw_rand.h"






int atcacert_verify_cert_sw(const atcacert_def_t* cert_def,
                            const uint8_t*        cert,
                            size_t                cert_size,
                            const uint8_t         ca_public_key[64])
{
    int ret = 0;
    uint8_t tbs_digest[32];
    uint8_t signature[64];

    if (cert_def == NULL || ca_public_key == NULL || cert == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcacert_get_tbs_digest(cert_def, cert, cert_size, tbs_digest);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    ret = atcacert_get_signature(cert_def, cert, cert_size, signature);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    ret = atcac_sw_ecdsa_verify_p256(tbs_digest, signature, ca_public_key);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    return ATCACERT_E_SUCCESS;
}



int atcacert_gen_challenge_sw(uint8_t challenge[32])
{
    if (challenge == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    return atcac_sw_random(challenge, 32);
}



int atcacert_verify_response_sw(const uint8_t device_public_key[64],
                                const uint8_t challenge[32],
                                const uint8_t response[64])
{
    if (device_public_key == NULL || challenge == NULL || response == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    return atcac_sw_ecdsa_verify_p256(challenge, response, device_public_key);
}