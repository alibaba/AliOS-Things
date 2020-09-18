
#ifndef __MTK_CRYPTO_API_USER_H__
#define __MTK_CRYPTO_API_USER_H__
int aes_ecb_hw_key_encrypt(const unsigned char *psrc, unsigned char *pdes, unsigned int dat_len);
int aes_ecb_hw_key_decrypt(const unsigned char *psrc, unsigned char *pdes, unsigned int dat_len);
int aes_cbc_hw_key_encrypt(const unsigned char *piv, const unsigned char *psrc, unsigned char *pdes, unsigned int dat_len);
int aes_cbc_hw_key_decrypt(const unsigned char *piv, const unsigned char *psrc, unsigned char *pdes, unsigned int dat_len);
#endif
