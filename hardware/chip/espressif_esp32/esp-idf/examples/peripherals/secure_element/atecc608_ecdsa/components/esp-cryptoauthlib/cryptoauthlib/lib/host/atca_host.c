/**
 * \file
 * \brief Host side methods to support CryptoAuth computations
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

#include "atca_host.h"
#include "crypto/atca_crypto_sw_sha2.h"


/** \brief This function copies otp and sn data into a command buffer.
 *
 * \param[in, out] param pointer to parameter structure
 * \return pointer to command buffer byte that was copied last
 */
uint8_t *atcah_include_data(struct atca_include_data_in_out *param)
{
    if (param->mode & MAC_MODE_INCLUDE_OTP_88)
    {
        memcpy(param->p_temp, param->otp, 11);            // use OTP[0:10], Mode:5 is overridden
        param->p_temp += 11;
    }
    else
    {
        if (param->mode & MAC_MODE_INCLUDE_OTP_64)
        {
            memcpy(param->p_temp, param->otp, 8);         // use 8 bytes OTP[0:7] for (6)
        }
        else
        {
            memset(param->p_temp, 0, 8);                  // use 8 zeros for (6)
        }
        param->p_temp += 8;

        memset(param->p_temp, 0, 3);                     // use 3 zeros for (7)
        param->p_temp += 3;
    }

    // (8) 1 byte SN[8]
    *param->p_temp++ = param->sn[8];

    // (9) 4 bytes SN[4:7] or zeros
    if (param->mode & MAC_MODE_INCLUDE_SN)
    {
        memcpy(param->p_temp, &param->sn[4], 4);           //use SN[4:7] for (9)
    }
    else
    {
        memset(param->p_temp, 0, 4);                       //use zeros for (9)
    }
    param->p_temp += 4;

    // (10) 2 bytes SN[0:1]
    *param->p_temp++ = param->sn[0];
    *param->p_temp++ = param->sn[1];

    // (11) 2 bytes SN[2:3] or zeros
    if (param->mode & MAC_MODE_INCLUDE_SN)
    {
        memcpy(param->p_temp, &param->sn[2], 2);           //use SN[2:3] for (11)
    }
    else
    {
        memset(param->p_temp, 0, 2);                       //use zeros for (9)
    }
    param->p_temp += 2;

    return param->p_temp;
}

/** \brief This function calculates host side nonce with the parameters passed.
 *    \param[in, out] param pointer to parameter structure
 *   \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_nonce(struct atca_nonce_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_NONCE];
    uint8_t *p_temp;
    uint8_t calc_mode = param->mode & NONCE_MODE_MASK;

    // Check parameters
    if (param->temp_key == NULL || param->num_in == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Calculate or pass-through the nonce to TempKey->Value
    if ((calc_mode == NONCE_MODE_SEED_UPDATE) || (calc_mode == NONCE_MODE_NO_SEED_UPDATE))
    {
        // RandOut is only required for these modes
        if (param->rand_out == NULL)
        {
            return ATCA_BAD_PARAM;
        }

        if ((param->zero & NONCE_ZERO_CALC_MASK) == NONCE_ZERO_CALC_TEMPKEY)
        {
            // Nonce calculation mode. Actual value of TempKey has been returned in RandOut
            memcpy(param->temp_key->value, param->rand_out, 32);

            // TempKey flags aren't changed
        }
        else
        {
            // Calculate nonce using SHA-256 (refer to data sheet)
            p_temp = temporary;

            memcpy(p_temp, param->rand_out, RANDOM_NUM_SIZE);
            p_temp += RANDOM_NUM_SIZE;

            memcpy(p_temp, param->num_in, NONCE_NUMIN_SIZE);
            p_temp += NONCE_NUMIN_SIZE;

            *p_temp++ = ATCA_NONCE;
            *p_temp++ = param->mode;
            *p_temp++ = 0x00;

            // Calculate SHA256 to get the nonce
            atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_NONCE, param->temp_key->value);

            // Update TempKey flags
            param->temp_key->source_flag = 0; // Random
            param->temp_key->key_id = 0;
            param->temp_key->gen_dig_data = 0;
            param->temp_key->no_mac_flag = 0;
            param->temp_key->valid = 1;
        }

        // Update TempKey to only 32 bytes
        param->temp_key->is_64 = 0;
    }
    else if ((param->mode & NONCE_MODE_MASK) == NONCE_MODE_PASSTHROUGH)
    {

        if ((param->mode & NONCE_MODE_TARGET_MASK) == NONCE_MODE_TARGET_TEMPKEY)
        {
            // Pass-through mode for TempKey (other targets have no effect on TempKey)
            if ((param->mode & NONCE_MODE_INPUT_LEN_MASK) == NONCE_MODE_INPUT_LEN_64)
            {
                memcpy(param->temp_key->value, param->num_in, 64);
                param->temp_key->is_64 = 1;
            }
            else
            {
                memcpy(param->temp_key->value, param->num_in, 32);
                param->temp_key->is_64 = 0;
            }

            // Update TempKey flags
            param->temp_key->source_flag = 1; // Not Random
            param->temp_key->key_id = 0;
            param->temp_key->gen_dig_data = 0;
            param->temp_key->no_mac_flag = 0;
            param->temp_key->valid = 1;
        }
        else //In the case of ECC608A, passthrough message may be stored in message digest buffer/ Alternate Key buffer
        {

            // Update TempKey flags
            param->temp_key->source_flag = 1; //Not Random
            param->temp_key->key_id = 0;
            param->temp_key->gen_dig_data = 0;
            param->temp_key->no_mac_flag = 0;
            param->temp_key->valid = 0;

        }
    }
    else
    {
        return ATCA_BAD_PARAM;
    }

    return ATCA_SUCCESS;
}

/** \brief Decrypt data that's been encrypted by the IO protection key.
 *          The ECDH and KDF commands on the ATECC608A are the only ones that
 *          support this operation.
 *
 *    \param[inout] param  Parameters required to perform the operation.
 *
 *   \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_io_decrypt(struct atca_io_decrypt_in_out *param)
{
    atcac_sha2_256_ctx ctx;
    uint8_t key[ATCA_KEY_SIZE];
    size_t block = 0;
    int i;

    if (param == NULL || param->io_key == NULL || param->out_nonce == NULL || param->data == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if (param->data_size % ATCA_BLOCK_SIZE != 0)
    {
        return ATCA_BAD_PARAM;
    }

    for (block = 0; block < param->data_size / ATCA_BLOCK_SIZE; block++)
    {
        // Calculate key for block
        atcac_sw_sha2_256_init(&ctx);
        atcac_sw_sha2_256_update(&ctx, param->io_key, 32);
        atcac_sw_sha2_256_update(&ctx, &param->out_nonce[block * 16], 16);
        atcac_sw_sha2_256_finish(&ctx, key);

        // Decrypt block
        for (i = 0; i < ATCA_BLOCK_SIZE; i++)
        {
            param->data[block * ATCA_BLOCK_SIZE + i] ^= key[i];
        }
    }

    return ATCA_SUCCESS;
}


/** \brief Calculate the expected MAC on the host side for the Verify command.
 *
 * \param[inout] param  Data required to perform the operation.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_verify_mac(atca_verify_mac_in_out_t *param)
{
    uint8_t verify_mode = (param->mode & VERIFY_MODE_MASK);
    uint8_t verify_source = (param->mode & VERIFY_MODE_SOURCE_MASK);
    atcac_sha2_256_ctx ctx;
    uint8_t message[32];
    const uint8_t* nonce = NULL;
    uint8_t input_params[4];
    const uint8_t sign_opcode = ATCA_SIGN;

    // Check parameters
    if (param->signature == NULL || param->msg_dig_buf == NULL || param->io_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Get the verify message
    if (verify_mode == VERIFY_MODE_VALIDATE || verify_mode == VERIFY_MODE_INVALIDATE)
    {
        if (param->other_data == NULL || param->temp_key == NULL || param->sn == NULL)
        {
            return ATCA_BAD_PARAM;
        }

        // Message is calculated based on TempKey and OtherData
        atcac_sw_sha2_256_init(&ctx);
        atcac_sw_sha2_256_update(&ctx, param->temp_key->value, 32);
        atcac_sw_sha2_256_update(&ctx, &sign_opcode, 1);
        atcac_sw_sha2_256_update(&ctx, &param->other_data[0], 10);
        atcac_sw_sha2_256_update(&ctx, &param->sn[8], 1);
        atcac_sw_sha2_256_update(&ctx, &param->other_data[10], 4);
        atcac_sw_sha2_256_update(&ctx, &param->sn[0], 2);
        atcac_sw_sha2_256_update(&ctx, &param->other_data[14], 5);
        atcac_sw_sha2_256_finish(&ctx, message);
    }
    else if (verify_source == VERIFY_MODE_SOURCE_MSGDIGBUF)
    {
        // Message source is the first 32 bytes of the message digest buffer
        memcpy(message, param->msg_dig_buf, 32);
    }
    else
    {
        // Message source is the first 32 bytes of TempKey
        if (param->temp_key == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        memcpy(message, param->temp_key->value, 32);
    }

    // Get the system nonce
    if (verify_source == VERIFY_MODE_SOURCE_MSGDIGBUF)
    {
        nonce = &param->msg_dig_buf[32];  // System nonce is the second 32 bytes of the message digest buffer
    }
    else
    {
        nonce = &param->msg_dig_buf[0];   // System nonce is the first 32 bytes of the message digest buffer

    }
    // Calculate MAC
    atcac_sw_sha2_256_init(&ctx);
    atcac_sw_sha2_256_update(&ctx, param->io_key, ATCA_KEY_SIZE);  // IO protection key
    atcac_sw_sha2_256_update(&ctx, message, 32);                   // Verify message
    atcac_sw_sha2_256_update(&ctx, nonce, 32);                     // Host (system) nonce
    atcac_sw_sha2_256_update(&ctx, param->signature, 64);          // Signature

    // Add Verify input parameters
    input_params[0] = ATCA_VERIFY;                   // Verify Opcode
    input_params[1] = param->mode;                   // Verify Mode (Param1)
    input_params[2] = (uint8_t)(param->key_id >> 0); // Verify Param2 (LSB)
    input_params[3] = (uint8_t)(param->key_id >> 8); // Verify Param2 (MSB)
    atcac_sw_sha2_256_update(&ctx, input_params, sizeof(input_params));

    // Calculate SHA256 to get mac
    atcac_sw_sha2_256_finish(&ctx, param->mac);

    return ATCA_SUCCESS;
}

/** \brief Encrypts the digest for the SecureBoot command when using the
 *          encrypted digest / validating mac option.
 *
 * \param[inout] param  Data required to perform the operation.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_secureboot_enc(atca_secureboot_enc_in_out_t* param)
{
    atcac_sha2_256_ctx ctx;
    size_t i;

    // Check parameters
    if (param->digest == NULL || param->temp_key == NULL || param->hashed_key == NULL || param->io_key == NULL || param->digest_enc == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Calculate key for encrypting digest
    atcac_sw_sha2_256_init(&ctx);
    atcac_sw_sha2_256_update(&ctx, param->io_key, ATCA_KEY_SIZE);
    atcac_sw_sha2_256_update(&ctx, param->temp_key->value, ATCA_KEY_SIZE);
    atcac_sw_sha2_256_finish(&ctx, param->hashed_key);

    // Encrypt digest (XOR with key)
    for (i = 0; i < SECUREBOOT_DIGEST_SIZE; i++)
    {
        param->digest_enc[i] = param->digest[i] ^ param->hashed_key[i];
    }

    return ATCA_SUCCESS;
}

/** \brief Calculates the expected MAC returned from the SecureBoot command
 *          when verification is a success.
 *
 * The result of this function (param->mac) should be compared with the actual
 * MAC returned to validate the response.
 *
 * \param[inout] param  Data required to perform the operation.
 *
 *   \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_secureboot_mac(atca_secureboot_mac_in_out_t *param)
{
    atcac_sha2_256_ctx ctx;
    uint8_t input_params[4];

    if (param->hashed_key == NULL || param->digest == NULL || param->mac == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Calculate MAC
    atcac_sw_sha2_256_init(&ctx);
    atcac_sw_sha2_256_update(&ctx, param->hashed_key, ATCA_KEY_SIZE);
    atcac_sw_sha2_256_update(&ctx, param->digest, SECUREBOOT_DIGEST_SIZE);

    // Signature is only skipped when running the SecureBoot command in
    // FullStore mode and SecureBootMode from the configuration zone is set to
    // FullDig
    if (!((param->mode & SECUREBOOT_MODE_MASK) == SECUREBOOT_MODE_FULL_STORE &&
          (param->secure_boot_config & SECUREBOOTCONFIG_MODE_MASK) == SECUREBOOTCONFIG_MODE_FULL_DIG))
    {
        if (param->signature == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        atcac_sw_sha2_256_update(&ctx, param->signature, SECUREBOOT_SIGNATURE_SIZE);
    }

    // Add SecureBoot input parameters
    input_params[0] = ATCA_SECUREBOOT;               // SecureBoot Opcode
    input_params[1] = param->mode;                   // SecureBoot Mode (Param1)
    input_params[2] = (uint8_t)(param->param2 >> 0); // SecureBoot Param2 (LSB)
    input_params[3] = (uint8_t)(param->param2 >> 8); // SecureBoot Param2 (MSB)
    atcac_sw_sha2_256_update(&ctx, input_params, sizeof(input_params));

    atcac_sw_sha2_256_finish(&ctx, param->mac);

    return ATCA_SUCCESS;
}



/** \brief This function generates an SHA-256 digest (MAC) of a key, challenge, and other information.

   The resulting digest will match with the one generated by the device when executing a MAC command.
   The TempKey (if used) should be valid (temp_key.valid = 1) before executing this function.

 * \param[in, out] param pointer to parameter structure
 *   \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_mac(struct atca_mac_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_MAC];
    uint8_t *p_temp;
    struct atca_include_data_in_out include_data;

    // Initialize struct
    include_data.otp = param->otp;
    include_data.sn = param->sn;
    include_data.mode = param->mode;

    // Check parameters
    if (!param->response
        || (param->mode & ~MAC_MODE_MASK)
        || (!(param->mode & MAC_MODE_BLOCK1_TEMPKEY) && !param->key)
        || (!(param->mode & MAC_MODE_BLOCK2_TEMPKEY) && !param->challenge)
        || ((param->mode & MAC_MODE_USE_TEMPKEY_MASK) && !param->temp_key)
        || (((param->mode & MAC_MODE_INCLUDE_OTP_64) || (param->mode & MAC_MODE_INCLUDE_OTP_88)) && !param->otp)
        || ((param->mode & MAC_MODE_INCLUDE_SN) && !param->sn)
        )
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity if TempKey is used
    if (((param->mode & MAC_MODE_USE_TEMPKEY_MASK) != 0)
        // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        && (param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        // If either mode parameter bit 0 or bit 1 are set, mode parameter bit 2 must match temp_key.source_flag.
        // Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
            || (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag)))
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start calculation
    p_temp = temporary;

    // (1) first 32 bytes
    memcpy(p_temp, param->mode & MAC_MODE_BLOCK1_TEMPKEY ? param->temp_key->value : param->key, ATCA_KEY_SIZE);                // use Key[KeyID]
    p_temp += ATCA_KEY_SIZE;

    // (2) second 32 bytes
    memcpy(p_temp, param->mode & MAC_MODE_BLOCK2_TEMPKEY ? param->temp_key->value : param->challenge, ATCA_KEY_SIZE);          // use Key[KeyID]
    p_temp += ATCA_KEY_SIZE;

    // (3) 1 byte opcode
    *p_temp++ = ATCA_MAC;

    // (4) 1 byte mode parameter
    *p_temp++ = param->mode;

    // (5) 2 bytes keyID
    *p_temp++ = param->key_id & 0xFF;
    *p_temp++ = (param->key_id >> 8) & 0xFF;

    include_data.p_temp = p_temp;
    atcah_include_data(&include_data);

    // Calculate SHA256 to get the MAC digest
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_MAC, param->response);

    // Update TempKey fields
    if (param->temp_key)
    {
        param->temp_key->valid = 0;
    }

    return ATCA_SUCCESS;
}




/** \brief This function performs the checkmac operation to generate client response on the host side .
 * \param[inout] param  Input and output parameters
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_check_mac(struct atca_check_mac_in_out *param)
{
    uint8_t msg[ATCA_MSG_SIZE_MAC];
    bool is_temp_key_req = false;

    // Check parameters
    if (param == NULL || param->other_data == NULL || param->sn == NULL || param->client_resp == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if ((param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) || (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY))
    {
        is_temp_key_req = true;  // Message uses TempKey
    }
    else if ((param->mode == 0x01 || param->mode == 0x05) && param->target_key != NULL)
    {
        is_temp_key_req = true;  // CheckMac copy will be performed

    }
    if (is_temp_key_req && param->temp_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if (!(param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) && param->slot_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if (!(param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY) && param->client_chal == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if ((param->mode & CHECKMAC_MODE_INCLUDE_OTP_64) && param->otp == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    if ((param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY) || (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY))
    {
        // This will use TempKey in message, check validity
        if (!param->temp_key->valid)
        {
            return ATCA_EXECUTION_ERROR;  // TempKey is not valid
        }
        if (((param->mode >> 2) & 0x01) != param->temp_key->source_flag)
        {
            return ATCA_EXECUTION_ERROR;  // TempKey SourceFlag doesn't match bit 2 of the mode
        }
    }

    // Build the message
    memset(msg, 0, sizeof(msg));
    if (param->mode & CHECKMAC_MODE_BLOCK1_TEMPKEY)
    {
        memcpy(&msg[0], param->temp_key->value, 32);
    }
    else
    {
        memcpy(&msg[0], param->slot_key, 32);
    }
    if (param->mode & CHECKMAC_MODE_BLOCK2_TEMPKEY)
    {
        memcpy(&msg[32], param->temp_key->value, 32);
    }
    else
    {
        memcpy(&msg[32], param->client_chal, 32);
    }
    memcpy(&msg[64], &param->other_data[0], 4);
    if (param->mode & CHECKMAC_MODE_INCLUDE_OTP_64)
    {
        memcpy(&msg[68], param->otp, 8);
    }
    memcpy(&msg[76], &param->other_data[4], 3);
    msg[79] = param->sn[8];
    memcpy(&msg[80], &param->other_data[7], 4);
    memcpy(&msg[84], &param->sn[0], 2);
    memcpy(&msg[86], &param->other_data[11], 2);

    // Calculate the client response
    atcac_sw_sha2_256(msg, sizeof(msg), param->client_resp);

    // Update TempKey fields
    if ((param->mode == 0x01 || param->mode == 0x05) && param->target_key != NULL)
    {
        // CheckMac Copy will be performed
        memcpy(param->temp_key->value, param->target_key, ATCA_KEY_SIZE);
        param->temp_key->gen_dig_data = 0;
        param->temp_key->source_flag = 1;
        param->temp_key->valid = 1;
    }

    return ATCA_SUCCESS;
}


/** \brief This function generates an HMAC / SHA-256 hash of a key and other information.

   The resulting hash will match with the one generated in the device by an HMAC command.
   The TempKey has to be valid (temp_key.valid = 1) before executing this function.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_hmac(struct atca_hmac_in_out *param)
{
    // Local Variables
    struct atca_include_data_in_out include_data;
    uint8_t temporary[HMAC_BLOCK_SIZE + ATCA_MSG_SIZE_HMAC];
    uint8_t i = 0;
    uint8_t *p_temp = NULL;

    // Check parameters
    if (!param->response || !param->key || !param->temp_key
        || (param->mode & ~HMAC_MODE_MASK)
        || (((param->mode & MAC_MODE_INCLUDE_OTP_64) || (param->mode & MAC_MODE_INCLUDE_OTP_88)) && !param->otp)
        || (!param->sn)
        )
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        // The mode parameter bit 2 must match temp_key.source_flag.
        // Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
        || (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag))
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start first calculation (inner)
    p_temp = temporary;

    // XOR key with ipad
    for (i = 0; i < ATCA_KEY_SIZE; i++)
    {
        *p_temp++ = param->key[i] ^ 0x36;
    }

    // zero pad key out to block size
    // Refer to fips-198 , length Key = 32 bytes, Block size = 512 bits = 64 bytes.
    // So the Key must be padded with zeros.
    memset(p_temp, 0x36, HMAC_BLOCK_SIZE - ATCA_KEY_SIZE);
    p_temp += HMAC_BLOCK_SIZE - ATCA_KEY_SIZE;

    // Next append the stream of data 'text'
    memset(p_temp, 0, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    *p_temp++ = ATCA_HMAC;
    *p_temp++ = param->mode;
    *p_temp++ = (uint8_t)(param->key_id >> 0);
    *p_temp++ = (uint8_t)(param->key_id >> 8);

    include_data.otp = param->otp;
    include_data.sn = param->sn;
    include_data.mode = param->mode;
    include_data.p_temp = p_temp;
    atcah_include_data(&include_data);

    // Calculate SHA256
    // H((K0^ipad):text), use param.response for temporary storage
    atcac_sw_sha2_256(temporary, HMAC_BLOCK_SIZE + ATCA_MSG_SIZE_HMAC, param->response);


    // Start second calculation (outer)
    p_temp = temporary;

    // XOR K0 with opad
    for (i = 0; i < ATCA_KEY_SIZE; i++)
    {
        *p_temp++ = param->key[i] ^ 0x5C;
    }

    // zero pad key out to block size
    // Refer to fips-198 , length Key = 32 bytes, Block size = 512 bits = 64 bytes.
    // So the Key must be padded with zeros.
    memset(p_temp, 0x5C, HMAC_BLOCK_SIZE - ATCA_KEY_SIZE);
    p_temp += HMAC_BLOCK_SIZE - ATCA_KEY_SIZE;

    // Append result from last calculation H((K0 ^ ipad) || text)
    memcpy(p_temp, param->response, ATCA_SHA_DIGEST_SIZE);
    p_temp += ATCA_SHA_DIGEST_SIZE;

    // Calculate SHA256 to get the resulting HMAC
    atcac_sw_sha2_256(temporary, HMAC_BLOCK_SIZE + ATCA_SHA_DIGEST_SIZE, param->response);

    // Update TempKey fields
    param->temp_key->valid = 0;

    return ATCA_SUCCESS;
}


/** \brief This function combines the current TempKey with a stored value.

   The stored value can be a data slot, OTP page, configuration zone, or hardware transport key.
   The TempKey generated by this function will match with the TempKey in the device generated
   when executing a GenDig command.
   The TempKey should be valid (temp_key.valid = 1) before executing this function.
   To use this function, an application first sends a GenDig command with a chosen stored value to the device.
   This stored value must be known by the application and is passed to this GenDig calculation function.
   The function calculates a new TempKey and returns it.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_gen_dig(struct atca_gen_dig_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_GEN_DIG];
    uint8_t *p_temp;

    // Check parameters
    if (param->sn == NULL || param->temp_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if ((param->zone <= GENDIG_ZONE_DATA) && (param->stored_value == NULL))
    {
        return ATCA_BAD_PARAM;  // Stored value cannot be null for Config,OTP and Data
    }

    if ((param->zone == GENDIG_ZONE_SHARED_NONCE || (param->zone == GENDIG_ZONE_DATA && param->is_key_nomac)) && param->other_data == NULL)
    {
        return ATCA_BAD_PARAM;  // Other data is required in these cases
    }

    if (param->zone > 5)
    {
        return ATCA_BAD_PARAM;  // Unknown zone

    }
    // Start calculation
    p_temp = temporary;


    // (1) 32 bytes inputKey
    if (param->zone == GENDIG_ZONE_SHARED_NONCE)
    {
        if (param->key_id & 0x8000)
        {
            memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);  // 32 bytes TempKey
        }
        else
        {
            memcpy(p_temp, param->other_data, ATCA_KEY_SIZE);       // 32 bytes other data

        }
    }
    else if (param->zone == GENDIG_ZONE_COUNTER || param->zone == GENDIG_ZONE_KEY_CONFIG)
    {
        memset(p_temp, 0x00, ATCA_KEY_SIZE);                        // 32 bytes of zero.

    }
    else
    {
        memcpy(p_temp, param->stored_value, ATCA_KEY_SIZE);     // 32 bytes of stored data

    }

    p_temp += ATCA_KEY_SIZE;


    if (param->zone == GENDIG_ZONE_DATA && param->is_key_nomac)
    {
        // If a key has the SlotConfig.NoMac bit set, then opcode and parameters come from OtherData
        memcpy(p_temp, param->other_data, 4);
        p_temp += 4;
    }
    else
    {
        // (2) 1 byte Opcode
        *p_temp++ = ATCA_GENDIG;

        // (3) 1 byte Param1 (zone)
        *p_temp++ = param->zone;

        // (4) 1 byte LSB of Param2 (keyID)
        *p_temp++ = (uint8_t)(param->key_id & 0xFF);
        if (param->zone == GENDIG_ZONE_SHARED_NONCE)
        {
            //(4) 1 byte zero for shared nonce mode
            *p_temp++ = 0;
        }
        else
        {
            //(4)  1 byte MSB of Param2 (keyID) for other modes
            *p_temp++ = (uint8_t)(param->key_id >> 8);
        }
    }

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];


    // (7)
    if (param->zone == GENDIG_ZONE_COUNTER)
    {
        *p_temp++ = 0;
        *p_temp++ = (uint8_t)(param->counter & 0xFF);   // (7) 4 bytes of counter
        *p_temp++ = (uint8_t)(param->counter >> 8);
        *p_temp++ = (uint8_t)(param->counter >> 16);
        *p_temp++ = (uint8_t)(param->counter >> 24);

        memset(p_temp, 0x00, 20);                       // (7) 20 bytes of zero
        p_temp += 20;

    }
    else if (param->zone == GENDIG_ZONE_KEY_CONFIG)
    {
        *p_temp++ = 0;
        *p_temp++ = param->slot_conf & 0xFF;            // (7) 2 bytes of Slot config
        *p_temp++ = (uint8_t)(param->slot_conf >> 8);

        *p_temp++ = param->key_conf & 0xFF;
        *p_temp++ = (uint8_t)(param->key_conf >> 8);   // (7) 2 bytes of key config

        *p_temp++ = param->slot_locked;                // (7) 1 byte of slot locked

        memset(p_temp, 0x00, 19);                      // (7) 19 bytes of zero
        p_temp += 19;


    }
    else
    {

        memset(p_temp, 0, ATCA_GENDIG_ZEROS_SIZE);       // (7) 25 zeros
        p_temp += ATCA_GENDIG_ZEROS_SIZE;

    }




    if (param->zone == GENDIG_ZONE_SHARED_NONCE && (param->key_id & 0x8000))
    {
        memcpy(p_temp, param->other_data, ATCA_KEY_SIZE);           // (8) 32 bytes OtherData
        p_temp += ATCA_KEY_SIZE;

    }
    else
    {
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);      // (8) 32 bytes TempKey
        p_temp += ATCA_KEY_SIZE;

    }





    // Calculate SHA256 to get the new TempKey
    atcac_sw_sha2_256(temporary, (p_temp - temporary), param->temp_key->value);

    // Update TempKey fields
    param->temp_key->valid = 1;

    if ((param->zone == GENDIG_ZONE_DATA) && (param->key_id <= 15))
    {
        param->temp_key->gen_dig_data = 1;
        param->temp_key->key_id = (param->key_id & 0xF);    // mask lower 4-bit only
        if (param->is_key_nomac == 1)
        {
            param->temp_key->no_mac_flag = 1;
        }
    }
    else
    {
        param->temp_key->gen_dig_data = 0;
        param->temp_key->key_id = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief This function generates mac with session key with a plain text.
 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_gen_mac(struct atca_gen_dig_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_GEN_DIG];
    uint8_t *p_temp;

    // Check parameters
    if (!param->stored_value || !param->temp_key)
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start calculation
    p_temp = temporary;

    // (1) 32 bytes SessionKey
    //     (Config[KeyID] or OTP[KeyID] or Data.slot[KeyID] or TransportKey[KeyID])
    memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    // (2) 1 byte Opcode
    *p_temp++ = ATCA_WRITE;

    // (3) 1 byte Param1 (zone)
    *p_temp++ = param->zone;

    // (4) 2 bytes Param2 (keyID)
    *p_temp++ = param->key_id & 0xFF;
    *p_temp++ = (param->key_id >> 8) & 0xFF;

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];

    // (7) 25 zeros
    memset(p_temp, 0, ATCA_GENDIG_ZEROS_SIZE);
    p_temp += ATCA_GENDIG_ZEROS_SIZE;

    // (8) 32 bytes PlainText
    memcpy(p_temp, param->stored_value, ATCA_KEY_SIZE);

    // Calculate SHA256 to get the new TempKey
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_GEN_DIG, param->temp_key->value);

    // Update TempKey fields
    param->temp_key->valid = 1;

    if ((param->zone == GENDIG_ZONE_DATA) && (param->key_id <= 15))
    {
        param->temp_key->gen_dig_data = 1;
        param->temp_key->key_id = (param->key_id & 0xF);    // mask lower 4-bit only
    }
    else
    {
        param->temp_key->gen_dig_data = 0;
        param->temp_key->key_id = 0;
    }

    return ATCA_SUCCESS;
}

/** \brief This function calculates the input MAC for the Write command.

   The Write command will need an input MAC if SlotConfig.WriteConfig.Encrypt is set.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_write_auth_mac(struct atca_write_mac_in_out *param)
{
    uint8_t mac_input[ATCA_MSG_SIZE_ENCRYPT_MAC];
    uint8_t i;
    uint8_t *p_temp;

    // Check parameters
    if (!param->input_data || !param->temp_key)
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }
    // Encrypt by XOR-ing Data with the TempKey
    for (i = 0; i < 32; i++)
    {
        param->encrypted_data[i] = param->input_data[i] ^ param->temp_key->value[i];
    }

    // If the pointer *mac is provided by the caller then calculate input MAC
    if (param->auth_mac)
    {
        // Start calculation
        p_temp = mac_input;

        // (1) 32 bytes TempKey
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;

        // (2) 1 byte Opcode
        *p_temp++ = ATCA_WRITE;

        // (3) 1 byte Param1 (zone)
        *p_temp++ = param->zone;

        // (4) 2 bytes Param2 (keyID)
        *p_temp++ = param->key_id & 0xFF;
        *p_temp++ = (param->key_id >> 8) & 0xFF;

        // (5) 1 byte SN[8]
        *p_temp++ = param->sn[8];

        // (6) 2 bytes SN[0:1]
        *p_temp++ = param->sn[0];
        *p_temp++ = param->sn[1];

        // (7) 25 zeros
        memset(p_temp, 0, ATCA_WRITE_MAC_ZEROS_SIZE);
        p_temp += ATCA_WRITE_MAC_ZEROS_SIZE;

        // (8) 32 bytes PlainText
        memcpy(p_temp, param->input_data, ATCA_KEY_SIZE);

        // Calculate SHA256 to get MAC
        atcac_sw_sha2_256(mac_input, sizeof(mac_input), param->auth_mac);
    }

    return ATCA_SUCCESS;
}

/** \brief This function calculates the input MAC for the PrivWrite command.

   The PrivWrite command will need an input MAC if SlotConfig.WriteConfig.Encrypt is set.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_privwrite_auth_mac(struct atca_write_mac_in_out *param)
{
    uint8_t mac_input[ATCA_MSG_SIZE_PRIVWRITE_MAC];
    uint8_t i = 0;
    uint8_t *p_temp = NULL;
    uint8_t session_key2[32];

    // Check parameters
    if (!param->input_data || !param->temp_key)
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }


    /* Encrypt by XOR-ing Data with the TempKey
     */

    // Encrypt the next 28 bytes of the cipher text, which is the first part of the private key.
    for (i = 0; i < 32; i++)
    {
        param->encrypted_data[i] = param->input_data[i] ^ param->temp_key->value[i];
    }

    // Calculate the new key for the last 4 bytes of the cipher text
    atcac_sw_sha2_256(param->temp_key->value, 32, session_key2);

    // Encrypt the last 4 bytes of the cipher text, which is the remaining part of the private key
    for (i = 32; i < 36; i++)
    {
        param->encrypted_data[i] = param->input_data[i] ^ session_key2[i - 32];
    }

    // If the pointer *mac is provided by the caller then calculate input MAC
    if (param->auth_mac)
    {
        // Start calculation
        p_temp = mac_input;

        // (1) 32 bytes TempKey
        memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
        p_temp += ATCA_KEY_SIZE;

        // (2) 1 byte Opcode
        *p_temp++ = ATCA_PRIVWRITE;

        // (3) 1 byte Param1 (zone)
        *p_temp++ = param->zone;

        // (4) 2 bytes Param2 (keyID)
        *p_temp++ = param->key_id & 0xFF;
        *p_temp++ = (param->key_id >> 8) & 0xFF;

        // (5) 1 byte SN[8]
        *p_temp++ = param->sn[8];

        // (6) 2 bytes SN[0:1]
        *p_temp++ = param->sn[0];
        *p_temp++ = param->sn[1];

        // (7) 21 zeros
        memset(p_temp, 0, ATCA_PRIVWRITE_MAC_ZEROS_SIZE);
        p_temp += ATCA_PRIVWRITE_MAC_ZEROS_SIZE;

        // (8) 36 bytes PlainText (Private Key)
        memcpy(p_temp, param->input_data, ATCA_PRIVWRITE_PLAIN_TEXT_SIZE);

        // Calculate SHA256 to get the new TempKey
        atcac_sw_sha2_256(mac_input, sizeof(mac_input), param->auth_mac);
    }

    return ATCA_SUCCESS;
}

/** \brief This function derives a key with a key and TempKey.

   Used in conjunction with DeriveKey command, the key derived by this function will match the key in the device.
   Two kinds of operation are supported:
   <ul>
   <li>Roll Key operation: target_key and parent_key parameters should be set to point to the same location (TargetKey).</li>
   <li>Create Key operation: target_key should be set to point to TargetKey, parent_key should be set to point to ParentKey.</li>
   </ul>
   After executing this function, the initial value of target_key will be overwritten with the derived key.
   The TempKey should be valid (temp_key.valid = 1) before executing this function.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_derive_key(struct atca_derive_key_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_DERIVE_KEY];
    uint8_t *p_temp;

    // Check parameters
    if (!param->parent_key || !param->target_key || !param->temp_key
        || (param->mode & ~DERIVE_KEY_RANDOM_FLAG) || (param->target_key_id > ATCA_KEY_ID_MAX))
    {
        return ATCA_BAD_PARAM;
    }


    // Check TempKey fields validity (TempKey is always used)
    if ( // TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
        param->temp_key->no_mac_flag || (param->temp_key->valid != 1)
        // The random parameter bit 2 must match temp_key.source_flag
        // Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
        || (!(param->mode & DERIVE_KEY_RANDOM_FLAG) != !(param->temp_key->source_flag))
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Start calculation
    p_temp = temporary;

    // (1) 32 bytes parent key
    memcpy(p_temp, param->parent_key, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    // (2) 1 byte Opcode
    *p_temp++ = ATCA_DERIVE_KEY;

    // (3) 1 byte Param1 (random)
    *p_temp++ = param->mode;

    // (4) 2 bytes Param2 (keyID)
    *p_temp++ = param->target_key_id & 0xFF;
    *p_temp++ = (param->target_key_id >> 8) & 0xFF;

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];

    // (7) 25 zeros
    memset(p_temp, 0, ATCA_DERIVE_KEY_ZEROS_SIZE);
    p_temp += ATCA_DERIVE_KEY_ZEROS_SIZE;

    // (8) 32 bytes TempKey
    memcpy(p_temp, param->temp_key->value, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    // Calculate SHA256 to get the derived key.
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_DERIVE_KEY, param->target_key);

    // Update TempKey fields
    param->temp_key->valid = 0;

    return ATCA_SUCCESS;
}


/** \brief This function calculates the input MAC for a DeriveKey command.

   The DeriveKey command will need an input MAC if SlotConfig[TargetKey].Bit15 is set.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_derive_key_mac(struct atca_derive_key_mac_in_out *param)
{
    uint8_t temporary[ATCA_MSG_SIZE_DERIVE_KEY_MAC];
    uint8_t *p_temp;

    // Check parameters
    if (!param->parent_key || !param->mac || (param->mode & ~DERIVE_KEY_RANDOM_FLAG)
        || (param->target_key_id > ATCA_KEY_ID_MAX))
    {
        return ATCA_BAD_PARAM;
    }

    // Start calculation
    p_temp = temporary;

    // (1) 32 bytes parent key
    memcpy(p_temp, param->parent_key, ATCA_KEY_SIZE);
    p_temp += ATCA_KEY_SIZE;

    // (2) 1 byte Opcode
    *p_temp++ = ATCA_DERIVE_KEY;

    // (3) 1 byte Param1 (random)
    *p_temp++ = param->mode;

    // (4) 2 bytes Param2 (keyID)
    *p_temp++ = param->target_key_id & 0xFF;
    *p_temp++ = (param->target_key_id >> 8) & 0xFF;

    // (5) 1 byte SN[8]
    *p_temp++ = param->sn[8];

    // (6) 2 bytes SN[0:1]
    *p_temp++ = param->sn[0];
    *p_temp++ = param->sn[1];

    // Calculate SHA256 to get the input MAC for DeriveKey command
    atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_DERIVE_KEY_MAC, param->mac);

    return ATCA_SUCCESS;
}


/** \brief This function decrypts 32-byte encrypted data received with the Read command.

   To use this function, first the nonce must be valid and synchronized between device and application.
   The application sends a GenDig command to the Device, using a key specified by SlotConfig.ReadKey.
   The device updates its TempKey.
   The application then updates its own TempKey using the GenDig calculation function, using the same key.
   The application sends a Read command to the device for a user zone configured with EncryptRead.
   The device encrypts 32-byte zone content, and outputs it to the host.
   The application passes these encrypted data to this decryption function. The function decrypts the data and returns them.
   TempKey must be updated by GenDig using a ParentKey as specified by SlotConfig.ReadKey before executing this function.
   The decryption function does not check whether the TempKey has been generated by a correct ParentKey for the corresponding zone.
   Therefore to get a correct result, the application has to make sure that prior GenDig calculation was done using correct ParentKey.

 * \param[in, out] param pointer to parameter structure
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_decrypt(struct atca_decrypt_in_out *param)
{
    uint8_t i;

    // Check parameters
    if (!param->crypto_data || !param->temp_key)
    {
        return ATCA_BAD_PARAM;
    }

    // Check TempKey fields validity
    // Note that if temp_key.key_id is not checked,
    // we cannot make sure if the key used in previous GenDig IS equal to
    // the key pointed by SlotConfig.ReadKey in the device.
    if ( // TempKey.CheckFlag must be 0
        param->temp_key->no_mac_flag
        // TempKey.Valid must be 1
        || (param->temp_key->valid != 1)
        // TempKey.GenData must be 1
        || (param->temp_key->gen_dig_data != 1)
        // TempKey.SourceFlag must be 0 (random)
        || param->temp_key->source_flag
        )
    {
        // Invalidate TempKey, then return
        param->temp_key->valid = 0;
        return ATCA_EXECUTION_ERROR;
    }

    // Decrypt by XOR-ing Data with the TempKey
    for (i = 0; i < ATCA_KEY_SIZE; i++)
    {
        param->crypto_data[i] ^= param->temp_key->value[i];
    }

    // Update TempKey fields
    param->temp_key->valid = 0;

    return ATCA_SUCCESS;
}

/** \brief This function creates a SHA256 digest on a little-endian system.
 *
 * \param[in] len byte length of message
 * \param[in] message pointer to message
 * \param[out] digest SHA256 of message
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_sha256(int32_t len, const uint8_t *message, uint8_t *digest)
{
    return atcac_sw_sha2_256(message, len, digest);
}

/** \brief Calculate the PubKey digest created by GenKey and saved to TempKey.
 *
 * \param[inout] param  GenKey parameters required to calculate the PubKey
 *                        digest. Digest is return in the temp_key parameter.
 *
   \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_gen_key_msg(struct atca_gen_key_in_out *param)
{
    uint8_t msg[128];

    if (param == NULL || param->public_key == NULL || param->sn == NULL || param->temp_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if (param->public_key_size == 0 || param->public_key_size > 88)
    {
        return ATCA_BAD_PARAM;
    }

    memset(msg, 0, sizeof(msg));
    memcpy(&msg[0], param->temp_key->value, 32);
    msg[32] = ATCA_GENKEY;

    if (param->mode & GENKEY_MODE_PUBKEY_DIGEST)
    {
        // Calculate PubKey digest of stored public key, takes priority over other bits
        if (param->other_data == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        memcpy(&msg[33], param->other_data, 3); // OtherData replaces mode and key_id in message
    }
    else if (param->mode & GENKEY_MODE_DIGEST)
    {
        msg[33] = param->mode;
        msg[34] = (uint8_t)(param->key_id >> 0);
        msg[35] = (uint8_t)(param->key_id >> 8);
    }
    else
    {
        // Mode indicates no PubKey digest was requested.
        // No change to TempKey.
        return ATCA_SUCCESS;
    }

    msg[36] = param->sn[8];
    memcpy(&msg[37], &param->sn[0], 2);

    // Copy public key into end of message
    memcpy(&msg[sizeof(msg) - param->public_key_size], param->public_key, param->public_key_size);

    atcac_sw_sha2_256(msg, sizeof(msg), param->temp_key->value);
    param->temp_key->gen_dig_data = 0;
    param->temp_key->gen_key_data = 1;
    param->temp_key->key_id = param->key_id;

    return ATCA_SUCCESS;
}

/** \brief Populate the slot_config, key_config, and is_slot_locked fields in
 *         the atca_sign_internal_in_out structure from the provided config
 *         zone.
 *
 * The atca_sign_internal_in_out structure has a number of fields
 * (slot_config, key_config, is_slot_locked) that can be determined
 * automatically from the current state of TempKey and the full config
 * zone.
 *
 * \param[inout] param          Sign(Internal) parameters to be filled out. Only
 *                              slot_config, key_config, and is_slot_locked will be
 *                              set.
 * \param[in]    device_type    The type of the device.
 * \param[in]    config         Full 128 byte config zone for the device.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_config_to_sign_internal(ATCADeviceType device_type, struct atca_sign_internal_in_out *param, const uint8_t* config)
{
    const uint8_t* value = NULL;
    uint16_t slot_locked = 0;

    if (param == NULL || config == NULL || param->temp_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // SlotConfig[TempKeyFlags.keyId]
    value = &config[20 + param->temp_key->key_id * 2];
    param->slot_config = (uint16_t)value[0] | ((uint16_t)value[1] << 8);

    // KeyConfig[TempKeyFlags.keyId]
    value = &config[96 + param->temp_key->key_id * 2];
    param->key_config = (uint16_t)value[0] | ((uint16_t)value[1] << 8);

    if (device_type == ATECC108A && param->temp_key->key_id < 8)
    {
        value = &config[52 + param->temp_key->key_id * 2];
        param->use_flag = value[0];
        param->update_count = value[1];
    }
    else
    {
        param->use_flag = 0x00;
        param->update_count = 0x00;
    }

    //SlotLocked:TempKeyFlags.keyId
    slot_locked = (uint16_t)config[88] | ((uint16_t)config[89] << 8);
    param->is_slot_locked = (slot_locked & (1 << param->temp_key->key_id)) ? false : true;

    return ATCA_SUCCESS;
}

/** \brief Builds the full message that would be signed by the Sign(Internal)
 *         command.
 *
 * Additionally, the function will optionally output the OtherData data
 * required by the Verify(In/Validate) command as well as the SHA256 digest of
 * the full message.
 *
 * \param[out] device_type  Device type to perform the calculation for.
 * \param[out] param        Input data and output buffers required.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_sign_internal_msg(ATCADeviceType device_type, struct atca_sign_internal_in_out *param)
{
    uint8_t msg[55];

    if (param == NULL || param->temp_key == NULL || param->sn == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    memset(msg, 0, sizeof(msg));
    memcpy(&msg[0], param->temp_key->value, 32);
    msg[32] = ATCA_SIGN;                          // Sign OpCode
    msg[33] = param->mode;                        // Sign Mode
    msg[34] = (uint8_t)(param->key_id >> 0);      // Sign KeyID
    msg[35] = (uint8_t)(param->key_id >> 8);
    msg[36] = (uint8_t)(param->slot_config >> 0); // SlotConfig[TempKeyFlags.keyId]
    msg[37] = (uint8_t)(param->slot_config >> 8);
    msg[38] = (uint8_t)(param->key_config >> 0);  // KeyConfig[TempKeyFlags.keyId]
    msg[39] = (uint8_t)(param->key_config >> 8);

    //TempKeyFlags (b0-3: keyId, b4: sourceFlag, b5: GenDigData, b6: GenKeyData, b7: NoMacFlag)
    msg[40] |= ((param->temp_key->key_id & 0x0F) << 0);
    msg[40] |= ((param->temp_key->source_flag & 0x01) << 4);
    msg[40] |= ((param->temp_key->gen_dig_data & 0x01) << 5);
    msg[40] |= ((param->temp_key->gen_key_data & 0x01) << 6);
    msg[40] |= ((param->temp_key->no_mac_flag & 0x01) << 7);

    if (device_type == ATECC108A && param->temp_key->key_id < 8)
    {
        msg[41] = param->use_flag;     // UseFlag[TempKeyFlags.keyId]
        msg[42] = param->update_count; // UpdateCount[TempKeyFlags.keyId]
    }
    else
    {
        msg[41] = 0x00;
        msg[42] = 0x00;
    }

    // Serial Number
    msg[43] = param->sn[8];
    memcpy(&msg[48], &param->sn[0], 2);
    if (param->mode & SIGN_MODE_INCLUDE_SN)
    {
        memcpy(&msg[44], &param->sn[4], 4);
        memcpy(&msg[50], &param->sn[2], 2);
    }

    // The bit within the SlotLocked field corresponding to the last key used in the TempKey computation is in the LSB
    msg[52] = param->is_slot_locked ? 0x00 : 0x01;

    // If the slot contains a public key corresponding to a supported curve, and if PubInfo indicates this key must be
    // validated before being used by Verify, and if the validity bits have a value of 0x05, then the PubKey Valid byte
    // will be 0x01. In all other cases, it will be 0.
    msg[53] = param->for_invalidate ? 0x01 : 0x00;

    msg[54] = 0x00;

    if (param->message)
    {
        memcpy(param->message, msg, sizeof(msg));
    }
    if (param->verify_other_data)
    {
        memcpy(&param->verify_other_data[0],  &msg[33], 10);
        memcpy(&param->verify_other_data[10], &msg[44], 4);
        memcpy(&param->verify_other_data[14], &msg[50], 5);
    }
    if (param->digest)
    {
        return atcac_sw_sha2_256(msg, sizeof(msg), param->digest);
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

/** \brief Builds the counter match value that needs to be stored in a slot.
 *
 * \param[in]  counter_value        Counter value to be used for the counter
 *                                  match. This must be a multiple of 32.
 * \param[out] counter_match_value  Data to be stored in the beginning of a
 *                                  counter match slot will be returned here
 *                                  (8 bytes).
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcah_encode_counter_match(uint32_t counter_value, uint8_t * counter_match_value)
{
    if ((counter_value > COUNTER_MAX_VALUE) || (counter_value % 32 != 0) || (counter_match_value == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Counter match value is stored in little-endian unsigned format
    counter_match_value[0] = (uint8_t)((counter_value >> 0) & 0xFF);
    counter_match_value[1] = (uint8_t)((counter_value >> 8) & 0xFF);
    counter_match_value[2] = (uint8_t)((counter_value >> 16) & 0xFF);
    counter_match_value[3] = (uint8_t)((counter_value >> 24) & 0xFF);

    // Counter match value should be repeated in the next 4 bytes
    memcpy(counter_match_value + 4, counter_match_value, 4);

    return ATCA_SUCCESS;
}