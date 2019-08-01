/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef QXWZ_ENABLED

#ifndef __ULOCATION_QXWZ_SERVICE_H__
#define __ULOCATION_QXWZ_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @param[in]  user_config: usr config.
 *
 * @return:
 *   0 if success
 *  -1 if fail
 */
int ulocation_qianxun_service(ulocation_qxwz_usr_config_t *usr_config);

#endif

#endif

