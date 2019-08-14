/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#ifndef _PROV_RES_H_
#define _PROV_RES_H_

#include <stdint.h>

/* prov result */
#define PROV_SUCCESS           (0x00000000)
#define PROV_ERROR_RECOVERY    (0xEEEE0000)

/*
 * get provision result
 *
 * return: provision result:
 *                0x00000000  -- success
 *                0xEEEE0000  -- failed, can recovery(software failure)
 */
uint32_t prov_get_prov_res(void);

#endif /* _PROV_RES_H_ */

