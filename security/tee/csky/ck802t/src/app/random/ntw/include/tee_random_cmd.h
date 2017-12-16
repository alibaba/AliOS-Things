/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef TEE_RANDOM_CMD_H
#define TEE_RANDOM_CMD_H

#define TEE_RANDOM_SEED_SET    (0x0)
#define TEE_RANDOM_DATA_GET    (0x1)

#define RANDOM_SRV_UUID    {                             \
    0x5b174c2e, 0x1b37, 0x11e7,                         \
    { 0xb2, 0x7b, 0x00, 0x0c, 0x29, 0x7c, 0x3b, 0x37 }  \
}

#endif
