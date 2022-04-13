/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef _ROTATE_H_
#define _ROTATE_H_
#include <stdbool.h>
#include <stdint.h>

/** @defgroup ugraphics_aos_api
 * @{
 */

/**
 * Rotate graphic 90 degree clockwise.
 *
 * @param[in]      src         graphic source buffer.
 * @param[in]      src_w       width of graphics.
 * @param[in]      src_h       height of graphics.
 * @param[in]      channel     channel number of graphics color.
 *
 * @return  0 on success, negative error on failure.
 */
int rotate_right90(unsigned char *src, int src_w, int src_h, int channel);

/**
 * Rotate graphic 90 degree anticlockwise.
 *
 * @param[in]      src         graphic source buffer.
 * @param[in]      src_w       width of graphics.
 * @param[in]      src_h       height of graphics.
 * @param[in]      channel     channel number of graphics color.
 *
 * @return  0 on success, negative error on failure.
 */
int rotate_left90(unsigned char *src, int src_w, int src_h, int channel);

/**
 * Rotate graphic 180 degree clockwise.
 *
 * @param[in]      src         graphic source buffer.
 * @param[in]      src_w       width of graphics.
 * @param[in]      src_h       height of graphics.
 * @param[in]      channel     channel number of graphics color.
 *
 * @return  0 on success, negative error on failure.
 */
int rotate_down(unsigned char *src, int src_w, int src_h, int channel);

/**
 * @}
 */

#endif // _ROTATE_H_

