
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAAS_COMMON_IMAGE_H
#define HAAS_COMMON_IMAGE_H

//#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
//#include <string>
#include <string.h>

/**@enum PixelFmt_t
* @brief Pixel Format Value \n
*/
typedef enum {
    PIXEL_FMT_NONE    = -1,
    PIXEL_FMT_YUV420P = 0,
    PIXEL_FMT_NV21    = 1,
    PIXEL_FMT_NV12    = 2,
    PIXEL_FMT_YUYV422 = 3,
    PIXEL_FMT_YUV422P = 4,
    PIXEL_FMT_RGB24   = 5,
    PIXEL_FMT_BGR24   = 6,
    PIXEL_FMT_ARGB    = 7,
    PIXEL_FMT_RGBA    = 8,
    PIXEL_FMT_ABGR    = 9,
    PIXEL_FMT_BGRA    = 10,
    PIXEL_FMT_RGB565  = 11,
} PixelFmt_t;

/**@struct ImagePos_t
* @brief image position \n
*/
typedef struct {
    uint32_t	x;
    uint32_t	y;
} ImagePos_t;

/**@struct Point_t
* @brief Point information \n
*/
typedef struct {
    uint32_t	x;
    uint32_t	y;
} Point_t;

/**@struct ImageSize_t
* @brief Image size infomation \n
*/
typedef struct {
    uint32_t	width; ///< image size width
    uint32_t	height; ///< image size height
} ImageSize_t;

/**@struct ImageBuffer_t
* @brief Image buffer information \n
*/
typedef struct {
    uint32_t		stride;
    uint32_t		width;
    uint32_t		height;
    uint32_t		size;
    PixelFmt_t	    format;
    uint32_t	    numPlane;
    uint8_t*		address[3];
    uint32_t		linesize[3];
} ImageBuffer_t;

#endif
