#ifndef _OV3660_SETTINGS_H_
#define _OV3660_SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "ov3660_regs.h"

static const ratio_settings_t ratio_table[] = {
    //  mw,   mh,  sx,  sy,   ex,   ey, ox, oy,   tx,   ty
    { 2048, 1536,   0,   0, 2079, 1547, 16, 6, 2300, 1564 }, //4x3
    { 1920, 1280,  64, 128, 2015, 1419, 16, 6, 2172, 1436 }, //3x2
    { 2048, 1280,   0, 128, 2079, 1419, 16, 6, 2300, 1436 }, //16x10
    { 1920, 1152,  64, 192, 2015, 1355, 16, 6, 2172, 1372 }, //5x3
    { 1920, 1080,  64, 242, 2015, 1333, 16, 6, 2172, 1322 }, //16x9
    { 2048,  880,   0, 328, 2079, 1219, 16, 6, 2300, 1236 }, //21x9
    { 1920, 1536,  64,   0, 2015, 1547, 16, 6, 2172, 1564 }, //5x4
    { 1536, 1536, 256,   0, 1823, 1547, 16, 6, 2044, 1564 }, //1x1
    {  864, 1536, 592,   0, 1487, 1547, 16, 6, 2044, 1564 }  //9x16
};

#define REG_DLY 0xffff
#define REGLIST_TAIL 0x0000

static const DRAM_ATTR uint16_t sensor_default_regs[][2] = {
    {SYSTEM_CTROL0, 0x82},  // software reset
    {REG_DLY, 10}, // delay 10ms

    {0x3103, 0x13},
    {SYSTEM_CTROL0, 0x42},
    {0x3017, 0xff},
    {0x3018, 0xff},
    {DRIVE_CAPABILITY, 0xc3},
    {CLOCK_POL_CONTROL, 0x21},

    {0x3611, 0x01},
    {0x3612, 0x2d},

    {0x3032, 0x00},
    {0x3614, 0x80},
    {0x3618, 0x00},
    {0x3619, 0x75},
    {0x3622, 0x80},
    {0x3623, 0x00},
    {0x3624, 0x03},
    {0x3630, 0x52},
    {0x3632, 0x07},
    {0x3633, 0xd2},
    {0x3704, 0x80},
    {0x3708, 0x66},
    {0x3709, 0x12},
    {0x370b, 0x12},
    {0x3717, 0x00},
    {0x371b, 0x60},
    {0x371c, 0x00},
    {0x3901, 0x13},

    {0x3600, 0x08},
    {0x3620, 0x43},
    {0x3702, 0x20},
    {0x3739, 0x48},
    {0x3730, 0x20},
    {0x370c, 0x0c},

    {0x3a18, 0x00},
    {0x3a19, 0xf8},

    {0x3000, 0x10},
    {0x3004, 0xef},

    {0x6700, 0x05},
    {0x6701, 0x19},
    {0x6702, 0xfd},
    {0x6703, 0xd1},
    {0x6704, 0xff},
    {0x6705, 0xff},

    {0x3c01, 0x80},
    {0x3c00, 0x04},
    {0x3a08, 0x00}, {0x3a09, 0x62}, //50Hz Band Width Step (10bit)
    {0x3a0e, 0x08}, //50Hz Max Bands in One Frame (6 bit)
    {0x3a0a, 0x00}, {0x3a0b, 0x52}, //60Hz Band Width Step (10bit)
    {0x3a0d, 0x09}, //60Hz Max Bands in One Frame (6 bit)

    {0x3a00, 0x3a},//night mode off
    {0x3a14, 0x09},
    {0x3a15, 0x30},
    {0x3a02, 0x09},
    {0x3a03, 0x30},

    {COMPRESSION_CTRL0E, 0x08},
    {0x4520, 0x0b},
    {0x460b, 0x37},
    {0x4713, 0x02},
    {0x471c, 0xd0},
    {0x5086, 0x00},

    {0x5002, 0x00},
    {0x501f, 0x00},

    {SYSTEM_CTROL0, 0x02},

    {0x5180, 0xff},
    {0x5181, 0xf2},
    {0x5182, 0x00},
    {0x5183, 0x14},
    {0x5184, 0x25},
    {0x5185, 0x24},
    {0x5186, 0x16},
    {0x5187, 0x16},
    {0x5188, 0x16},
    {0x5189, 0x68},
    {0x518a, 0x60},
    {0x518b, 0xe0},
    {0x518c, 0xb2},
    {0x518d, 0x42},
    {0x518e, 0x35},
    {0x518f, 0x56},
    {0x5190, 0x56},
    {0x5191, 0xf8},
    {0x5192, 0x04},
    {0x5193, 0x70},
    {0x5194, 0xf0},
    {0x5195, 0xf0},
    {0x5196, 0x03},
    {0x5197, 0x01},
    {0x5198, 0x04},
    {0x5199, 0x12},
    {0x519a, 0x04},
    {0x519b, 0x00},
    {0x519c, 0x06},
    {0x519d, 0x82},
    {0x519e, 0x38},

    {0x5381, 0x1d},
    {0x5382, 0x60},
    {0x5383, 0x03},
    {0x5384, 0x0c},
    {0x5385, 0x78},
    {0x5386, 0x84},
    {0x5387, 0x7d},
    {0x5388, 0x6b},
    {0x5389, 0x12},
    {0x538a, 0x01},
    {0x538b, 0x98},

    {0x5480, 0x01},
//    {0x5481, 0x05},
//    {0x5482, 0x09},
//    {0x5483, 0x10},
//    {0x5484, 0x3a},
//    {0x5485, 0x4c},
//    {0x5486, 0x5a},
//    {0x5487, 0x68},
//    {0x5488, 0x74},
//    {0x5489, 0x80},
//    {0x548a, 0x8e},
//    {0x548b, 0xa4},
//    {0x548c, 0xb4},
//    {0x548d, 0xc8},
//    {0x548e, 0xde},
//    {0x548f, 0xf0},
//    {0x5490, 0x15},

    {0x5000, 0xa7},
    {0x5800, 0x0C},
    {0x5801, 0x09},
    {0x5802, 0x0C},
    {0x5803, 0x0C},
    {0x5804, 0x0D},
    {0x5805, 0x17},
    {0x5806, 0x06},
    {0x5807, 0x05},
    {0x5808, 0x04},
    {0x5809, 0x06},
    {0x580a, 0x09},
    {0x580b, 0x0E},
    {0x580c, 0x05},
    {0x580d, 0x01},
    {0x580e, 0x01},
    {0x580f, 0x01},
    {0x5810, 0x05},
    {0x5811, 0x0D},
    {0x5812, 0x05},
    {0x5813, 0x01},
    {0x5814, 0x01},
    {0x5815, 0x01},
    {0x5816, 0x05},
    {0x5817, 0x0D},
    {0x5818, 0x08},
    {0x5819, 0x06},
    {0x581a, 0x05},
    {0x581b, 0x07},
    {0x581c, 0x0B},
    {0x581d, 0x0D},
    {0x581e, 0x12},
    {0x581f, 0x0D},
    {0x5820, 0x0E},
    {0x5821, 0x10},
    {0x5822, 0x10},
    {0x5823, 0x1E},
    {0x5824, 0x53},
    {0x5825, 0x15},
    {0x5826, 0x05},
    {0x5827, 0x14},
    {0x5828, 0x54},
    {0x5829, 0x25},
    {0x582a, 0x33},
    {0x582b, 0x33},
    {0x582c, 0x34},
    {0x582d, 0x16},
    {0x582e, 0x24},
    {0x582f, 0x41},
    {0x5830, 0x50},
    {0x5831, 0x42},
    {0x5832, 0x15},
    {0x5833, 0x25},
    {0x5834, 0x34},
    {0x5835, 0x33},
    {0x5836, 0x24},
    {0x5837, 0x26},
    {0x5838, 0x54},
    {0x5839, 0x25},
    {0x583a, 0x15},
    {0x583b, 0x25},
    {0x583c, 0x53},
    {0x583d, 0xCF},

    {0x3a0f, 0x30},
    {0x3a10, 0x28},
    {0x3a1b, 0x30},
    {0x3a1e, 0x28},
    {0x3a11, 0x60},
    {0x3a1f, 0x14},

    {0x5302, 0x28},
    {0x5303, 0x20},

    {0x5306, 0x1c}, //de-noise offset 1
    {0x5307, 0x28}, //de-noise offset 2

    {0x4002, 0xc5},
    {0x4003, 0x81},
    {0x4005, 0x12},

    {0x5688, 0x11},
    {0x5689, 0x11},
    {0x568a, 0x11},
    {0x568b, 0x11},
    {0x568c, 0x11},
    {0x568d, 0x11},
    {0x568e, 0x11},
    {0x568f, 0x11},

    {0x5580, 0x06},
    {0x5588, 0x00},
    {0x5583, 0x40},
    {0x5584, 0x2c},

    {ISP_CONTROL_01, 0x83}, // turn color matrix, awb and SDE
    {REGLIST_TAIL, 0x00}, // tail
};

static const DRAM_ATTR uint16_t sensor_fmt_jpeg[][2] = {
    {FORMAT_CTRL, 0x00}, // YUV422
    {FORMAT_CTRL00, 0x30}, // YUYV
    {0x3002, 0x00},//0x1c to 0x00 !!!
    {0x3006, 0xff},//0xc3 to 0xff !!!
    {0x471c, 0x50},//0xd0 to 0x50 !!!
    {REGLIST_TAIL, 0x00}, // tail
};

static const DRAM_ATTR uint16_t sensor_fmt_raw[][2] = {
    {FORMAT_CTRL00, 0x00}, // RAW
    {REGLIST_TAIL, 0x00}
};

static const DRAM_ATTR uint16_t sensor_fmt_grayscale[][2] = {
    {FORMAT_CTRL, 0x00}, // YUV422
    {FORMAT_CTRL00, 0x10}, // Y8
    {REGLIST_TAIL, 0x00}
};

static const DRAM_ATTR uint16_t sensor_fmt_yuv422[][2] = {
    {FORMAT_CTRL, 0x00}, // YUV422
    {FORMAT_CTRL00, 0x30}, // YUYV
    {REGLIST_TAIL, 0x00}
};

static const DRAM_ATTR uint16_t sensor_fmt_rgb565[][2] = {
    {FORMAT_CTRL, 0x01}, // RGB
    {FORMAT_CTRL00, 0x61}, // RGB565 (BGR)
    {REGLIST_TAIL, 0x00}
};

static const DRAM_ATTR uint8_t sensor_saturation_levels[9][11] = {
    {0x1d, 0x60, 0x03, 0x07, 0x48, 0x4f, 0x4b, 0x40, 0x0b, 0x01, 0x98},//-4
    {0x1d, 0x60, 0x03, 0x08, 0x54, 0x5c, 0x58, 0x4b, 0x0d, 0x01, 0x98},//-3
    {0x1d, 0x60, 0x03, 0x0a, 0x60, 0x6a, 0x64, 0x56, 0x0e, 0x01, 0x98},//-2
    {0x1d, 0x60, 0x03, 0x0b, 0x6c, 0x77, 0x70, 0x60, 0x10, 0x01, 0x98},//-1
    {0x1d, 0x60, 0x03, 0x0c, 0x78, 0x84, 0x7d, 0x6b, 0x12, 0x01, 0x98},//0
    {0x1d, 0x60, 0x03, 0x0d, 0x84, 0x91, 0x8a, 0x76, 0x14, 0x01, 0x98},//+1
    {0x1d, 0x60, 0x03, 0x0e, 0x90, 0x9e, 0x96, 0x80, 0x16, 0x01, 0x98},//+2
    {0x1d, 0x60, 0x03, 0x10, 0x9c, 0xac, 0xa2, 0x8b, 0x17, 0x01, 0x98},//+3
    {0x1d, 0x60, 0x03, 0x11, 0xa8, 0xb9, 0xaf, 0x96, 0x19, 0x01, 0x98},//+4
};

static const DRAM_ATTR uint8_t sensor_special_effects[7][4] = {
    {0x06, 0x40, 0x2c, 0x08},//Normal
    {0x46, 0x40, 0x28, 0x08},//Negative
    {0x1e, 0x80, 0x80, 0x08},//Grayscale
    {0x1e, 0x80, 0xc0, 0x08},//Red Tint
    {0x1e, 0x60, 0x60, 0x08},//Green Tint
    {0x1e, 0xa0, 0x40, 0x08},//Blue Tint
    {0x1e, 0x40, 0xa0, 0x08},//Sepia
};

#endif
