/**
 * \file
 * \brief  Definitions and Prototypes for ATCA Utility Functions
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


#ifndef ATCA_HOST_H
#define ATCA_HOST_H

#include <stdint.h>
#include "cryptoauthlib.h"  // contains definitions used by chip and these routines

/** \defgroup atcah Host side crypto methods (atcah_)
 *
 * \brief
 * Use these functions if your system does not use an ATCADevice as a host but
 * implements the host in firmware. The functions provide host-side cryptographic functionality
 * for an ATECC client device. They are intended to accompany the CryptoAuthLib functions.
 * They can be called directly from an application, or integrated into an API.
 *
 * Modern compilers can garbage-collect unused functions. If your compiler does not support this feature,
 * you can just discard this module from your project if you do use an ATECC as a host. Or, if you don't,
 * delete the functions you do not use.
   @{ */

/** \name Definitions for ATECC Message Sizes to Calculate a SHA256 Hash

 *  \brief "||" is the concatenation operator.
 *         The number in braces is the length of the hash input value in bytes.
   @{ */

//! RandOut{32} || NumIn{20} || OpCode{1} || Mode{1} || LSB of Param2{1}
#define ATCA_MSG_SIZE_NONCE            (55)


/** \brief (Key or TempKey){32} || (Challenge or TempKey){32} || OpCode{1} || Mode{1} || Param2{2}
|| (OTP0_7 or 0){8} || (OTP8_10 or 0){3} || SN8{1} || (SN4_7 or 0){4} || SN0_1{2} || (SN2_3 or 0){2}
*/
#define ATCA_MSG_SIZE_MAC              (88)
#define ATCA_MSG_SIZE_HMAC             (88)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_GEN_DIG          (96)


//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_DERIVE_KEY       (96)


//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2} || SN8{1} || SN0_1{2}
#define ATCA_MSG_SIZE_DERIVE_KEY_MAC   (39)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2}|| SN8{1} || SN0_1{2} || 0{25} || TempKey{32}
#define ATCA_MSG_SIZE_ENCRYPT_MAC      (96)

//! KeyId{32} || OpCode{1} || Param1{1} || Param2{2}|| SN8{1} || SN0_1{2} || 0{21} || PlainText{36}
#define ATCA_MSG_SIZE_PRIVWRITE_MAC    (96)

#define ATCA_COMMAND_HEADER_SIZE       ( 4)
#define ATCA_GENDIG_ZEROS_SIZE         (25)
#define ATCA_WRITE_MAC_ZEROS_SIZE      (25)
#define ATCA_PRIVWRITE_MAC_ZEROS_SIZE  (21)
#define ATCA_PRIVWRITE_PLAIN_TEXT_SIZE (36)
#define ATCA_DERIVE_KEY_ZEROS_SIZE     (25)
#define HMAC_BLOCK_SIZE                 (64)
#define ENCRYPTION_KEY_SIZE             (64)

/** @} */

/** \name Default Fixed Byte Values of Serial Number (SN[0:1] and SN[8])
   @{ */
#define ATCA_SN_0_DEF                (0x01)
#define ATCA_SN_1_DEF                (0x23)
#define ATCA_SN_8_DEF                (0xEE)
/** @} */


/** \name Definition for TempKey Mode
   @{ */
//! mode mask for MAC command when using TempKey
#define MAC_MODE_USE_TEMPKEY_MASK      ((uint8_t)0x03)
/** @} */

/** \brief Structure to hold TempKey fields
 */
typedef struct atca_temp_key
{
    uint8_t  value[ATCA_KEY_SIZE * 2]; //!< Value of TempKey (64 bytes for ATECC608A only)
    unsigned key_id       : 4;         //!< If TempKey was derived from a slot or transport key (GenDig or GenKey), that key ID is saved here.
    unsigned source_flag  : 1;         //!< Indicates id TempKey started from a random nonce (0) or not (1).
    unsigned gen_dig_data : 1;         //!< TempKey was derived from the GenDig command.
    unsigned gen_key_data : 1;         //!< TempKey was derived from the GenKey command (ATECC devices only).
    unsigned no_mac_flag  : 1;         //!< TempKey was derived from a key that has the NoMac bit set preventing the use of the MAC command. Known as CheckFlag in ATSHA devices).
    unsigned valid        : 1;         //!< TempKey is valid.
    uint8_t  is_64;                    //!< TempKey has 64 bytes of valid data
} atca_temp_key_t;


/** \struct atca_include_data_in_out
 *  \brief Input / output parameters for function atca_include_data().
 *  \var atca_include_data_in_out::p_temp
 *       \brief [out] pointer to output buffer
 *  \var atca_include_data_in_out::otp
 *       \brief [in] pointer to one-time-programming data
 *  \var atca_include_data_in_out::sn
 *       \brief [in] pointer to serial number data
 */
struct atca_include_data_in_out
{
    uint8_t *      p_temp;
    const uint8_t *otp;
    const uint8_t *sn;
    uint8_t        mode;
};


/** \struct atca_nonce_in_out
 *  \brief Input/output parameters for function atca_nonce().
 *  \var atca_nonce_in_out::mode
 *       \brief [in] Mode parameter used in Nonce command (Param1).
 *  \var atca_nonce_in_out::zero
 *       \brief [in] Zero parameter used in Nonce command (Param2).
 *  \var atca_nonce_in_out::num_in
 *       \brief [in] Pointer to 20-byte NumIn data used in Nonce command.
 *  \var atca_nonce_in_out::rand_out
 *       \brief [in] Pointer to 32-byte RandOut data from Nonce command.
 *  \var atca_nonce_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
typedef struct atca_nonce_in_out
{
    uint8_t               mode;
    uint16_t              zero;
    const uint8_t *       num_in;
    const uint8_t *       rand_out;
    struct atca_temp_key *temp_key;
} atca_nonce_in_out_t;


typedef struct atca_io_decrypt_in_out
{
    const uint8_t* io_key;     //!< IO protection key (32 bytes).
    const uint8_t* out_nonce;  //!< OutNonce returned from command (32 bytes).
    uint8_t*       data;       //!< As input, encrypted data. As output, decrypted data.
    size_t         data_size;  //!< Size of data in bytes (32 or 64).
} atca_io_decrypt_in_out_t;

typedef struct atca_verify_mac
{
    uint8_t                mode;        //!< Mode (Param1) parameter used in Verify command.
    uint16_t               key_id;      //!< KeyID (Param2) used in Verify command.
    const uint8_t*         signature;   //!< Signature used in Verify command (64 bytes).
    const uint8_t*         other_data;  //!< OtherData used in Verify command (19 bytes).
    const uint8_t*         msg_dig_buf; //!< Message digest buffer (64 bytes).
    const uint8_t*         io_key;      //!< IO protection key value (32 bytes).
    const uint8_t*         sn;          //!< Serial number (9 bytes).
    const atca_temp_key_t* temp_key;    //!< TempKey
    uint8_t*               mac;         //!< Calculated verification MAC is returned here (32 bytes).
} atca_verify_mac_in_out_t;


typedef struct atca_secureboot_enc_in_out
{
    const uint8_t*              io_key;      //!< IO protection key value (32 bytes)
    const struct atca_temp_key* temp_key;    //!< Current value of TempKey
    const uint8_t*              digest;      //!< Plaintext digest as input
    uint8_t*                    hashed_key;  //!< Calculated key is returned here (32 bytes)
    uint8_t*                    digest_enc;  //!< Encrypted (ciphertext) digest is return here (32 bytes)
} atca_secureboot_enc_in_out_t;


typedef struct atca_secureboot_mac_in_out
{
    uint8_t        mode;                //!< SecureBoot mode (param1)
    uint16_t       param2;              //!< SecureBoot param2
    uint16_t       secure_boot_config;  //!< SecureBootConfig value from configuration zone
    const uint8_t* hashed_key;          //!< Hashed key. SHA256(IO Protection Key | TempKey)
    const uint8_t* digest;              //!< Digest (unencrypted)
    const uint8_t* signature;           //!< Signature (can be NULL if not required)
    uint8_t*       mac;                 //!< MAC is returned here
} atca_secureboot_mac_in_out_t;

/** \struct atca_mac_in_out
 *  \brief Input/output parameters for function atca_mac().
 *  \var atca_mac_in_out::mode
 *       \brief [in] Mode parameter used in MAC command (Param1).
 *  \var atca_mac_in_out::key_id
 *       \brief [in] KeyID parameter used in MAC command (Param2).
 *  \var atca_mac_in_out::challenge
 *       \brief [in] Pointer to 32-byte Challenge data used in MAC command, depending on mode.
 *  \var atca_mac_in_out::key
 *       \brief [in] Pointer to 32-byte key used to generate MAC digest.
 *  \var atca_mac_in_out::otp
 *       \brief [in] Pointer to 11-byte OTP, optionally included in MAC digest, depending on mode.
 *  \var atca_mac_in_out::sn
 *       \brief [in] Pointer to 9-byte SN, optionally included in MAC digest, depending on mode.
 *  \var atca_mac_in_out::response
 *       \brief [out] Pointer to 32-byte SHA-256 digest (MAC).
 *  \var atca_mac_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */



typedef struct atca_mac_in_out
{
    uint8_t               mode;
    uint16_t              key_id;
    const uint8_t *       challenge;
    const uint8_t *       key;
    const uint8_t *       otp;
    const uint8_t *       sn;
    uint8_t *             response;
    struct atca_temp_key *temp_key;
} atca_mac_in_out_t;


/** \struct atca_hmac_in_out
 *  \brief Input/output parameters for function atca_hmac().
 *  \var atca_hmac_in_out::mode
 *       \brief [in] Mode parameter used in HMAC command (Param1).
 *  \var atca_hmac_in_out::key_id
 *       \brief [in] KeyID parameter used in HMAC command (Param2).
 *  \var atca_hmac_in_out::key
 *       \brief [in] Pointer to 32-byte key used to generate HMAC digest.
 *  \var atca_hmac_in_out::otp
 *       \brief [in] Pointer to 11-byte OTP, optionally included in HMAC digest, depending on mode.
 *  \var atca_hmac_in_out::sn
 *       \brief [in] Pointer to 9-byte SN, optionally included in HMAC digest, depending on mode.
 *  \var atca_hmac_in_out::response
 *       \brief [out] Pointer to 32-byte SHA-256 HMAC digest.
 *  \var atca_hmac_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_hmac_in_out
{
    uint8_t               mode;
    uint16_t              key_id;
    const uint8_t *       key;
    const uint8_t *       otp;
    const uint8_t *       sn;
    uint8_t *             response;
    struct atca_temp_key *temp_key;
};


/**
 *  \brief Input/output parameters for function atcah_gen_dig().
 */
typedef struct atca_gen_dig_in_out
{
    uint8_t               zone;         //!< [in] Zone/Param1 for the GenDig command
    uint16_t              key_id;       //!< [in] KeyId/Param2 for the GenDig command
    uint16_t              slot_conf;    //!< [in] Slot config for the GenDig command
    uint16_t              key_conf;     //!< [in] Key config for the GenDig command
    uint8_t               slot_locked;  //!< [in] slot locked for the GenDig command
    uint32_t              counter;      //!< [in] counter for the GenDig command
    bool                  is_key_nomac; //!< [in] Set to true if the slot pointed to be key_id has the SotConfig.NoMac bit set
    const uint8_t *       sn;           //!< [in] Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       stored_value; //!< [in] 32-byte slot value, config block, OTP block as specified by the Zone/KeyId parameters
    const uint8_t *       other_data;   //!< [in] 32-byte value for shared nonce zone, 4-byte value if is_key_nomac is true, ignored and/or NULL otherwise
    struct atca_temp_key *temp_key;     //!< [inout] Current state of TempKey
} atca_gen_dig_in_out_t;

/**
 *  \brief Input/output parameters for function atcah_write_auth_mac() and atcah_privwrite_auth_mac().
 */
typedef struct atca_write_mac_in_out
{
    uint8_t               zone;           //!< Zone/Param1 for the Write or PrivWrite command
    uint16_t              key_id;         //!< KeyID/Param2 for the Write or PrivWrite command
    const uint8_t *       sn;             //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       input_data;     //!< Data to be encrypted. 32 bytes for Write command, 36 bytes for PrivWrite command.
    uint8_t *             encrypted_data; //!< Encrypted version of input_data will be returned here. 32 bytes for Write command, 36 bytes for PrivWrite command.
    uint8_t *             auth_mac;       //!< Write MAC will be returned here. 32 bytes.
    struct atca_temp_key *temp_key;       //!< Current state of TempKey.
} atca_write_mac_in_out_t;

/**
 *  \brief Input/output parameters for function atcah_derive_key().
 */
struct atca_derive_key_in_out
{
    uint8_t               mode;          //!< Mode (param 1) of the derive key command
    uint16_t              target_key_id; //!< Key ID (param 2) of the target slot to run the command on
    const uint8_t *       sn;            //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *       parent_key;    //!< Parent key to be used in the derive key calculation (32 bytes).
    uint8_t *             target_key;    //!< Derived key will be returned here (32 bytes).
    struct atca_temp_key *temp_key;      //!< Current state of TempKey.
};


/**
 *  \brief Input/output parameters for function atcah_derive_key_mac().
 */
struct atca_derive_key_mac_in_out
{
    uint8_t        mode;            //!< Mode (param 1) of the derive key command
    uint16_t       target_key_id;   //!< Key ID (param 2) of the target slot to run the command on
    const uint8_t *sn;              //!< Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *parent_key;      //!< Parent key to be used in the derive key calculation (32 bytes).
    uint8_t *      mac;             //!< DeriveKey MAC will be returned here.
};


/** \struct atca_decrypt_in_out
 *  \brief Input/output parameters for function atca_decrypt().
 *  \var atca_decrypt_in_out::crypto_data
 *       \brief [in,out] Pointer to 32-byte data. Input encrypted data from Read command (Contents field), output decrypted.
 *  \var atca_decrypt_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
struct atca_decrypt_in_out
{
    uint8_t *             crypto_data;
    struct atca_temp_key *temp_key;
};


/** \brief Input/output parameters for function atcah_check_mac().
 */
typedef struct atca_check_mac_in_out
{
    uint8_t        mode;            //!< [in] CheckMac command Mode
    uint16_t       key_id;          //!< [in] CheckMac command KeyID
    const uint8_t *sn;              //!< [in] Device serial number SN[0:8]. Only SN[0:1] and SN[8] are required though.
    const uint8_t *client_chal;     //!< [in] ClientChal data, 32 bytes. Can be NULL if mode[0] is 1.
    uint8_t *      client_resp;     //!< [out] Calculated ClientResp will be returned here.
    const uint8_t *other_data;      //!< [in] OtherData, 13 bytes
    const uint8_t *otp;             //!< [in] First 8 bytes of the OTP zone data. Can be NULL is mode[5] is 0.
    const uint8_t *slot_key;        //!< [in] 32 byte key value in the slot specified by slot_id. Can be NULL if mode[1] is 1.
    /// [in] If this is not NULL, it assumes CheckMac copy is enabled for the specified key_id (ReadKey=0). If key_id
    /// is even, this should be the 32-byte key value for the slot key_id+1, otherwise this should be set to slot_key.
    const uint8_t *       target_key;
    struct atca_temp_key *temp_key; //!< [in,out] Current state of TempKey. Required if mode[0] or mode[1] are 1.
} atca_check_mac_in_out_t;


/** \struct atca_verify_in_out
 *  \brief Input/output parameters for function atcah_verify().
 *  \var atca_verify_in_out::curve_type
 *       \brief [in] Curve type used in Verify command (Param2).
 *  \var atca_verify_in_out::signature
 *       \brief [in] Pointer to ECDSA signature to be verified
 *  \var atca_verify_in_out::public_key
 *       \brief [in] Pointer to the public key to be used for verification
 *  \var atca_verify_in_out::temp_key
 *       \brief [in,out] Pointer to TempKey structure.
 */
typedef struct atca_verify_in_out
{
    uint16_t              curve_type;
    const uint8_t *       signature;
    const uint8_t *       public_key;
    struct atca_temp_key *temp_key;
} atca_verify_in_out_t;

/** \brief Input/output parameters for calculating the PubKey digest put into
 *         TempKey by the GenKey command with the
 *         atcah_gen_key_msg() function.
 */
typedef struct atca_gen_key_in_out
{
    uint8_t               mode;            //!< [in] GenKey Mode
    uint16_t              key_id;          //!< [in]  GenKey KeyID
    const uint8_t *       public_key;      //!< [in]  Public key to be used in the PubKey digest. X and Y integers in big-endian format. 64 bytes for P256 curve.
    size_t                public_key_size; //!< [in] Total number of bytes in the public key. 64 bytes for P256 curve.
    const uint8_t *       other_data;      //!< [in]  3 bytes required when bit 4 of the mode is set. Can be NULL otherwise.
    const uint8_t *       sn;              //!< [in] Device serial number SN[0:8] (9 bytes). Only SN[0:1] and SN[8] are required though.
    struct atca_temp_key *temp_key;        //!< [in,out] As input the current state of TempKey. As output, the resulting PubKEy digest.
} atca_gen_key_in_out_t;

/** \brief Input/output parameters for calculating the message and digest used
 *         by the Sign(internal) command. Used with the
 *         atcah_sign_internal_msg() function.
 */
typedef struct atca_sign_internal_in_out
{
    uint8_t                     mode;              //!< [in] Sign Mode
    uint16_t                    key_id;            //!< [in] Sign KeyID
    uint16_t                    slot_config;       //!< [in] SlotConfig[TempKeyFlags.keyId]
    uint16_t                    key_config;        //!< [in] KeyConfig[TempKeyFlags.keyId]
    uint8_t                     use_flag;          //!< [in] UseFlag[TempKeyFlags.keyId], 0x00 for slots 8 and above and for ATECC508A
    uint8_t                     update_count;      //!< [in] UpdateCount[TempKeyFlags.keyId], 0x00 for slots 8 and above and for ATECC508A
    bool                        is_slot_locked;    //!< [in] Is TempKeyFlags.keyId slot locked.
    bool                        for_invalidate;    //!< [in] Set to true if this will be used for the Verify(Invalidate) command.
    const uint8_t *             sn;                //!< [in] Device serial number SN[0:8] (9 bytes)
    const struct atca_temp_key *temp_key;          //!< [in] The current state of TempKey.
    uint8_t*                    message;           //!< [out] Full 55 byte message the Sign(internal) command will build. Can be NULL if not required.
    uint8_t*                    verify_other_data; //!< [out] The 19 byte OtherData bytes to be used with the Verify(In/Validate) command. Can be NULL if not required.
    uint8_t*                    digest;            //!< [out] SHA256 digest of the full 55 byte message. Can be NULL if not required.
} atca_sign_internal_in_out_t;

#ifdef __cplusplus
extern "C" {
#endif

ATCA_STATUS atcah_nonce(struct atca_nonce_in_out *param);
ATCA_STATUS atcah_mac(struct atca_mac_in_out *param);
ATCA_STATUS atcah_check_mac(struct atca_check_mac_in_out *param);
ATCA_STATUS atcah_hmac(struct atca_hmac_in_out *param);
ATCA_STATUS atcah_gen_dig(struct atca_gen_dig_in_out *param);
ATCA_STATUS atcah_gen_mac(struct atca_gen_dig_in_out *param);
ATCA_STATUS atcah_write_auth_mac(struct atca_write_mac_in_out *param);
ATCA_STATUS atcah_privwrite_auth_mac(struct atca_write_mac_in_out *param);
ATCA_STATUS atcah_derive_key(struct atca_derive_key_in_out *param);
ATCA_STATUS atcah_derive_key_mac(struct atca_derive_key_mac_in_out *param);
ATCA_STATUS atcah_decrypt(struct atca_decrypt_in_out *param);
ATCA_STATUS atcah_sha256(int32_t len, const uint8_t *message, uint8_t *digest);
uint8_t *atcah_include_data(struct atca_include_data_in_out *param);
ATCA_STATUS atcah_gen_key_msg(struct atca_gen_key_in_out *param);
ATCA_STATUS atcah_config_to_sign_internal(ATCADeviceType device_type, struct atca_sign_internal_in_out *param, const uint8_t* config);
ATCA_STATUS atcah_sign_internal_msg(ATCADeviceType device_type, struct atca_sign_internal_in_out *param);
ATCA_STATUS atcah_verify_mac(atca_verify_mac_in_out_t *param);
ATCA_STATUS atcah_secureboot_enc(atca_secureboot_enc_in_out_t* param);
ATCA_STATUS atcah_secureboot_mac(atca_secureboot_mac_in_out_t *param);
ATCA_STATUS atcah_encode_counter_match(uint32_t counter, uint8_t * counter_match);
ATCA_STATUS atcah_io_decrypt(struct atca_io_decrypt_in_out *param);
#ifdef __cplusplus
}
#endif

/** @} */

#endif //ATCA_HOST_H
