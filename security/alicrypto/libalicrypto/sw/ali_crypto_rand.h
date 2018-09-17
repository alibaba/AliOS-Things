/*
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#ifndef _ALI_CRYPTO_RAND_H_
#define _ALI_CRYPTO_RAND_H_

#include "ali_crypto.h"

extern void ali_crypt_rand_reseed(uint32_t seed);
extern uint32_t ali_crypt_rand_word(void);
extern uint32_t ali_crypt_rand_bytes(uint8_t *buf, uint32_t len);

#endif /* _ALI_CRYPTO_RAND_H_ */
