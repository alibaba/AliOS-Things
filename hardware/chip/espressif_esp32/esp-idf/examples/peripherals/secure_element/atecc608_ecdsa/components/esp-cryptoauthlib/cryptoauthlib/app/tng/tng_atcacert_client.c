/**
 * \file
 * \brief Client side certificate I/O functions for TNG devices.
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

#include "tng_atca.h"
#include "atcacert/atcacert_client.h"
#include "tng_atcacert_client.h"
#include "tngtls_cert_def_1_signer.h"
#include "tngtls_cert_def_2_device.h"
#include "tngtls_cert_def_3_device.h"
#include "tnglora_cert_def_1_signer.h"
#include "tnglora_cert_def_2_device.h"
#include "tnglora_cert_def_4_device.h"
#include "tng_root_cert.h"

int tng_atcacert_max_device_cert_size(size_t* max_cert_size)
{
    return atcacert_max_cert_size(&g_tnglora_cert_def_4_device, max_cert_size);
}

int tng_atcacert_read_device_cert(uint8_t* cert, size_t* cert_size, const uint8_t* signer_cert)
{
    int ret;
    const atcacert_def_t* cert_def = NULL;
    uint8_t ca_public_key[72];

    ret = tng_get_device_cert_def(&cert_def);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }
    // Get the CA (signer) public key
    if (signer_cert != NULL)
    {
        // Signer certificate is supplied, get the public key from there
        ret = atcacert_get_subj_public_key(
            cert_def->ca_cert_def,
            signer_cert,
            cert_def->ca_cert_def->cert_template_size,  // Cert size doesn't need to be accurate
            ca_public_key);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }
    }
    else
    {
        // No signer certificate supplied, read from the device
        ret = atcacert_read_device_loc(&cert_def->ca_cert_def->public_key_dev_loc, ca_public_key);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }
        if (cert_def->ca_cert_def->public_key_dev_loc.count == 72)
        {
            // Public key is formatted with padding bytes in front of the X and Y components
            atcacert_public_key_remove_padding(ca_public_key, ca_public_key);
        }
    }

    return atcacert_read_cert(cert_def, ca_public_key, cert, cert_size);
}

int tng_atcacert_device_public_key(uint8_t* public_key, uint8_t* cert)
{
    int ret;
    const atcacert_def_t* cert_def = NULL;
    uint8_t raw_public_key[72];

    if (public_key == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = tng_get_device_cert_def(&cert_def);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    ret = atcacert_read_device_loc(&cert_def->public_key_dev_loc, raw_public_key);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }
    if (cert_def->public_key_dev_loc.count == 72)
    {
        // Public key is formatted with padding bytes in front of the X and Y components
        atcacert_public_key_remove_padding(raw_public_key, public_key);
    }
    else
    {
        memcpy(public_key, raw_public_key, 64);
    }

    return ATCACERT_E_SUCCESS;
}

int tng_atcacert_max_signer_cert_size(size_t* max_cert_size)
{
    return atcacert_max_cert_size(&g_tngtls_cert_def_1_signer, max_cert_size);
}

int tng_atcacert_read_signer_cert(uint8_t* cert, size_t* cert_size)
{
    int ret;
    tng_type_t type;
    const atcacert_def_t* cert_def = NULL;
    const uint8_t* ca_public_key = NULL;

    ret = tng_get_type(&type);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    if (type == TNGTYPE_LORA)
    {
        cert_def = &g_tnglora_cert_def_1_signer;
    }
    else
    {
        cert_def = &g_tngtls_cert_def_1_signer;
    }

    // Get the CA (root) public key
    ca_public_key = &g_cryptoauth_root_ca_002_cert[CRYPTOAUTH_ROOT_CA_002_PUBLIC_KEY_OFFSET];

    return atcacert_read_cert(cert_def, ca_public_key, cert, cert_size);
}

int tng_atcacert_signer_public_key(uint8_t* public_key, uint8_t* cert)
{
    int ret;
    tng_type_t type;
    const atcacert_def_t* cert_def = NULL;
    uint8_t raw_public_key[72];

    if (public_key == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (cert != NULL)
    {
        // TNG TLS cert def will work for either if the certificate is supplied
        return atcacert_get_subj_public_key(
            &g_tngtls_cert_def_1_signer,
            cert,
            g_tngtls_cert_def_1_signer.cert_template_size,  // cert size doesn't need to be accurate
            public_key);
    }
    else
    {
        ret = tng_get_type(&type);
        if (ret != ATCA_SUCCESS)
        {
            return ret;
        }

        if (type == TNGTYPE_LORA)
        {
            cert_def = &g_tnglora_cert_def_1_signer;
        }
        else
        {
            cert_def = &g_tngtls_cert_def_1_signer;
        }

        ret = atcacert_read_device_loc(&cert_def->public_key_dev_loc, raw_public_key);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }
        if (cert_def->public_key_dev_loc.count == 72)
        {
            // Public key is formatted with padding bytes in front of the X and Y components
            atcacert_public_key_remove_padding(raw_public_key, public_key);
        }
        else
        {
            memcpy(public_key, raw_public_key, 64);
        }
    }

    return ATCACERT_E_SUCCESS;
}

int tng_atcacert_root_cert_size(size_t* cert_size)
{
    if (cert_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    *cert_size = g_cryptoauth_root_ca_002_cert_size;

    return ATCACERT_E_SUCCESS;
}

int tng_atcacert_root_cert(uint8_t* cert, size_t* cert_size)
{
    if (cert == NULL || cert_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    if (*cert_size < g_cryptoauth_root_ca_002_cert_size)
    {
        return ATCACERT_E_BUFFER_TOO_SMALL;
    }

    memcpy(cert, g_cryptoauth_root_ca_002_cert, g_cryptoauth_root_ca_002_cert_size);
    *cert_size = g_cryptoauth_root_ca_002_cert_size;

    return ATCACERT_E_SUCCESS;
}

int tng_atcacert_root_public_key(uint8_t* public_key)
{
    if (public_key == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    memcpy(public_key, &g_cryptoauth_root_ca_002_cert[CRYPTOAUTH_ROOT_CA_002_PUBLIC_KEY_OFFSET], 64);

    return ATCACERT_E_SUCCESS;
}
