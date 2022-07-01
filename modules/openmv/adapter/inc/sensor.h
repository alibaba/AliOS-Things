#ifndef __SENSOR_B_H__
#define __SENSOR_B_H__

#include <sensor.h>
#ifdef __IMLIB_H__
typedef enum {
    OMV_PIXFORMAT_INVALID = 0,
    OMV_PIXFORMAT_BINARY,    // 1BPP/BINARY
    OMV_PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    OMV_PIXFORMAT_RGB565,    // 2BPP/RGB565
    OMV_PIXFORMAT_YUV422,    // 2BPP/YUV422
    OMV_PIXFORMAT_BAYER,     // 1BPP/RAW
    OMV_PIXFORMAT_JPEG,      // JPEG/COMPRESSED
} omv_pixformat_t;
#endif
#include "omv_sensor.h"

#endif
