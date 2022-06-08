
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_IMAGE_UTILS_H
#define HAAS_IMAGE_UTILS_H

#include <stdint.h>
#include "base/include/base/HaasCommonImage.h"

class ImageUtils {
public:
	static uint32_t GetSize(PixelFmt_t format, uint32_t width, uint32_t height);
};

#endif
