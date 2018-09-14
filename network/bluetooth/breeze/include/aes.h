/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#if !defined(AES_H)
#define AES_H

// Encrypt in-place. Data and key should be 16 bytes.
void AES_Encrypt(unsigned char *data, unsigned char *key);
void AES_Decrypt(unsigned char *data, unsigned char *key);

#endif //AES_H
