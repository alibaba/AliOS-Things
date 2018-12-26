/**
 * @file    wm_crypto.h
 *
 * @brief   crypto driver module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_CRYPTO_H
#define WM_CRYPTO_H

/**
 * @brief	Encrypt plain data by 128 AES crypto
 * @param[in] key	the key for encryption
 * @param[in] iv	the IV value for encryption 
 * @param[in] data  where the plain data stored 
 * @param[in] data_len length of the plain data to be encrypted
 * @retval 0 finish Encryption successfully
 * @retval -1 Error
 * @note Encrypted data will be placed into the plain @data area
 *
 */
 int  aes_128_cbc_encrypt (const u8 *key, const u8 *iv, u8 *data, size_t data_len) ;

/**
 * @brief	Decrypt data by 128 AES crypto
 * @param[in] key	the key for encryption
 * @param[in] iv	the IV value for encryption 
 * @param[in] data  where the plain data stored 
 * @param[in] data_len length of the plain data to be decrypted
 * @retval 0 finish Decryption successfully
 * @retval -1 Error
 * @note plain data will be placed into the encrypted @data area 
 *
 */
int  aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data,
						 size_t data_len);

/**
 * @brief	XOR RC4 stream to given data with skip-stream-start
 * @param[in] key		RC4 key
 * @param[in] keylen	RC4 key length 
 * @param[in] data  	data to be XOR'ed with RC4 stream 
 * @param[in] data_len  length of the plain data to be encrypted
 * @retval 0 finish Encryption/Decryption successfully
 * @retval -1 Error
 * @note this function should be used for Encryption & Decryption both For the Encryption, the plain @data
 *       will be replaced by the encrypted output, and vice versa;
 */
int rc4(const u8 *key, size_t keylen, u8 *data, size_t data_len);


/**
 * @brief	MD5 hash for data vector
 * @param[in] addr		Pointers to the data area
 * @param[in] len		Lengths of the data block 
 * @param[in] mac  		Buffer for the hash (16 bytes) 
 * @retval 0 finish caculation successfully
 * @retval -1 Error
 * @note 
 */
int md5(const u8 *addr, int len, u8 *mac);

/**
 * @brief	HMAC-MD5 over data buffer (RFC 2104)
 * @param[in] key			Key for HMAC operations 
 * @param[in] keylen		Length of the key in bytes  
 * @param[in] data  		data to be caculated  
 * @param[in] data_len		Lengths of the data block 
 * @param[in] mac	  		Buffer for the hash (16 bytes) 
 * @retval 0 finish caculation successfully
 * @retval -1 Error
 * @note 
 */
int hmac_md5(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac);

/**
 * @brief	SHA1 hash for data vector
 * @param[in] addr			Pointers to the data area
 * @param[in] len 			Lengths of the data block 
 * @param[in] mac	  		Buffer for the hash (16 bytes) 
 * @retval 0 finish caculation successfully
 * @retval -1 Error
 * @note 
 */

int sha1(const u8 *addr, int len, u8 *mac);

#endif

