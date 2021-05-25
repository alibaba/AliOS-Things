/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    aes_api.h
  * @brief   aes apis abstracted for encryption related.
  * @details AES encryption APIs which delivers HW/SW implemented reliable and safe AES solution.
  * @author  Abel
  * @date    2017.5.17
  * @version v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */


/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _AES_API_H_
#define _AES_API_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>


/** @defgroup AES_API AES API Sets
  * @brief API sets for aes encryption implementation
  * @{
  */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup AES_API_Exported_Functions AES API Sets Exported Functions
    * @{
    */

/**
    * @brief  128 bit AES ECB encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_ecb_encrypt(uint8_t plaintext[16], const uint8_t key[16], uint8_t *encrypted);
/**
    * @brief  128 bit AES ECB decryption on speicified encrypted data and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @return decryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
*/
bool aes128_ecb_decrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
/**
    * @brief  128 bit AES ECB encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_ecb_encrypt_msb2lsb(uint8_t plaintext[16], const uint8_t key[16],
                                uint8_t *encrypted);
/**
    * @brief  128 bit AES ECB decryption on speicified encrypted data and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @return decryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
*/
bool aes128_ecb_decrypt_msb2lsb(uint8_t *input, const uint8_t *key, uint8_t *output);

/**
    * @brief  256 bit AES ECB encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes256_ecb_encrypt(uint8_t plaintext[16], const uint8_t key[32], uint8_t *encrypted);
/**
    * @brief  256 bit AES ECB decryption on speicified encrypted data and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @return decryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
*/
bool aes256_ecb_decrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
/**
    * @brief  256 bit AES ECB encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
    */
bool aes256_ecb_encrypt_msb2lsb(uint8_t plaintext[16], const uint8_t key[32],
                                uint8_t *encrypted);
/**
    * @brief  256 bit AES ECB decryption on speicified encrypted data and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @return decryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
    */
bool aes256_ecb_decrypt_msb2lsb(uint8_t *input, const uint8_t *key, uint8_t *output);

/** @} */ /* End of group AES_API_Exported_Functions */

/** @} */ /* End of group AES_API */


#endif
