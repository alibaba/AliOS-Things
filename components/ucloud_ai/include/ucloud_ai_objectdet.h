/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_OBJECTDET_H_
#define _UCLOUD_AI_OBJECTDET_H_
#include "ucloud_ai_common.h"

/** @defgroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai detecting object api.
 *
 * @param[in]  path                object you want to detect.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_objectdet_object_detect(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai detecting main body api.
 *
 * @param[in]  path                main body you want to detect.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_objectdet_detect_main_body(char *path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#endif // _UCLOUD_AI_OBJECTDET_H_
