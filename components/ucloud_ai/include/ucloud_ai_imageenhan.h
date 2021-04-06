/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_IMAGEENHAN_H_
#define _UCLOUD_AI_IMAGEENHAN_H_
#include "ucloud_ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai imageenhan erasing person api.
 *
 * @param[in]  image_path          person image you want to enhance.
 * @param[in]  user_mask_path      mask style image you want to follow.
 * @param[in]  cb        callback function to deal with result.
 *
 */
void ucloud_ai_imageenhan_erase_person(char *image_path, char *user_mask_path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai imageenhan extending image style api.
 *
 * @param[in]  major_path          image you want to extend.
 * @param[in]  style_path          style image you want to follow.
 * @param[in]  cb                  callback function to deal with result.
 *
 */
void ucloud_ai_imageenhan_extend_image_style(char *major_path, char *style_path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _UCLOUD_AI_IMAGEENHAN_H_
