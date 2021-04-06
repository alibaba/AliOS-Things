/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _UCLOUD_AI_FACEBODY_H_
#define _UCLOUD_AI_FACEBODY_H_
#include "ucloud_ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ucloud_ai_aos_api
 * @{
 */

/**
 * Ucloud ai facebody comparing face api.
 *
 * @param[in]  path      face image you want to recognize.
 * @param[in]  myface    original face image you want to compare to.
 * @param[in]  cb        callback function to deal with result.
 *
 */
void ucloud_ai_facebody_comparing_face(char *path, char *myface, ucloud_ai_cb_t cb);

/**
 * Ucloud ai facebody recognizing expression api.
 *
 * @param[in]  path      face image you want to recognize.
 * @param[in]  cb        callback function to deal with result.
 *
 */
void ucloud_ai_facebody_recognize_expression(char *path, ucloud_ai_cb_t cb);

/**
 * Ucloud ai facebody generating human animation style api.
 *
 * @param[in]  path      face image you want to recognize.
 * @param[in]  cb        callback function to deal with result.
 *
 */
void ucloud_ai_facebody_generate_human_anime_style(char *path, ucloud_ai_cb_t cb);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _UCLOUD_AI_FACEBODY_H_
