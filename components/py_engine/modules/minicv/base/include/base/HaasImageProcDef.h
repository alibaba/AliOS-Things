/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_IMAGE_PROC_DEF_H
#define HAAS_IMAGE_PROC_DEF_H

#include "base/include/base/HaasCommonImage.h"

/**@enum ImageProcType_t
* @brief Image Proc Type \n
*/
typedef enum {
    IMAGE_PROC_NONE         = -1,
    IMAGE_PROC_CPU          = 0,
    IMAGE_PROC_OPENCV       = 1,
    IMAGE_PROC_HAL          = 2,
    IMAGE_PROC_MAX          = 30,
} ImageProcType_t;
#endif
