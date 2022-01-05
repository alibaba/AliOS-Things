/**
 * \file
 * \brief Client side cert i/o methods. These declarations deal with the client-side, the node being authenticated,
 *        of the authentication process. It is assumed the client has an ECC CryptoAuthentication device
 *        (e.g. ATECC508A) and the certificates are stored on that device.
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


#include <stdlib.h>
#include "atcacert_client.h"
#include "atcacert_pem.h"
#include "cryptoauthlib.h"
#include "basic/atca_basic.h"

// Perform floor integer division (-1 / 2 == -1) instead of truncate towards zero (-1 / 2 == 0)
static int floor_div(int a, int b)
{
    int d = a / b;
    int r = a % b;

    return r ? (d - ((a < 0) ^ (b < 0))) : d;
}

int atcacert_get_response(uint8_t       device_private_key_slot,
                          const uint8_t challenge[32],
                          uint8_t       response[64])
{
    if (device_private_key_slot > 15 || challenge == NULL || response == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    return atcab_sign(device_private_key_slot, challenge, response);
}

int atcacert_read_device_loc(const atcacert_device_loc_t* device_loc,
                             uint8_t*                     data)
{
    int ret = 0;

    if (device_loc->zone == DEVZONE_DATA && device_loc->is_genkey)
    {
        uint8_t public_key[ATCA_PUB_KEY_SIZE];
        if (device_loc->offset + device_loc->count > sizeof(public_key))
        {
            return ATCACERT_E_BAD_PARAMS;
        }

        ret = atcab_get_pubkey(device_loc->slot, public_key);
        if (ret != ATCA_SUCCESS)
        {
            return ret;
        }
        memcpy(data, &public_key[device_loc->offset], device_loc->count);
    }
    else
    {
        size_t count = device_loc->count;
        size_t zone_size;
        ret = atcab_get_zone_size(device_loc->zone, device_loc->slot, &zone_size);
        if (ret != ATCA_SUCCESS)
        {
            return ret;
        }
        if (device_loc->offset + device_loc->count > (uint16_t)zone_size)
        {
            if (device_loc->offset > zone_size)
            {
                return ATCACERT_E_BAD_PARAMS;
            }
            count = zone_size - device_loc->offset;
        }

        ret = atcab_read_bytes_zone(
            device_loc->zone,
            device_loc->slot,
            device_loc->offset,
            data,
            count);
        if (ret != ATCA_SUCCESS)
        {
            return ret;
        }
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_read_cert(const atcacert_def_t* cert_def,
                       const uint8_t         ca_public_key[64],
                       uint8_t*              cert,
                       size_t*               cert_size)
{
    int ret = 0;
    atcacert_device_loc_t device_locs[16];
    size_t device_locs_count = 0;
    size_t i = 0;
    atcacert_build_state_t build_state;

    if (cert_def == NULL || cert == NULL || cert_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcacert_get_device_locs(
        cert_def,
        device_locs,
        &device_locs_count,
        sizeof(device_locs) / sizeof(device_locs[0]),
        ATCA_BLOCK_SIZE);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    ret = atcacert_cert_build_start(&build_state, cert_def, cert, cert_size, ca_public_key);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    for (i = 0; i < device_locs_count; i++)
    {
        static uint8_t data[416];
        ret = atcacert_read_device_loc(&device_locs[i], data);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }

        ret = atcacert_cert_build_process(&build_state, &device_locs[i], data);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }
    }

    ret = atcacert_cert_build_finish(&build_state);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_write_cert(const atcacert_def_t* cert_def,
                        const uint8_t*        cert,
                        size_t                cert_size)
{
    int ret = 0;
    atcacert_device_loc_t device_locs[16];
    size_t device_locs_count = 0;
    size_t i = 0;

    if (cert_def == NULL || cert == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }

    ret = atcacert_get_device_locs(
        cert_def,
        device_locs,
        &device_locs_count,
        sizeof(device_locs) / sizeof(device_locs[0]),
        ATCA_BLOCK_SIZE);
    if (ret != ATCACERT_E_SUCCESS)
    {
        return ret;
    }

    for (i = 0; i < device_locs_count; i++)
    {
        int end_block;
        int start_block;
        static uint8_t data[416];
        int block;

        if (device_locs[i].zone == DEVZONE_CONFIG)
        {
            continue;  // Cert data isn't written to the config zone, only read
        }
        if (device_locs[i].zone == DEVZONE_DATA && device_locs[i].is_genkey)
        {
            continue;  // Public key is generated not written

        }
        ret = atcacert_get_device_data(cert_def, cert, cert_size, &device_locs[i], data);
        if (ret != ATCACERT_E_SUCCESS)
        {
            return ret;
        }

        start_block = device_locs[i].offset / ATCA_BLOCK_SIZE;
        end_block = floor_div((int)(device_locs[i].offset + device_locs[i].count) - 1, ATCA_BLOCK_SIZE);
        for (block = start_block; block <= end_block; block++)
        {
            ret = atcab_write_zone(
                device_locs[i].zone,
                device_locs[i].slot,
                (uint8_t)block,
                0,
                &data[(block - start_block) * ATCA_BLOCK_SIZE],
                ATCA_BLOCK_SIZE);
            if (ret != ATCA_SUCCESS)
            {
                return ret;
            }
        }
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_create_csr_pem(const atcacert_def_t* csr_def, char* csr, size_t* csr_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    size_t csr_max_size;
    size_t csr_der_size;

    // Check the pointers
    if (csr_def == NULL || csr == NULL || csr_size == NULL)
    {
        return ATCACERT_E_BAD_PARAMS;
    }
    csr_max_size = *csr_size;
    *csr_size = 0;

    // Create DER CSR
    csr_der_size = csr_max_size;
    status = atcacert_create_csr(csr_def, (uint8_t*)csr, &csr_der_size);
    if (status != ATCACERT_E_SUCCESS)
    {
        return status;
    }

    // Move the DER CSR to the end of the buffer, so we can encode it into
    // PEM in place.
    memmove(csr + (csr_max_size - csr_der_size), csr, csr_der_size);

    *csr_size = csr_max_size;
    status = atcacert_encode_pem_csr((uint8_t*)(csr + (csr_max_size - csr_der_size)), csr_der_size, csr, csr_size);
    if (status != ATCACERT_E_SUCCESS)
    {
        return status;
    }

    return ATCACERT_E_SUCCESS;
}

int atcacert_create_csr(const atcacert_def_t* csr_def, uint8_t* csr, size_t* csr_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t pub_key[ATCA_PUB_KEY_SIZE] = { 0 };
    uint8_t sig[ATCA_SIG_SIZE] = { 0 };
    const atcacert_device_loc_t* pub_dev_loc = NULL;
    const atcacert_cert_loc_t* pub_loc = NULL;
    uint16_t key_slot = 0;
    uint16_t priv_key_slot = 0;
    uint8_t tbs_digest[ATCA_BLOCK_SIZE] = { 0 };
    size_t csr_max_size = 0;

    do
    {
        // Check the pointers
        if (csr_def == NULL || csr == NULL || csr == NULL || csr_size == NULL)
        {
            status = ATCACERT_E_BAD_PARAMS;
            BREAK(status, "Null input parameter");
        }
        // Check the csr buffer size
        if (*csr_size < csr_def->cert_template_size)
        {
            status = ATCACERT_E_BUFFER_TOO_SMALL;
            BREAK(status, "CSR buffer size too small");
        }
        // Copy the CSR template into the CSR that will be returned
        memcpy(csr, csr_def->cert_template, csr_def->cert_template_size);
        csr_max_size = *csr_size;
        *csr_size = csr_def->cert_template_size;

        // Get a few elements from the csr_def structure
        pub_loc = &(csr_def->std_cert_elements[STDCERT_PUBLIC_KEY]);
        pub_dev_loc = &(csr_def->public_key_dev_loc);
        key_slot = pub_dev_loc->slot;
        priv_key_slot = csr_def->private_key_slot;

        // Get the public key from the device
        if (pub_dev_loc->is_genkey)
        {
            // Calculate the public key from the private key
            status = atcab_get_pubkey(key_slot, pub_key);
            if (status != ATCA_SUCCESS)
            {
                BREAK(status, "Could not generate public key");
            }
        }
        else
        {
            // Read the public key from a slot
            status = atcab_read_pubkey(key_slot, pub_key);
            if (status != ATCA_SUCCESS)
            {
                BREAK(status, "Could not read public key");
            }
        }
        // Insert the public key into the CSR template
        status = atcacert_set_cert_element(csr_def, pub_loc, csr, *csr_size, pub_key, ATCA_PUB_KEY_SIZE);
        if (status != ATCA_SUCCESS)
        {
            BREAK(status, "Setting CSR public key failed");
        }

        // Get the CSR TBS digest
        status = atcacert_get_tbs_digest(csr_def, csr, *csr_size, tbs_digest);
        if (status != ATCA_SUCCESS)
        {
            BREAK(status, "Get TBS digest failed");
        }

        // Sign the TBS digest
        status = atcab_sign(priv_key_slot, tbs_digest, sig);
        if (status != ATCA_SUCCESS)
        {
            BREAK(status, "Signing CSR failed");
        }

        // Insert the signature into the CSR template
        status = atcacert_set_signature(csr_def, csr, csr_size, csr_max_size, sig);
        if (status != ATCA_SUCCESS)
        {
            BREAK(status, "Setting CSR signature failed");
        }

        // The exact size of the csr cannot be determined until after adding the signature
        // it is returned in the csr_size parameter.  (*csr_size = *csr_size;)

    }
    while (false);

    return status;
}


