/**
 * \file
 * \brief host side methods using CryptoAuth hardware
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

#include "atcacert_host_hw.h"
#include "basic/atca_basic.h"
#include "crypto/atca_crypto_sw_sha2.h"





int atcacert_verify_cert_hw(const atcacert_def_t* cert_def,
                            const uint8_t*        cert,
                            size_t                cert_size,
                            const uint8_t         ca_public_key[64])
{
    int ret = 0;
    uint8_t tbs_digest[32];
    uint8_t signature[64];
    bool is_verified = false;

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

    ret = atcab_verify_extern(tbs_digest, signature, ca_public_key, &is_verified);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    return is_verified ? ATCACERT_E_SUCCESS : ATCACERT_E_VERIFY_FAILED;
}




int atcacert_gen_challenge_hw(uint8_t challenge[32])
{
    if (challenge == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    return atcab_random(challenge);
}


int atcacert_verify_response_hw(const uint8_t device_public_key[64],
                                const uint8_t challenge[32],
                                const uint8_t response[64])
{
    int ret = 0;
    bool is_verified = false;

    if (device_public_key == NULL || challenge == NULL || response == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcab_verify_extern(challenge, response, device_public_key, &is_verified);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    return is_verified ? ATCACERT_E_SUCCESS : ATCACERT_E_VERIFY_FAILED;
}