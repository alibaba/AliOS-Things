/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */
#ifndef TEE_RANDOM_H
#define TEE_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

int tee_random_generate(uint8_t *buf, size_t len);
int tee_random_seed(uint32_t seed);


#ifdef __cplusplus
}
#endif
#endif
