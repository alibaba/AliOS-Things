/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_security.h
 * @brief:   vendor security interface
 * @author:  
 * @date:    2019/12/16
 * @version: 1.0
 */
#ifndef _TG_VENDOR_SECURITY_H_
#define _TG_VENDOR_SECURITY_H_

#define RSA2048_PUBLIC_KEY_LEN   256
#define SHA256_DIGEST_SIZE       32
#define PUB_OPT_HEADER_LEN       4
#define PUB_OPT_END_LEN          4

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Get current secure boot status
 *
 * @returns 0: secure boot is not enabled; 1: secure boot enabled
 */
int tg_secure_get_secboot_status(void);

/**
 * @brief Set secure boot root public key hash into efuse
 *
 * @param[in] pubkey_hash: The hash of root public key
 * @returns 0: succeed; -1: failed
 */
int tg_secure_set_secboot_hash(const char* pubkey_hash);

/**
 * @brief Set secure boot root public key hash into efuse
 *
 * @param[in] buffer: The buffer to store the hash
 * @param[in] buffer_len: length of the buffer
 * @returns 0: succeed; -1: failed
 */
int tg_secure_get_secboot_hash(char* buffer, int buffer_len);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_SECURITY_H_ */
