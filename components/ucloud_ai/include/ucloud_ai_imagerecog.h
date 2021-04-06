/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_IMAGERECOG_H_
#define _UCLOUD_AI_IMAGERECOG_H_
#include "ucloud_ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai cassifying rubbish api.
 *
 * @param[in]  path                rubbish image you want to cassify.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_imagerecog_classifying_rubbish(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai detecting fruits api.
 *
 * @param[in]  path                fruit image you want to detect.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_imagerecog_detect_fruits(char *path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _UCLOUD_AI_IMAGERECOG_H_
