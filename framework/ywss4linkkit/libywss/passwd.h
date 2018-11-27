/*
 * Copyright (c) 2017-2018 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
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
