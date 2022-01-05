/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Verify command.
 *
 * The Verify command takes an ECDSA [R,S] signature and verifies that it is
 * correctly generated given an input message digest and public key.
 *
 * \note List of devices that support this command - ATECC108A, ATECC508A, and
 *       ATECC608A. There are differences in the modes that they support. Refer
 *       to device datasheet for full details.
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
#include "atca_basic.h"
#include "atca_execution.h"
#include "host/atca_host.h"


/** \brief Executes the Verify command, which takes an ECDSA [R,S] signature
 *          and verifies that it is correctly generated from a given message and
 *          public key. In all cases, the signature is an input to the command.
 *
 * For the Stored, External, and ValidateExternal Modes, the contents of
 * TempKey (or Message Digest Buffer in some cases for the ATECC608A) should
 * contain the 32 byte message.
 *
 * \param[in] mode        Verify command mode and options
 * \param[in] key_id      Stored mode, the slot containing the public key to
 *                        be used for the verification.
 *                        ValidateExternal mode, the slot containing the
 *                        public key to be validated.
 *                        External mode, KeyID contains the curve type to be
 *                        used to Verify the signature.
 *                        Validate or Invalidate mode, the slot containing
 *                        the public key to be (in)validated.
 * \param[in] signature   Signature to be verified. R and S integers in
 *                        big-endian format. 64 bytes for P256 curve.
 * \param[in] public_key  If mode is External, the public key to be used for
 *                        verification. X and Y integers in big-endian format.
 *                        64 bytes for P256 curve. NULL for all other modes.
 * \param[in] other_data  If mode is Validate, the bytes used to generate the
 *                        message for the validation (19 bytes). NULL for all
 *                        other modes.
 * \param[out] mac        If mode indicates a validating MAC, then the MAC will
 *                        will be returned here. Can be NULL otherwise.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_verify(uint8_t mode, uint16_t key_id, const uint8_t* signature, const uint8_t* public_key, const uint8_t* other_data, uint8_t* mac)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t verify_mode = (mode & VERIFY_MODE_MASK);

    do
    {
        if (signature == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        if (verify_mode == VERIFY_MODE_EXTERNAL && public_key == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        if ((verify_mode == VERIFY_MODE_VALIDATE || verify_mode == VERIFY_MODE_INVALIDATE) && other_data == NULL)
        {
            return ATCA_BAD_PARAM;
        }

        // Build the verify command
        packet.param1 = mode;
        packet.param2 = key_id;
        memcpy(&packet.data[0], signature, ATCA_SIG_SIZE);
        if (verify_mode == VERIFY_MODE_EXTERNAL)
        {
            memcpy(&packet.data[ATCA_SIG_SIZE], public_key, ATCA_PUB_KEY_SIZE);
        }
        else if (other_data)
        {
            memcpy(&packet.data[ATCA_SIG_SIZE], other_data, VERIFY_OTHER_DATA_SIZE);
        }

        if ((status = atVerify(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

        // The Verify command may return MAC if requested
        if ((mac != NULL) && (packet.data[ATCA_COUNT_IDX] >= (ATCA_PACKET_OVERHEAD + MAC_SIZE)))
        {
            memcpy(mac, &packet.data[ATCA_RSP_DATA_IDX], MAC_SIZE);
        }

    }
    while (false);

    return status;
}

/** \brief Executes the Verify command with verification MAC for the External
 *          or Stored Verify modes.. This function is only available on the
 *          ATECC608A.
 *
 * \param[in] mode          Verify command mode. Can be VERIFY_MODE_EXTERNAL or
 *                          VERIFY_MODE_STORED.
 * \param[in] key_id        For VERIFY_MODE_STORED mode, the slot containing the
 *                          public key to be used for the verification.
 *                          For VERIFY_MODE_EXTERNAL mode, KeyID contains the
 *                          curve type to be used to Verify the signature. Only
 *                          VERIFY_KEY_P256 supported.
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   For VERIFY_MODE_EXTERNAL mode, the public key to be
 *                          used for verification. X and Y integers in
 *                          big-endian format. 64 bytes for P256 curve. Null for
 *                          VERIFY_MODE_STORED mode.
 * \param[in]  num_in       System nonce (32 byte) used for the verification
 *                          MAC.
 * \param[in]  io_key       IO protection key for verifying the validation MAC.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
static ATCA_STATUS atcab_verify_extern_stored_mac(
    uint8_t        mode,
    uint16_t       key_id,
    const uint8_t* message,
    const uint8_t* signature,
    const uint8_t* public_key,
    const uint8_t* num_in,
    const uint8_t* io_key,
    bool*          is_verified)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t msg_dig_buf[64];
    atca_verify_mac_in_out_t verify_mac_params;
    uint8_t mac[SECUREBOOT_MAC_SIZE];
    uint8_t host_mac[SECUREBOOT_MAC_SIZE];

    do
    {
        if (is_verified == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        *is_verified = false;
        if (signature == NULL || message == NULL || num_in == NULL || io_key == NULL)
        {
            return ATCA_BAD_PARAM;
        }
        if ((mode & VERIFY_MODE_MASK) == VERIFY_MODE_EXTERNAL && public_key == NULL)
        {
            return ATCA_BAD_PARAM; // Only External mode requires an external public key

        }
        // When using the message digest buffer as the message source, the
        // second 32 bytes in the buffer will be the MAC system nonce.
        memcpy(&msg_dig_buf[0], message, 32);
        memcpy(&msg_dig_buf[32], num_in, 32);
        if ((status = atcab_nonce_load(NONCE_MODE_TARGET_MSGDIGBUF, msg_dig_buf, 64)) != ATCA_SUCCESS)
        {
            break;
        }

        // Calculate the expected MAC
        memset(&verify_mac_params, 0, sizeof(verify_mac_params));
        verify_mac_params.mode = mode | VERIFY_MODE_SOURCE_MSGDIGBUF | VERIFY_MODE_MAC_FLAG;
        verify_mac_params.key_id = key_id;
        verify_mac_params.signature = signature;
        verify_mac_params.msg_dig_buf = msg_dig_buf;
        verify_mac_params.io_key = io_key;
        verify_mac_params.temp_key = NULL;
        verify_mac_params.sn = NULL;
        verify_mac_params.mac = host_mac;
        status = atcah_verify_mac(&verify_mac_params);
        if (status != ATCA_SUCCESS)
        {
            break;
        }

        status = atcab_verify(verify_mac_params.mode, verify_mac_params.key_id, signature, public_key, NULL, mac);
        if (status != ATCA_SUCCESS)
        {
            if (status == ATCA_CHECKMAC_VERIFY_FAILED)
            {
                status = ATCA_SUCCESS;  // Verify failed, but command succeeded
            }
            break;
        }

        *is_verified = (memcmp(host_mac, mac, MAC_SIZE) == 0);
    }
    while (0);

    return status;
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with all components (message, signature, and
 *          public key) supplied. The message to be signed will be loaded into
 *          the Message Digest Buffer to the ATECC608A device or TempKey for
 *          other devices.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   The public key to be used for verification. X and
 *                          Y integers in big-endian format. 64 bytes for
 *                          P256 curve.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_extern(const uint8_t *message, const uint8_t *signature, const uint8_t *public_key, bool *is_verified)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t nonce_target = NONCE_MODE_TARGET_TEMPKEY;
    uint8_t verify_source = VERIFY_MODE_SOURCE_TEMPKEY;

    if (is_verified == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *is_verified = false;

    if (signature == NULL || message == NULL || public_key == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        // Load message into device
        if (_gDevice->mCommands->dt == ATECC608A)
        {
            // Use the Message Digest Buffer for the ATECC608A
            nonce_target = NONCE_MODE_TARGET_MSGDIGBUF;
            verify_source = VERIFY_MODE_SOURCE_MSGDIGBUF;
        }
        if ((status = atcab_nonce_load(nonce_target, message, 32)) != ATCA_SUCCESS)
        {
            break;
        }

        status = atcab_verify(VERIFY_MODE_EXTERNAL | verify_source, VERIFY_KEY_P256, signature, public_key, NULL, NULL);
        *is_verified = (status == ATCA_SUCCESS);

        if (status == ATCA_CHECKMAC_VERIFY_FAILED)
        {
            status = ATCA_SUCCESS;  // Verify failed, but command succeeded

        }
    }
    while (0);

    return status;
}

/** \brief Executes the Verify command with verification MAC, which verifies a
 *          signature (ECDSA verify operation) with all components (message,
 *          signature, and public key) supplied. This function is only available
 *          on the ATECC608A.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  public_key   The public key to be used for verification. X and
 *                          Y integers in big-endian format. 64 bytes for
 *                          P256 curve.
 * \param[in]  num_in       System nonce (32 byte) used for the verification
 *                          MAC.
 * \param[in]  io_key       IO protection key for verifying the validation MAC.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_extern_mac(const uint8_t *message, const uint8_t* signature, const uint8_t* public_key, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    return atcab_verify_extern_stored_mac(
        VERIFY_MODE_EXTERNAL,
        VERIFY_KEY_P256,
        message,
        signature,
        public_key,
        num_in,
        io_key,
        is_verified);
}

/** \brief Executes the Verify command, which verifies a signature (ECDSA
 *          verify operation) with a public key stored in the device. The
 *          message to be signed will be loaded into the Message Digest Buffer
 *          to the ATECC608A device or TempKey for other devices.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  key_id       Slot containing the public key to be used in the
 *                         verification.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_stored(const uint8_t *message, const uint8_t *signature, uint16_t key_id, bool *is_verified)
{
    ATCA_STATUS status = ATCA_GEN_FAIL;
    uint8_t nonce_target = NONCE_MODE_TARGET_TEMPKEY;
    uint8_t verify_source = VERIFY_MODE_SOURCE_TEMPKEY;

    if (is_verified == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    *is_verified = false;

    if (signature == NULL || message == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    do
    {
        // Load message into device
        if (_gDevice->mCommands->dt == ATECC608A)
        {
            // Use the Message Digest Buffer for the ATECC608A
            nonce_target = NONCE_MODE_TARGET_MSGDIGBUF;
            verify_source = VERIFY_MODE_SOURCE_MSGDIGBUF;
        }
        if ((status = atcab_nonce_load(nonce_target, message, 32)) != ATCA_SUCCESS)
        {
            break;
        }

        status = atcab_verify(VERIFY_MODE_STORED | verify_source, key_id, signature, NULL, NULL, NULL);
        *is_verified = (status == ATCA_SUCCESS);

        if (status == ATCA_CHECKMAC_VERIFY_FAILED)
        {
            status = ATCA_SUCCESS;  // Verify failed, but command succeeded

        }
    }
    while (0);

    return status;
}

/** \brief Executes the Verify command with verification MAC, which verifies a
 *          signature (ECDSA verify operation) with a public key stored in the
 *          device. This function is only available on the ATECC608A.
 *
 * \param[in]  message      32 byte message to be verified. Typically
 *                          the SHA256 hash of the full message.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  key_id       Slot containing the public key to be used in the
 *                          verification.
 * \param[in]  num_in       System nonce (32 byte) used for the verification
 *                          MAC.
 * \param[in]  io_key       IO protection key for verifying the validation MAC.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_stored_mac(const uint8_t *message, const uint8_t *signature, uint16_t key_id, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified)
{
    return atcab_verify_extern_stored_mac(
        VERIFY_MODE_STORED,
        key_id,
        message,
        signature,
        NULL,
        num_in,
        io_key,
        is_verified);
}

/** \brief Executes the Verify command in Validate mode to validate a public
 *          key stored in a slot.
 *
 * This command can only be run after GenKey has been used to create a PubKey
 * digest of the public key to be validated in TempKey (mode=0x10).
 *
 * \param[in]  key_id       Slot containing the public key to be validated.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  other_data   19 bytes of data used to build the verification
 *                          message.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          validation public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_validate(uint16_t key_id, const uint8_t *signature, const uint8_t *other_data, bool *is_verified)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (signature == NULL || other_data == NULL || is_verified == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_verify(VERIFY_MODE_VALIDATE, key_id, signature, NULL, other_data, NULL);
    *is_verified = (status == ATCA_SUCCESS);

    if (status == ATCA_CHECKMAC_VERIFY_FAILED)
    {
        status = ATCA_SUCCESS;  // Verify failed, but command succeeded

    }
    return status;
}

/** \brief Executes the Verify command in Invalidate mode which invalidates a
 *          previously validated public key stored in a slot.
 *
 * This command can only be run after GenKey has been used to create a PubKey
 * digest of the public key to be invalidated in TempKey (mode=0x10).
 *
 * \param[in]  key_id       Slot containing the public key to be invalidated.
 * \param[in]  signature    Signature to be verified. R and S integers in
 *                          big-endian format. 64 bytes for P256 curve.
 * \param[in]  other_data   19 bytes of data used to build the verification
 *                          message.
 * \param[out] is_verified  Boolean whether or not the message, signature,
 *                          validation public key verified.
 *
 * \return ATCA_SUCCESS on verification success or failure, because the
 *         command still completed successfully.
 */
ATCA_STATUS atcab_verify_invalidate(uint16_t key_id, const uint8_t *signature, const uint8_t *other_data, bool *is_verified)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (signature == NULL || other_data == NULL || is_verified == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    status = atcab_verify(VERIFY_MODE_INVALIDATE, key_id, signature, NULL, other_data, NULL);
    *is_verified = (status == ATCA_SUCCESS);

    if (status == ATCA_CHECKMAC_VERIFY_FAILED)
    {
        status = ATCA_SUCCESS;  // Verify failed, but command succeeded

    }
    return status;
}

