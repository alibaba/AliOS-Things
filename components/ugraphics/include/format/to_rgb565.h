/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _TO_RGB565_H_
#define _TO_RGB565_H_
#include <stdbool.h>
#include <stdint.h>


/** @defgroup ugraphics_aos_api
 * @{
 */

/**
 * Rotate graphic 180 degree clockwise.
 *
 * @param[in]      rgb888_buf         rgb888 graphics buffer.
 * @param[in]      rgb888_size        rgb888 graphics size.
 * @param[out]     rgb565_buf         rgb565 grachics buffer after convert.
 * @param[out]     rgb565_size        rgb565 grachics size after convert.
 *
 * @return  0 on success, negative error on failure.
 */
int rgb888torgb565(unsigned char *rgb888_buf, int rgb888_size, \
                     unsigned short *rgb565_buf, int rgb565_size);

/**
 * Rotate graphic 180 degree clockwise.
 *
 * @param[in]      rgb888_buf         rgb565 graphics buffer.
 * @param[in]      rgb888_size        rgb565 graphics size.
 * @param[out]     rgb565_buf         rgb888 grachics buffer after convert.
 * @param[out]     rgb565_size        rgb888 grachics size after convert.
 *
 * @return  0 on success, negative error on failure.
 */
int rgb565torgb888(unsigned short *rgb565_buf, int rgb565_size, \
                     unsigned char *rgb888_buf, int rgb888_size);

/**
 * @}
 */

#endif // _TO_RGB565_H_

