/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_CRYPT_H__
#define __AWSS_CRYPT_H__

int aes_decrypt_string(char *cipher, char *plain, int len, int sec_lvl, char cbc);

#endif
