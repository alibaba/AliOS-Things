/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _PASSWD_H_
#define _PASSWD_H_

#define KEY_MAX_LEN     (40)
#define AES128_KEY_LEN  (16)
#define RANDOM_MAX_LEN  (16)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum {
    SSID_DECODE_TABLE = 0x0,
    NOTIFY_ENCODE_TABLE,
    DICT_CRYPT_TABLE_IDX_MAX,
};

int produce_signature(uint8_t *sign, uint8_t *txt, uint32_t txt_len, const char *key);
int aes_decrypt_string(char *cipher, char *plain, int len, int sec_lvl, char cbc);

int awss_dict_crypt(char tab_idx, uint8_t *data, uint8_t len);

const char *cal_passwd(void *key, void *random, void *passwd);

extern uint8_t aes_random[RANDOM_MAX_LEN];

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
