/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _XOR_H_
#define _XOR_H_

#define CMD_XOR (0x12345678)

#define XOR_SRV_UUID                                       \
    {                                                      \
        0x78b97118, 0x998a, 0x1aca,                        \
        {                                                  \
            0x02, 0x8e, 0xb6, 0x01, 0xd5, 0x3e, 0xd5, 0x03 \
        }                                                  \
    }

extern int tee_xor_test(void);

#endif /* _XOR_H_ */
