/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_IMAGESEG_H_
#define _UCLOUD_AI_IMAGESEG_H_
#include "ucloud_ai_common.h"

/** @defgroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai segmenting face api.
 *
 * @param[in]  path                face image you want to segment.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_imageseg_segment_face(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai segmenting common image api.
 *
 * @param[in]  path                common image you want to segment.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_imageseg_segment_common_image(char *path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#endif // _UCLOUD_AI_IMAGESEG_H_
