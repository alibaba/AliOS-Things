/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 */

#ifndef _PROV_DBG_H_
#define _PROV_DBG_H_

#include <stdlib.h>
#include <stdint.h>
#include "ls_osa.h"

#define PROV_TAG "PROV"

#define PROV_ERR(_f, _a ...) ls_osa_print("PROV ERR %s %d: "_f, __FUNCTION__, __LINE__, ##_a)

#ifdef CONFIG_PROV_DBG
#define PROV_INF(_f, _a ...) ls_osa_print("PROV INF %s %d: "_f, __FUNCTION__, __LINE__, ##_a)
#else
#define PROV_INF(_f, _a ...)
#endif /* CONFIG_PROV_DBG */

#ifdef CONFIG_PROV_DBG
void prov_dump_data(char *name, uint8_t *data, uint32_t len);
#endif

#endif /* _PROV_DBG_H_ */

