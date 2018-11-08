/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_CRYPTO_CMD_H_
#define _TEE_CRYPTO_CMD_H_

#define CRYPTO_SRV_UUID    {                            \
     0x5445455f, 0x4944, 0x325f,                        \
    {0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x49, 0x40 }   \
}

#define TEE_AES_INIT        0
#define TEE_AES_PROCESS     1
#define TEE_AES_FINISH      2
#define TEE_AES_RESET       3

#define TEE_HASH_INIT       4
#define TEE_HASH_UPDATE     5
#define TEE_HASH_FINISH     6
#define TEE_HASH_DIGEST     7

#define TEE_RAND_GEN        8
#define TEE_SEED            9

#endif /* _TEE_CRYPTO_CMD_H_ */
