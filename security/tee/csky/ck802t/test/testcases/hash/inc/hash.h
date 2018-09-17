/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _HASH_H_
#define _HASH_H_

#define TEE_SHA_TEST (0x0)
#define TEE_MD5_TEST (0x1)

#define HASH_SRV_UUID                                      \
    {                                                      \
        0x7465655f, 0x6861, 0x7368,                        \
        {                                                  \
            0x5f, 0x61, 0x70, 0x69, 0x5f, 0x74, 0x65, 0x73 \
        }                                                  \
    }

extern int tee_hash_test(void);

#endif /* _HASH_H_ */
