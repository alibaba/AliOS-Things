/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef TEE_AES_CMD_H
#define TEE_AES_CMD_H

#define TEE_AES_CBC_ENC    (0x0)
#define TEE_AES_CBC_DEC    (0x1)


#define AES_BLOCK_SIZE  16
#define AES_KEY_SIZE    16
#define AES_IV_SIZE     16

#define AES_SRV_UUID    {                               \
    0x7465655f, 0x6861, 0x7368,                         \
    { 0x5f, 0x61, 0x70, 0x69, 0x5f, 0x74, 0x65, 0x73 }  \
}

#endif
