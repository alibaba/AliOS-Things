/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#ifndef _PLAT_DBG_H_
#define _PLAT_DBG_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ls_osa.h"

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)


#define PL_ERR(_f, _a ...) ls_osa_print("PL ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)

#ifdef CONFIG_PL_DBG
#define PL_INF(_f, _a ...) ls_osa_print("PL ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define PL_INF(_f, _a ...)
#endif

void dump_data(char *name, uint8_t *data, uint32_t len);

#endif /* _PLAT_DBG_H_ */

