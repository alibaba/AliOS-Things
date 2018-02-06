/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_TRM_PRIV_H_
#define _TEE_TRM_PRIV_H_

#include "tee_dbg.h"

#ifdef CONFIG_TRM_DRV_DBG
#define TRM_DRV_DBG_E(str, x...)                                              \
do {                                                                        \
    tee_dbg_print(ERR, "%s:%d: " str, __FUNCTION__, __LINE__, ## x);    \
} while (0)

#define TRM_DRV_DBG_I(str, x...)                                              \
do {                                                                        \
    tee_dbg_print(INF, "%s:%d: " str, __FUNCTION__, __LINE__, ## x);    \
} while (0)
#else
#define TRM_DRV_DBG_E(str, x...)      TEE_ERROR(-8)
#define TRM_DRV_DBG_I(str, x...)
#endif

int32_t trm_get_key_data(uint32_t key_type, void *data, uint32_t *size);

#if CONFIG_ID2_RSA_KEY
int32_t tee_trm_read_rsa_key(void);
#elif CONFIG_ID2_AES_KEY
int32_t tee_trm_read_sym_key(uint32_t type);
#endif /* CONFIG_ID2_RSA_KEY */

#endif /* _TEE_TRM_PRIV_H */

