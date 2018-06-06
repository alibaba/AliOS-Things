/*
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_ID2_H
#define _TEE_ID2_H

#include "tee_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int tee_get_ID2(uint8_t *id2, uint32_t *len);

#ifdef __cplusplus
}
#endif

#endif
