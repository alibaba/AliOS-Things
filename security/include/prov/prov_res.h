/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#ifndef _PROV_RES_H_
#define _PROV_RES_H_

#include <stdint.h>

/* prov status */
#define PROV_STAT_ABORT        (0x10001000)
#define PROV_STAT_FINISHED     (0x10001001)

/* prov result */
#define PROV_SUCCESS           (0x00000000)
#define PROV_ERROR_RECOVERY    (0xEEEE0000)

/*
 * get provision result
 *
 * prov_res[out]: provision result:
 *                0x00000000  -- success
 *                0xEEEE0000  -- failed, can recovery(software failure)
 *
 * return: 0 - success
 *        -1 - fail
 *
 * Notice: the prov_res is valid only if the return value is 0 */
int prov_get_prov_res(uint32_t *prov_res);

#endif /* _PROV_RES_H_*/

