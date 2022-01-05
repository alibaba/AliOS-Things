/**
 * \file
 * \brief  Contains API for performing the symmetric Authentication between the Host and the device
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


#include "cryptoauthlib.h"
#include "host/atca_host.h"
#include "symmetric_authentication.h"




/** \brief Function which does the authentication between the host and device.
 *  \param[in]  slot                The slot number used for the symmetric authentication.
 *  \param[in]  master_key          The master key used for the calculating the symmetric key.
 *  \param[in]  rand_number         The 20 byte rand_number from the host.
 *  \return ATCA_SUCCESS on successful authentication, otherwise an error code.
 */

ATCA_STATUS symmetric_authenticate(uint8_t slot, const uint8_t *master_key, const uint8_t *rand_number)
{
    ATCA_STATUS status;
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];
    uint8_t symmetric_key[ATCA_KEY_SIZE];
    atca_temp_key_t temp_key, temp_key_derive;
    uint8_t rand_out[RANDOM_NUM_SIZE];
    atca_nonce_in_out_t nonce_params;
    atca_mac_in_out_t mac_params;
    uint8_t host_mac[MAC_SIZE];
    uint8_t device_mac[MAC_SIZE];
    struct atca_derive_key_in_out derivekey_params;

    do
    {
        // Read serial number for host-side MAC calculations
        if ((status = atcab_read_serial_number(sn)) != ATCA_SUCCESS)
        {
            break;
        }

        // Setup nonce command
        memset(&temp_key, 0, sizeof(temp_key));
        memset(&nonce_params, 0, sizeof(nonce_params));
        nonce_params.mode = NONCE_MODE_SEED_UPDATE;
        nonce_params.zero = 0;
        nonce_params.num_in = rand_number;
        nonce_params.rand_out = rand_out;
        nonce_params.temp_key = &temp_key;

        // Create random nonce
        if ((status = atcab_nonce_rand(nonce_params.num_in, rand_out)) != ATCA_SUCCESS)
        {
            break;
        }

        // Calculate nonce in host
        if ((status = atcah_nonce(&nonce_params)) != ATCA_SUCCESS)
        {
            break;
        }

        memset(&temp_key_derive, 0, sizeof(temp_key_derive));
        temp_key_derive.valid = 1;
        memcpy(temp_key_derive.value, sn, sizeof(sn)); // 32 bytes TempKey ( SN[0:8] with padded 23 zeros used in symmetric key calculation)

        // Parameters used deriving the symmetric key
        derivekey_params.mode = 0;
        derivekey_params.target_key_id = slot;
        derivekey_params.parent_key = master_key;
        derivekey_params.sn = sn;
        derivekey_params.target_key = symmetric_key;
        derivekey_params.temp_key = &temp_key_derive;

        // calculate the symmetric_diversified_key
        if ((status = atcah_derive_key(&derivekey_params)) != ATCA_SUCCESS)
        {
            break;
        }

        // Setup MAC command
        memset(&mac_params, 0, sizeof(mac_params));
        mac_params.mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN; // Block 1 is a key, block 2 is TempKey
        mac_params.key_id = slot;
        mac_params.challenge = NULL;
        mac_params.key = symmetric_key;
        mac_params.otp = NULL;
        mac_params.sn = sn;
        mac_params.response = host_mac;
        mac_params.temp_key = &temp_key;

        // Generate the MAC command from the device
        if ((status = atcab_mac(mac_params.mode, mac_params.key_id, mac_params.challenge, device_mac)) != ATCA_SUCCESS)
        {
            break;
        }

        // Calculate the MAC on the host side
        if (( status = atcah_mac(&mac_params)) != ATCA_SUCCESS)
        {
            break;
        }

        //Check whether the MAC calculated on host is same as that generated from the device
        if (memcmp(device_mac, host_mac, 32) != 0)
        {
            status = ATCA_CHECKMAC_VERIFY_FAILED;
        }

    }
    while (0);

    return status;
}