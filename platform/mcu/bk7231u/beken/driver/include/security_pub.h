#ifndef _SEC_PUB_H_
#define _SEC_PUB_H_

#define SEC_DEV_NAME		"sec"

void hal_sha_update( void *ctx, const unsigned char *input,size_t ilen );
void hal_sha_finish( void *ctx, unsigned char output[32] );
extern void bk_secrity_init(void);
extern void bk_secrity_exit(void);



/********************************************************/
//sha256
/* @brief sha256_init
  * @retval ctx
  */
void * hal_sha256_init(void);

/**
  * @brief Append SHA256 data to calculate.
  * @param ct:  from hal_sha256_init
  * @param input: the data needed to calculate sha256.
  * @param ilen: size of data.
  */
#define    hal_sha256_update     hal_sha_update
/**
  * @brief Calculate the SHA256 of all appended data.
  * @param ct: from hal_sha256_init
  * @param output: Store the result of calculation of SHA1, result
  *                need 32 bytes space.
  */
#define    hal_sha256_finish     hal_sha_finish

//sha1
/* @brief sha1_init
  * @retval ctx
  */
void * hal_sha1_init(void);
/**
  * @brief Append SHA1 data to calculate.
  * @param ct:  from hal_sha1_init
  * @param input: the data needed to calculate sha1.
  * @param ilen: size of data.
  */
#define    hal_sha1_update     hal_sha_update
/**
  * @brief Calculate the SHA1 of all appended data.
  * @param ct: from hal_sha1_init
  * @param output: Store the result of calculation of SHA1, result
  *                need 20 bytes space.
  */
#define    hal_sha1_finish     hal_sha_finish

//aes

/* @brief aes context init
  * @param key: the aes key
  * @param key: the aes key len,must be 32,24,16.
  * @retval ctx
  */
void *hal_aes_init(const u8 *key, size_t key_size);

/* @brief aes_encrypt
  * @param ctx: from hal_aes_init
  * @param plain: plain data which is need to be encrypt.
  * @param cipher: cipher data which is store the encrpyted data.
  */
void hal_aes_encrypt(void *ctx, const u8 *plain, u8 *cipher);

/* @brief aes_decrypt
  * @param ctx: from hal_aes_init
  * @param cipher: cipher data which is needed to be decrypted.
  * @param plain: plain data which store the decrypted data.
  */
void hal_aes_decrypt(void *ctx, const u8 *cipher, u8 *plain);

/* @brief deinit free ctx
  * @param ctx: from hal_aes_init
  */
void hal_aes_deinit(void *ctx);
/********************************************************/

#endif
