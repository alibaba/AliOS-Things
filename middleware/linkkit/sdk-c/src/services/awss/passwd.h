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

int produce_signature(unsigned char *sign, unsigned char *txt, unsigned int txt_len, const char *key);
int aes_decrypt_string(char *cipher, char *plain, int len, int sec_lvl, char cbc);

const char *cal_passwd(void *key, void *random, void *passwd);

extern unsigned char aes_random[RANDOM_MAX_LEN];

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
