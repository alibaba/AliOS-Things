#include <stdio.h>
#include <string.h>
#include "layer21.h"
#include "mp3dec.h"

#if (CONFIG_APP_MP3PLAYER == 1)

const uint16_t mpa_bitrate_tab[2][3][15] = {
    { {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
      {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
      {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 } },
    { {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160}
    }
};

const uint16_t mpa_freq_tab[3] = { 44100, 48000, 32000 };

/*******************************************************/
/* layer 2 tables */

const int sblimit_table[5] = { 27 , 30 , 8, 12 , 30 };

const int quant_steps[17] = {
    3,     5,    7,    9,    15,
    31,    63,  127,  255,   511,
    1023,  2047, 4095, 8191, 16383,
    32767, 65535
};

/* we use a negative value if grouped */
const int quant_bits[17] = {
    -5,  -7,  3, -10, 4,
    5,  6,  7,  8,  9,
    10, 11, 12, 13, 14,
    15, 16
};

/* encoding tables which give the quantization index. Note how it is
   possible to store them efficiently ! */
const unsigned char alloc_table_0[] = {
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
};

const unsigned char alloc_table_1[] = {
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    3,  0,  1,  2,  3,  4,  5, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
    2,  0,  1, 16,
};

const unsigned char alloc_table_2[] = {
    4,  0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    4,  0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
};

const unsigned char alloc_table_3[] = {
    4,  0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    4,  0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
};

const unsigned char alloc_table_4[] = {
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    4,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    3,  0,  1,  3,  4,  5,  6,  7,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
    2,  0,  1,  3,
};

const uint8_t * const alloc_tables[5] =
    { alloc_table_0, alloc_table_1, alloc_table_2, alloc_table_3, alloc_table_4};

/*******************************************************/

const int16_t window[512]=
    {
        /********window begin *************/
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0xffff,0xffff,0xffff,0xffff,0xffff,
        0xffff,0xffff,0xfffe,0xfffe,0xfffe,0xfffe,0xfffe,0xfffd,
        0xfffd,0xfffd,0xfffc,0xfffc,0xfffb,0xfffb,0xfffa,0xfffa,
        0xfff9,0xfff8,0xfff7,0xfff7,0xfff6,0xfff5,0xfff4,0xfff3,
        0xfff2,0xfff0,0xffef,0xffee,0xffec,0xffeb,0xffe9,0xffe8,
        0xffe6,0xffe4,0xffe3,0xffe1,0xffdf,0xffdd,0xffdb,0xffda,
        0xffd8,0xffd6,0xffd4,0xffd2,0xffd1,0xffcf,0xffce,0xffcc,
        0x0035,0x0037,0x0038,0x0038,0x0039,0x0039,0x0039,0x0039,
        0x0038,0x0037,0x0036,0x0034,0x0032,0x002f,0x002c,0x0029,
        0x0025,0x0020,0x001b,0x0015,0x000e,0x0007,0x0000,0xfff7,
        0xffee,0xffe4,0xffda,0xffcf,0xffc3,0xffb7,0xffa9,0xff9c,
        0xff8d,0xff7e,0xff6f,0xff5f,0xff4e,0xff3d,0xff2c,0xff1a,
        0xff08,0xfef6,0xfee4,0xfed2,0xfebf,0xfead,0xfe9b,0xfe8a,
        0xfe78,0xfe68,0xfe58,0xfe48,0xfe3a,0xfe2d,0xfe20,0xfe16,
        0xfe0c,0xfe04,0xfdfe,0xfdf9,0xfdf7,0xfdf6,0xfdf8,0xfdfc,
        0x01fd,0x01f4,0x01e8,0x01d9,0x01c8,0x01b3,0x019b,0x0180,
        0x0162,0x0140,0x011b,0x00f3,0x00c7,0x0097,0x0065,0x002e,
        0xfff5,0xffb8,0xff78,0xff35,0xfeee,0xfea5,0xfe59,0xfe0b,
        0xfdba,0xfd66,0xfd11,0xfcba,0xfc62,0xfc08,0xfbae,0xfb53,
        0xfaf8,0xfa9d,0xfa42,0xf9e9,0xf991,0xf93a,0xf8e6,0xf895,
        0xf847,0xf7fc,0xf7b5,0xf773,0xf737,0xf6ff,0xf6ce,0xf6a4,
        0xf680,0xf665,0xf651,0xf646,0xf645,0xf64c,0xf65e,0xf67b,
        0xf6a2,0xf6d5,0xf713,0xf75e,0xf7b5,0xf819,0xf88a,0xf909,
        0x066c,0x05d2,0x052a,0x0474,0x03b0,0x02de,0x01fd,0x010f,
        0x0012,0xff07,0xfdee,0xfcc7,0xfb93,0xfa52,0xf904,0xf7a9,
        0xf642,0xf4d0,0xf353,0xf1cb,0xf039,0xee9e,0xecfb,0xeb4f,
        0xe99c,0xe7e3,0xe624,0xe460,0xe298,0xe0cd,0xdf00,0xdd32,
        0xdb64,0xd996,0xd7ca,0xd601,0xd43b,0xd27a,0xd0bf,0xcf0a,
        0xcd5d,0xcbb9,0xca1e,0xc88e,0xc709,0xc591,0xc426,0xc2c9,
        0xc17b,0xc03e,0xbf11,0xbdf5,0xbceb,0xbbf4,0xbb11,0xba41,
        0xb986,0xb8df,0xb84e,0xb7d3,0xb76e,0xb71f,0xb6e6,0xb6c4,
        0x4948,0x493c,0x491a,0x48e1,0x4892,0x482d,0x47b2,0x4721,
        0x467a,0x45bf,0x44ef,0x440c,0x4315,0x420b,0x40ef,0x3fc2,
        0x3e85,0x3d37,0x3bda,0x3a6f,0x38f7,0x3772,0x35e2,0x3447,
        0x32a3,0x30f6,0x2f41,0x2d86,0x2bc5,0x29ff,0x2836,0x266a,
        0x249c,0x22ce,0x2100,0x1f33,0x1d68,0x1ba0,0x19dc,0x181d,
        0x1664,0x14b1,0x1305,0x1162,0x0fc7,0x0e35,0x0cad,0x0b30,
        0x09be,0x0857,0x06fc,0x05ae,0x046d,0x0339,0x0212,0x00f9,
        0xffee,0xfef1,0xfe03,0xfd22,0xfc50,0xfb8c,0xfad6,0xfa2e,
        0x066c,0x06f7,0x0776,0x07e7,0x084b,0x08a2,0x08ed,0x092b,
        0x095e,0x0985,0x09a2,0x09b4,0x09bb,0x09ba,0x09af,0x099b,
        0x0980,0x095c,0x0932,0x0901,0x08c9,0x088d,0x084b,0x0804,
        0x07b9,0x076b,0x071a,0x06c6,0x066f,0x0617,0x05be,0x0563,
        0x0508,0x04ad,0x0452,0x03f8,0x039e,0x0346,0x02ef,0x029a,
        0x0246,0x01f5,0x01a7,0x015b,0x0112,0x00cb,0x0088,0x0048,
        0x000b,0xffd2,0xff9b,0xff69,0xff39,0xff0d,0xfee5,0xfec0,
        0xfe9e,0xfe80,0xfe65,0xfe4d,0xfe38,0xfe27,0xfe18,0xfe0c,
        0x01fd,0x0204,0x0208,0x020a,0x0209,0x0207,0x0202,0x01fc,
        0x01f4,0x01ea,0x01e0,0x01d3,0x01c6,0x01b8,0x01a8,0x0198,
        0x0188,0x0176,0x0165,0x0153,0x0141,0x012e,0x011c,0x010a,
        0x00f8,0x00e6,0x00d4,0x00c3,0x00b2,0x00a1,0x0091,0x0082,
        0x0073,0x0064,0x0057,0x0049,0x003d,0x0031,0x0026,0x001c,
        0x0012,0x0009,0x0000,0xfff9,0xfff2,0xffeb,0xffe5,0xffe0,
        0xffdb,0xffd7,0xffd4,0xffd1,0xffce,0xffcc,0xffca,0xffc9,
        0xffc8,0xffc7,0xffc7,0xffc7,0xffc7,0xffc8,0xffc8,0xffc9,
        0x0035,0x0034,0x0032,0x0031,0x002f,0x002e,0x002c,0x002a,
        0x0028,0x0026,0x0025,0x0023,0x0021,0x001f,0x001d,0x001c,
        0x001a,0x0018,0x0017,0x0015,0x0014,0x0012,0x0011,0x0010,
        0x000e,0x000d,0x000c,0x000b,0x000a,0x0009,0x0009,0x0008,
        0x0007,0x0006,0x0006,0x0005,0x0005,0x0004,0x0004,0x0003,
        0x0003,0x0003,0x0002,0x0002,0x0002,0x0002,0x0002,0x0001,
        0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        /********window end*************/
    };
/* lower 2 bits: modulo 3, higher bits: shift */
const uint16_t scale_factor_modshift[64] =
    {
        /********scale_factor_modshift begin *************/
        0x0000,0x0001,0x0002,0x0004,0x0005,0x0006,0x0008,0x0009,
        0x000a,0x000c,0x000d,0x000e,0x0010,0x0011,0x0012,0x0014,
        0x0015,0x0016,0x0018,0x0019,0x001a,0x001c,0x001d,0x001e,
        0x0020,0x0021,0x0022,0x0024,0x0025,0x0026,0x0028,0x0029,
        0x002a,0x002c,0x002d,0x002e,0x0030,0x0031,0x0032,0x0034,
        0x0035,0x0036,0x0038,0x0039,0x003a,0x003c,0x003d,0x003e,
        0x0040,0x0041,0x0042,0x0044,0x0045,0x0046,0x0048,0x0049,
        0x004a,0x004c,0x004d,0x004e,0x0050,0x0051,0x0052,0x0054,
        /********scale_factor_modshift end*************/
    };
/* [i][j]:  2^(-j/3) * FRAC_ONE * 2^(i+2) / (2^(i+2) - 1) */
const int32_t scale_factor_mult[15][3] =
    {
        /********scale_factor_mult begin *************/
        {0x00015554,0x00010ee9,0x0000d705},
        {0x00012492,0x0000e836,0x0000b84e},
        {0x00011110,0x0000d8ba,0x0000ac04},
        {0x00010842,0x0000d1bd,0x0000a678},
        {0x00010410,0x0000ce69,0x0000a3d4},
        {0x00010204,0x0000ccc9,0x0000a28a},
        {0x00010100,0x0000cbfb,0x0000a1e6},
        {0x00010080,0x0000cb95,0x0000a195},
        {0x00010040,0x0000cb62,0x0000a16d},
        {0x00010020,0x0000cb49,0x0000a159},
        {0x00010010,0x0000cb3c,0x0000a14f},
        {0x00010008,0x0000cb36,0x0000a14a},
        {0x00010004,0x0000cb33,0x0000a147},
        {0x00010002,0x0000cb31,0x0000a146},
        {0xffff0000,0xffff34d0,0xffff5ebb},
        /********scale_factor_mult end*************/
    };
/* mult table for layer 2 group quantization */

#define SCALE_GEN(v)                                                    \
    { FIXR(1.0 * (v)), FIXR(0.7937005259 * (v)), FIXR(0.6299605249 * (v)) }

const int32_t scale_factor_mult2[3][3] = {
    SCALE_GEN(4.0 / 3.0), /* 3 steps */
    SCALE_GEN(4.0 / 5.0), /* 5 steps */
    SCALE_GEN(4.0 / 9.0), /* 9 steps */
};

/* 2^(n/4) */
const uint32_t scale_factor_mult3[4] =
    {
        FIXR(1.0),
        FIXR(1.18920711500272106671),
        FIXR(1.41421356237309504880),
        FIXR(1.68179283050742908605),
    };
#define COS0_0  FIXR(0.50060299823519630134)
#define COS0_1  FIXR(0.50547095989754365998)
#define COS0_2  FIXR(0.51544730992262454697)
#define COS0_3  FIXR(0.53104259108978417447)
#define COS0_4  FIXR(0.55310389603444452782)
#define COS0_5  FIXR(0.58293496820613387367)
#define COS0_6  FIXR(0.62250412303566481615)
#define COS0_7  FIXR(0.67480834145500574602)
#define COS0_8  FIXR(0.74453627100229844977)
#define COS0_9  FIXR(0.83934964541552703873)
#define COS0_10 FIXR(0.97256823786196069369)
#define COS0_11 FIXR(1.16943993343288495515)
#define COS0_12 FIXR(1.48416461631416627724)
#define COS0_13 FIXR(2.05778100995341155085)
#define COS0_14 FIXR(3.40760841846871878570)
#define COS0_15 FIXR(10.19000812354805681150)

#define COS1_0 FIXR(0.50241928618815570551)
#define COS1_1 FIXR(0.52249861493968888062)
#define COS1_2 FIXR(0.56694403481635770368)
#define COS1_3 FIXR(0.64682178335999012954)
#define COS1_4 FIXR(0.78815462345125022473)
#define COS1_5 FIXR(1.06067768599034747134)
#define COS1_6 FIXR(1.72244709823833392782)
#define COS1_7 FIXR(5.10114861868916385802)

#define COS2_0 FIXR(0.50979557910415916894)
#define COS2_1 FIXR(0.60134488693504528054)
#define COS2_2 FIXR(0.89997622313641570463)
#define COS2_3 FIXR(2.56291544774150617881)

#define COS3_0 FIXR(0.54119610014619698439)
#define COS3_1 FIXR(1.30656296487637652785)

#define COS4_0 FIXR(0.70710678118654752439)

/* butterfly operator */
#define BF(a, b, c)                                \
    {                                            \
        tmp0 = tab[a] + tab[b];                    \
        tmp1 = tab[a] - tab[b];                    \
        tab[a] = tmp0;                            \
        tab[b] = MULL(tmp1, c);                    \
    }

#define BF1(a, b, c, d)                            \
    {                                            \
        BF(a, b, COS4_0);                        \
        BF(c, d, -COS4_0);                        \
        tab[c] += tab[d];                        \
    }

#define BF2(a, b, c, d)                            \
    {                                            \
        BF(a, b, COS4_0);                        \
        BF(c, d, -COS4_0);                        \
        tab[c] += tab[d];                        \
        tab[a] += tab[c];                        \
        tab[c] += tab[b];                        \
        tab[b] += tab[d];                        \
    }

#define ADD(a, b) tab[a] += tab[b]

/* DCT32 without 1/sqrt(2) coef zero scaling. */
static void dct32(int32_t *out, int32_t *tab)
{
    int tmp0, tmp1;

    /* pass 1 */
    BF(0, 31, COS0_0);
    BF(1, 30, COS0_1);
    BF(2, 29, COS0_2);
    BF(3, 28, COS0_3);
    BF(4, 27, COS0_4);
    BF(5, 26, COS0_5);
    BF(6, 25, COS0_6);
    BF(7, 24, COS0_7);
    BF(8, 23, COS0_8);
    BF(9, 22, COS0_9);
    BF(10, 21, COS0_10);
    BF(11, 20, COS0_11);
    BF(12, 19, COS0_12);
    BF(13, 18, COS0_13);
    BF(14, 17, COS0_14);
    BF(15, 16, COS0_15);

    /* pass 2 */
    BF(0, 15, COS1_0);
    BF(1, 14, COS1_1);
    BF(2, 13, COS1_2);
    BF(3, 12, COS1_3);
    BF(4, 11, COS1_4);
    BF(5, 10, COS1_5);
    BF(6,  9, COS1_6);
    BF(7,  8, COS1_7);

    BF(16, 31, -COS1_0);
    BF(17, 30, -COS1_1);
    BF(18, 29, -COS1_2);
    BF(19, 28, -COS1_3);
    BF(20, 27, -COS1_4);
    BF(21, 26, -COS1_5);
    BF(22, 25, -COS1_6);
    BF(23, 24, -COS1_7);

    /* pass 3 */
    BF(0, 7, COS2_0);
    BF(1, 6, COS2_1);
    BF(2, 5, COS2_2);
    BF(3, 4, COS2_3);

    BF(8, 15, -COS2_0);
    BF(9, 14, -COS2_1);
    BF(10, 13, -COS2_2);
    BF(11, 12, -COS2_3);

    BF(16, 23, COS2_0);
    BF(17, 22, COS2_1);
    BF(18, 21, COS2_2);
    BF(19, 20, COS2_3);

    BF(24, 31, -COS2_0);
    BF(25, 30, -COS2_1);
    BF(26, 29, -COS2_2);
    BF(27, 28, -COS2_3);

    /* pass 4 */
    BF(0, 3, COS3_0);
    BF(1, 2, COS3_1);

    BF(4, 7, -COS3_0);
    BF(5, 6, -COS3_1);

    BF(8, 11, COS3_0);
    BF(9, 10, COS3_1);

    BF(12, 15, -COS3_0);
    BF(13, 14, -COS3_1);

    BF(16, 19, COS3_0);
    BF(17, 18, COS3_1);

    BF(20, 23, -COS3_0);
    BF(21, 22, -COS3_1);

    BF(24, 27, COS3_0);
    BF(25, 26, COS3_1);

    BF(28, 31, -COS3_0);
    BF(29, 30, -COS3_1);

    /* pass 5 */
    BF1(0, 1, 2, 3);
    BF2(4, 5, 6, 7);
    BF1(8, 9, 10, 11);
    BF2(12, 13, 14, 15);
    BF1(16, 17, 18, 19);
    BF2(20, 21, 22, 23);
    BF1(24, 25, 26, 27);
    BF2(28, 29, 30, 31);

    /* pass 6 */

    ADD( 8, 12);
    ADD(12, 10);
    ADD(10, 14);
    ADD(14,  9);
    ADD( 9, 13);
    ADD(13, 11);
    ADD(11, 15);

    out[ 0] = tab[0];
    out[16] = tab[1];
    out[ 8] = tab[2];
    out[24] = tab[3];
    out[ 4] = tab[4];
    out[20] = tab[5];
    out[12] = tab[6];
    out[28] = tab[7];
    out[ 2] = tab[8];
    out[18] = tab[9];
    out[10] = tab[10];
    out[26] = tab[11];
    out[ 6] = tab[12];
    out[22] = tab[13];
    out[14] = tab[14];
    out[30] = tab[15];

    ADD(24, 28);
    ADD(28, 26);
    ADD(26, 30);
    ADD(30, 25);
    ADD(25, 29);
    ADD(29, 27);
    ADD(27, 31);

    out[ 1] = tab[16] + tab[24];
    out[17] = tab[17] + tab[25];
    out[ 9] = tab[18] + tab[26];
    out[25] = tab[19] + tab[27];
    out[ 5] = tab[20] + tab[28];
    out[21] = tab[21] + tab[29];
    out[13] = tab[22] + tab[30];
    out[29] = tab[23] + tab[31];
    out[ 3] = tab[24] + tab[20];
    out[19] = tab[25] + tab[21];
    out[11] = tab[26] + tab[22];
    out[27] = tab[27] + tab[23];
    out[ 7] = tab[28] + tab[18];
    out[23] = tab[29] + tab[19];
    out[15] = tab[30] + tab[17];
    out[31] = tab[31];
}
/* signed 16x16 -> 32 multiply add accumulate */
#define MACS(rt, ra, rb) rt += (ra) * (rb)

/* signed 16x16 -> 32 multiply */
#define MULS(ra, rb) ((ra) * (rb))

#define OUT_SHIFT (WFRAC_BITS + FRAC_BITS - 15)

static int round_sample(int sum)
{
    int sum1;
    sum1 = (sum + (1 << (OUT_SHIFT - 1))) >> OUT_SHIFT;
    if (sum1 < -32768)
        sum1 = -32768;
    else if (sum1 > 32767)
        sum1 = 32767;
    //printf("smpls 1:%d\r\n",sum1);
    return sum1;
}
#define SUM8(sum, op, w, p)                        \
    {                                            \
        sum op MULS((w)[0 * 64], p[0 * 64]);    \
        sum op MULS((w)[1 * 64], p[1 * 64]);    \
        sum op MULS((w)[2 * 64], p[2 * 64]);    \
        sum op MULS((w)[3 * 64], p[3 * 64]);    \
        sum op MULS((w)[4 * 64], p[4 * 64]);    \
        sum op MULS((w)[5 * 64], p[5 * 64]);    \
        sum op MULS((w)[6 * 64], p[6 * 64]);    \
        sum op MULS((w)[7 * 64], p[7 * 64]);    \
    }

#define SUM8P2(sum1, op1, sum2, op2, w1, w2, p) \
    {                                            \
        int tmp;                                \
        tmp = p[0 * 64];                        \
        sum1 op1 MULS((w1)[0 * 64], tmp);        \
        sum2 op2 MULS((w2)[0 * 64], tmp);        \
        tmp = p[1 * 64];                        \
        sum1 op1 MULS((w1)[1 * 64], tmp);        \
        sum2 op2 MULS((w2)[1 * 64], tmp);        \
        tmp = p[2 * 64];                        \
        sum1 op1 MULS((w1)[2 * 64], tmp);        \
        sum2 op2 MULS((w2)[2 * 64], tmp);        \
        tmp = p[3 * 64];                        \
        sum1 op1 MULS((w1)[3 * 64], tmp);        \
        sum2 op2 MULS((w2)[3 * 64], tmp);        \
        tmp = p[4 * 64];                        \
        sum1 op1 MULS((w1)[4 * 64], tmp);        \
        sum2 op2 MULS((w2)[4 * 64], tmp);        \
        tmp = p[5 * 64];                        \
        sum1 op1 MULS((w1)[5 * 64], tmp);        \
        sum2 op2 MULS((w2)[5 * 64], tmp);        \
        tmp = p[6 * 64];                        \
        sum1 op1 MULS((w1)[6 * 64], tmp);        \
        sum2 op2 MULS((w2)[6 * 64], tmp);        \
        tmp = p[7 * 64];                        \
        sum1 op1 MULS((w1)[7 * 64], tmp);        \
        sum2 op2 MULS((w2)[7 * 64], tmp);        \
    }

int unaligned32_be(const void *v)
{
    return SWAP32( (*(uint32_t*)(v))); //original
}

void init_get_bits(GetBitContext *s,
                   const uint8_t *buffer, int bit_size)
{
    const int buffer_size= (bit_size+7)>>3;

    s->buffer= buffer;
    s->size_in_bits= bit_size;
    s->buffer_end= buffer + buffer_size;
    s->index=0;
#if 0
    {
        OPEN_READER(re, s)
            UPDATE_CACHE(re, s)
            UPDATE_CACHE(re, s)
            CLOSE_READER(re, s)
            }
#endif
}

void skip_bits(GetBitContext *s, int n)
{
    //Note gcc seems to optimize this to s->index+=n for the ALT_READER :))
#if 1
    s->index+=n;
#else
    OPEN_READER(re, s)
        UPDATE_CACHE(re, s)
        LAST_SKIP_BITS(re, s, n)
        CLOSE_READER(re, s)
#endif
        }


int get_bits_count(GetBitContext *s)
{
    return s->index;
}
unsigned int get_bits1(GetBitContext *s)
{
    int index= s->index;
    uint8_t result= s->buffer[ index>>3 ];
    result<<= (index&0x07);
    result>>= 8 - 1;
    index++;
    s->index= index;
    return result;
}
unsigned int get_bits(GetBitContext *s, int n){
#if 0
    register int tmp;
    OPEN_READER(re, s)
        UPDATE_CACHE(re, s)
        tmp= SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n)
        CLOSE_READER(re, s)
#else
        uint32_t tmp=0;
    uint16_t i;
    for(i=0;i<n;i++)
    {
        tmp |= get_bits1(s);
        tmp <<= 1;
        //tmp |= (get_bits1(s)<<i);
    }
    tmp>>=1;
#endif
    //printf("bits:%x\n",tmp);
    return tmp;
}




/* 32 sub band synthesis filter. Input: 32 sub band samples, Output:
   32 samples. */
/* XXX: optimize by avoiding ring buffer usage */
static void synth_filter(L2DecodeContext *s1,
                         int ch, int16_t *samples, int incr,
                         int32_t sb_samples[SBLIMIT])
{
    int32_t tmp[32];
    register int16_t *synth_buf;
    register const int16_t *w, *w2, *p;
    int j, offset, v;
    int16_t *samples2;
#if FRAC_BITS <= 15
    int sum, sum2;
#else
    int64_t sum, sum2;
#endif

    dct32(tmp, sb_samples);
    offset = s1->synth_buf_offset[ch];
    synth_buf = s1->synth_buf + ch*1024 + offset;

    for(j=0;j<32;j++) {
        v = tmp[j];
#if FRAC_BITS <= 15
        /* NOTE: can cause a loss in precision if very high amplitude
           sound */
        if (v > 32767)
            v = 32767;
        else if (v < -32768)
            v = -32768;
#endif
        synth_buf[j] = v;
    }
    /* copy to avoid wrap */
    memcpy(synth_buf + 512, synth_buf, 32 * sizeof(int16_t));

    samples2 = samples + 31 * incr;
    w = window;
    w2 = window + 31;

    sum = 0;
    p = synth_buf + 16;
    SUM8(sum, +=, w, p);
    p = synth_buf + 48;
    SUM8(sum, -=, w + 32, p);
    *samples = round_sample(sum);
    samples += incr;
    w++;

    /* we calculate two samples at the same time to avoid one memory
       access per two sample */
    for(j=1;j<16;j++) {
        sum = 0;
        sum2 = 0;
        p = synth_buf + 16 + j;
        SUM8P2(sum, +=, sum2, -=, w, w2, p);
        p = synth_buf + 48 - j;
        SUM8P2(sum, -=, sum2, -=, w + 32, w2 + 32, p);

        *samples = round_sample(sum);
        samples += incr;
        *samples2 = round_sample(sum2);
        samples2 -= incr;
        w++;
        w2--;
    }

    p = synth_buf + 32;
    sum = 0;
    SUM8(sum, -=, w + 32, p);
    *samples = round_sample(sum);

    offset = (offset - 32) & 511;
    s1->synth_buf_offset[ch] = offset;
}


/* layer 1 unscaling */
/* n = number of bits of the mantissa minus 1 */
static int l1_unscale(int n, int mant, int scale_factor)
{
    int shift, mod;
    int64_t val;

    shift = scale_factor_modshift[scale_factor];
    mod = shift & 3;
    shift >>= 2;
    val = MUL64(mant + (-1 << n) + 1, scale_factor_mult[n-1][mod]);
    shift += n;
    /* NOTE: at this point, 1 <= shift >= 21 + 15 */
    return (int)((val + (1L << (shift - 1))) >> shift);
}

static int l2_unscale_group(int steps, int mant, int scale_factor)
{
    int shift, mod, val;

    shift = scale_factor_modshift[scale_factor];
    mod = shift & 3;
    shift >>= 2;

    val = (mant - (steps >> 1)) * scale_factor_mult2[steps >> 2][mod];
    /* NOTE: at this point, 0 <= shift <= 21 */
    if (shift > 0)
        val = (val + (1 << (shift - 1))) >> shift;
    return val;
}
/* fast header check for resync */
static int check_header(uint32_t header)
{
    /* header */
    if ((header & 0xffe00000) != 0xffe00000)
        return -1;
    /* layer check */
    if (((header >> 17) & 3) == 0)
        return -1;
    /* bit rate */
    if (((header >> 12) & 0xf) == 0xf)
        return -1;
    /* frequency */
    if (((header >> 10) & 3) == 3)
        return -1;
    return 0;
}

/* header + layer + bitrate + freq + lsf/mpeg25 */
#define SAME_HEADER_MASK                                            \
    (0xffe00000 | (3 << 17) | (0xf << 12) | (3 << 10) | (3 << 19))

/* header decoding. MUST check the header before because no
   consistency check is done there. Return 1 if free format found and
   that the frame size must be computed externally */
static int decode_header(L2DecodeContext *s, uint32_t header)
{
    int sample_rate, frame_size, mpeg25, padding;
    int sample_rate_index, bitrate_index;
    if (header & (1<<20)) {
        s->lsf = (header & (1<<19)) ? 0 : 1;
        mpeg25 = 0;
    } else {
        s->lsf = 1;
        mpeg25 = 1;
    }

    s->layer = 4 - ((header >> 17) & 3);
    /* extract frequency */
    sample_rate_index = (header >> 10) & 3;
    sample_rate = mpa_freq_tab[sample_rate_index] >> (s->lsf + mpeg25);
    sample_rate_index += 3 * (s->lsf + mpeg25);
    s->sample_rate_index = sample_rate_index;
    s->error_protection = ((header >> 16) & 1) ^ 1;
    s->sample_rate = sample_rate;

    bitrate_index = (header >> 12) & 0xf;
    padding = (header >> 9) & 1;
    s->mode = (header >> 6) & 3;
    s->mode_ext = (header >> 4) & 3;

    if (s->mode == MPA_MONO)
        s->nb_channels = 1;
    else
        s->nb_channels = 2;

    if (bitrate_index != 0) {
        frame_size = mpa_bitrate_tab[s->lsf][s->layer - 1][bitrate_index];
        s->bit_rate = frame_size * 1000;
        switch(s->layer) {
        case 1:
            frame_size = (frame_size * 12000) / sample_rate;
            frame_size = (frame_size + padding) * 4;
            break;
        case 2:
            frame_size = (frame_size * 144000) / sample_rate;
            frame_size += padding;
            break;
        default:
        case 3:
            frame_size = (frame_size * 144000) / (sample_rate << s->lsf);
            frame_size += padding;
            break;
        }
        s->frame_size = frame_size;
    } else {
        /* if no frame size computed, signal it */
        if (!s->free_format_frame_size)
            return 1;
        /* free format: compute bitrate and real frame size from the
           frame size we extracted by reading the bitstream */
        s->frame_size = s->free_format_frame_size;
        switch(s->layer) {
        case 1:
            s->frame_size += padding  * 4;
            s->bit_rate = (s->frame_size * sample_rate) / 48000;
            break;
        case 2:
            s->frame_size += padding;
            s->bit_rate = (s->frame_size * sample_rate) / 144000;
            break;
        default:
        case 3:
            s->frame_size += padding;
            s->bit_rate = (s->frame_size * (sample_rate << s->lsf)) / 144000;
            break;
        }
    }

    return 0;
}

/* useful helper to get mpeg audio stream infos. Return -1 if error in
   header, otherwise the coded frame size in bytes */
int mpa_decode_header(L2DecodeContext *s, uint32_t head)
{
    uint32_t nHead = SWAP32(head);


    if (check_header(nHead) != 0)
    {
        printf("check_header Error\n");
        return -1;
    }

    if (decode_header(s, nHead) != 0)
    {
        return -1;
    }
    return s->frame_size;
}
/* return the number of decoded frames */
static int mp_decode_layer1(L2DecodeContext *s)
{
    int bound, i, v=0, n, ch, j, mant;
    uint8_t allocation[MPA_MAX_CHANNELS][SBLIMIT];
    uint8_t scale_factors[MPA_MAX_CHANNELS][SBLIMIT];

    if (s->mode == MPA_JSTEREO)
        bound = (s->mode_ext + 1) * 4;
    else
        bound = SBLIMIT;

    /* allocation bits */
    for(i=0;i<bound;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            allocation[ch][i] = get_bits(&s->gb, 4);
        }
    }
    for(i=bound;i<SBLIMIT;i++) {
        allocation[0][i] = get_bits(&s->gb, 4);
    }

    /* scale factors */
    for(i=0;i<bound;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (allocation[ch][i])
                scale_factors[ch][i] = get_bits(&s->gb, 6);
        }
    }
    for(i=bound;i<SBLIMIT;i++) {
        if (allocation[0][i]) {
            scale_factors[0][i] = get_bits(&s->gb, 6);
            scale_factors[1][i] = get_bits(&s->gb, 6);
        }
    }

    /* compute samples */
    for(j=0;j<12;j++) {
        for(i=0;i<bound;i++) {
            for(ch=0;ch<s->nb_channels;ch++) {
                n = allocation[ch][i];
                if (n) {
                    mant = get_bits(&s->gb, n + 1);
                    v = l1_unscale(n, mant, scale_factors[ch][i]);
                } else {
                    v = 0;
                }
                //s->sb_samples[ch][j][i] = v;
                s->sb_samples[ch*12*32+j*32+i] = v;
            }
        }
        for(i=bound;i<SBLIMIT;i++) {
            n = allocation[0][i];
            if (n) {
                mant = get_bits(&s->gb, n + 1);
                v = l1_unscale(n, mant, scale_factors[0][i]);
                //s->sb_samples[0][j][i] = v;
                s->sb_samples[0*12*32+j*32+i] = v;
                v = l1_unscale(n, mant, scale_factors[1][i]);
                //s->sb_samples[1][j][i] = v;
                s->sb_samples[1*12*32+j*32+i] = v;
            } else {
                //s->sb_samples[0][j][i] = 0;
                s->sb_samples[0*12*32+j*32+i] = v;
                //s->sb_samples[1][j][i] = 0;
                s->sb_samples[1*12*32+j*32+i] = v;
            }
        }
    }
    return 12;
}

/* bitrate is in kb/s */
int l2_select_table(int bitrate, int nb_channels, int freq, int lsf)
{
    int ch_bitrate, table;

    ch_bitrate = bitrate / nb_channels;
    if (!lsf) {
        if ((freq == 48000 && ch_bitrate >= 56) ||
            (ch_bitrate >= 56 && ch_bitrate <= 80))
            table = 0;
        else if (freq != 48000 && ch_bitrate >= 96)
            table = 1;
        else if (freq != 32000 && ch_bitrate <= 48)
            table = 2;
        else
            table = 3;
    } else {
        table = 4;
    }
    return table;
}

static int mp_decode_layer2(L2DecodeContext *s)
{
    int sblimit; /* number of used subbands */
    const unsigned char *alloc_table;
    int table, bit_alloc_bits, i, j, ch, bound, v;
    unsigned char bit_alloc[MPA_MAX_CHANNELS][SBLIMIT];
    unsigned char scale_code[MPA_MAX_CHANNELS][SBLIMIT];
    unsigned char scale_factors[MPA_MAX_CHANNELS][SBLIMIT][3], *sf;
    int scale, qindex, bits, steps, k, l, m, b;

    /* select decoding table */
    table = l2_select_table(s->bit_rate / 1000, s->nb_channels,
                            s->sample_rate, s->lsf);
    sblimit = sblimit_table[table];
    alloc_table = alloc_tables[table];

    if (s->mode == MPA_JSTEREO)
        bound = (s->mode_ext + 1) * 4;
    else
        bound = sblimit;


    /* sanity check */
    if( bound > sblimit ) bound = sblimit;

    /* parse bit allocation */
    j = 0;
    for(i=0;i<bound;i++) {
        bit_alloc_bits = alloc_table[j];
        for(ch=0;ch<s->nb_channels;ch++) {
            bit_alloc[ch][i] = get_bits(&s->gb, bit_alloc_bits);
        }
        j += 1 << bit_alloc_bits;
    }
    for(i=bound;i<sblimit;i++) {
        bit_alloc_bits = alloc_table[j];
        v = get_bits(&s->gb, bit_alloc_bits);
        bit_alloc[0][i] = v;
        bit_alloc[1][i] = v;
        j += 1 << bit_alloc_bits;
    }

    /* scale codes */
    for(i=0;i<sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i])
                scale_code[ch][i] = get_bits(&s->gb, 2);
        }
    }

    /* scale factors */
    for(i=0;i<sblimit;i++) {
        for(ch=0;ch<s->nb_channels;ch++) {
            if (bit_alloc[ch][i]) {
                sf = scale_factors[ch][i];
                switch(scale_code[ch][i]) {
                default:
                case 0:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[1] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    break;
                case 2:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[1] = sf[0];
                    sf[2] = sf[0];
                    break;
                case 1:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    sf[1] = sf[0];
                    break;
                case 3:
                    sf[0] = get_bits(&s->gb, 6);
                    sf[2] = get_bits(&s->gb, 6);
                    sf[1] = sf[2];
                    break;
                }
            }
        }
    }

    /* samples */
    for(k=0;k<3;k++)
    {
        for(l=0;l<12;l+=3)
        {
            j = 0;
            for(i=0;i<bound;i++)
            {
                bit_alloc_bits = alloc_table[j];
                for(ch=0;ch<s->nb_channels;ch++)
                {
                    b = bit_alloc[ch][i];
                    if (b)
                    {
                        scale = scale_factors[ch][i][k];
                        qindex = alloc_table[j+b];
                        bits = quant_bits[qindex];
                        if (bits < 0) {
                            /* 3 values at the same time */
                            v = get_bits(&s->gb, -bits);
                            steps = quant_steps[qindex];
                            //s->sb_samples[ch][k * 12 + l + 0][i] =
                            s->sb_samples[ch*36*32+(k * 12 + l + 0)*32+i] =
                                l2_unscale_group(steps, v % steps, scale);
                            v = v / steps;
                            //s->sb_samples[ch][k * 12 + l + 1][i] =
                            s->sb_samples[ch*36*32+(k * 12 + l + 1)*32+i] =
                                l2_unscale_group(steps, v % steps, scale);
                            v = v / steps;
                            //s->sb_samples[ch][k * 12 + l + 2][i] =
                            s->sb_samples[ch*36*32+(k * 12 + l + 2)*32+i] =
                                l2_unscale_group(steps, v, scale);
                        } else {
                            for(m=0;m<3;m++) {
                                v = get_bits(&s->gb, bits);
                                v = l1_unscale(bits - 1, v, scale);
                                //s->sb_samples[ch][k * 12 + l + m][i] = v;
                                s->sb_samples[ch*36*32+(k * 12 + l + m)*32+i] = v;
                            }
                        }
                    } else {
                        //s->sb_samples[ch][k * 12 + l + 0][i] = 0;
                        s->sb_samples[ch*36*32+(k * 12 + l + 0)*32+i] = 0;
                        //s->sb_samples[ch][k * 12 + l + 1][i] = 0;
                        s->sb_samples[ch*36*32+(k * 12 + l + 1)*32+i] = 0;
                        //s->sb_samples[ch][k * 12 + l + 2][i] = 0;
                        s->sb_samples[ch*36*32+(k * 12 + l + 2)*32+i] = 0;
                    }
                }
                /* next subband in alloc table */
                j += 1 << bit_alloc_bits;
            }
            /* XXX: find a way to avoid this duplication of code */
            for(i=bound;i<sblimit;i++) {
                bit_alloc_bits = alloc_table[j];
                b = bit_alloc[0][i];
                if (b) {
                    int mant, scale0, scale1;
                    scale0 = scale_factors[0][i][k];
                    scale1 = scale_factors[1][i][k];
                    qindex = alloc_table[j+b];
                    bits = quant_bits[qindex];
                    if (bits < 0) {
                        /* 3 values at the same time */
                        v = get_bits(&s->gb, -bits);
                        steps = quant_steps[qindex];
                        mant = v % steps;
                        v = v / steps;
                        //s->sb_samples[0][k * 12 + l + 0][i] =
                        s->sb_samples[0*36*32+(k * 12 + l + 0)*32+i] =
                            l2_unscale_group(steps, mant, scale0);
                        //s->sb_samples[1][k * 12 + l + 0][i] =
                        s->sb_samples[1*36*32+(k * 12 + l + 0)*32+i] =
                            l2_unscale_group(steps, mant, scale1);
                        mant = v % steps;
                        v = v / steps;
                        //s->sb_samples[0][k * 12 + l + 1][i] =
                        s->sb_samples[0*36*32+(k * 12 + l + 1)*32+i] =
                            l2_unscale_group(steps, mant, scale0);
                        //s->sb_samples[1][k * 12 + l + 1][i] =
                        s->sb_samples[1*36*32+(k * 12 + l + 1)*32+i] =
                            l2_unscale_group(steps, mant, scale1);
                        //s->sb_samples[0][k * 12 + l + 2][i] =
                        s->sb_samples[0*36*32+(k * 12 + l + 2)*32+i] =
                            l2_unscale_group(steps, v, scale0);
                        //s->sb_samples[1][k * 12 + l + 2][i] =
                        s->sb_samples[1*36*32+(k * 12 + l + 2)*32+i] =
                            l2_unscale_group(steps, v, scale1);
                    } else {
                        for(m=0;m<3;m++) {
                            mant = get_bits(&s->gb, bits);
                            //s->sb_samples[0][k * 12 + l + m][i] =
                            s->sb_samples[0*36*32+(k * 12 + l + m)*32+i] =
                                l1_unscale(bits - 1, mant, scale0);
                            //s->sb_samples[1][k * 12 + l + m][i] =
                            s->sb_samples[1*36*32+(k * 12 + l + m)*32+i] =
                                l1_unscale(bits - 1, mant, scale1);
                        }
                    }
                } else {
                    //s->sb_samples[0][k * 12 + l + 0][i] = 0;
                    s->sb_samples[0*36*32+(k * 12 + l + 0)*32+i] = 0;
                    //s->sb_samples[0][k * 12 + l + 1][i] = 0;
                    s->sb_samples[0*36*32+(k * 12 + l + 1)*32+i] = 0;
                    //s->sb_samples[0][k * 12 + l + 2][i] = 0;
                    s->sb_samples[0*36*32+(k * 12 + l + 2)*32+i] = 0;
                    //s->sb_samples[1][k * 12 + l + 0][i] = 0;
                    s->sb_samples[1*36*32+(k * 12 + l + 0)*32+i] = 0;
                    //s->sb_samples[1][k * 12 + l + 1][i] = 0;
                    s->sb_samples[1*36*32+(k * 12 + l + 1)*32+i] = 0;
                    //s->sb_samples[1][k * 12 + l + 2][i] = 0;
                    s->sb_samples[1*36*32+(k * 12 + l + 2)*32+i] = 0;
                }
                /* next subband in alloc table */
                j += 1 << bit_alloc_bits;
            }
            /* fill remaining samples to zero */
            for(i=sblimit;i<SBLIMIT;i++) {
                for(ch=0;ch<s->nb_channels;ch++) {
                    //s->sb_samples[ch][k * 12 + l + 0][i] = 0;
                    s->sb_samples[ch*36*32+(k * 12 + l + 0)*32+i] = 0;
                    //s->sb_samples[ch][k * 12 + l + 1][i] = 0;
                    s->sb_samples[ch*36*32+(k * 12 + l + 1)*32+i] = 0;
                    //s->sb_samples[ch][k * 12 + l + 2][i] = 0;
                    s->sb_samples[ch*36*32+(k * 12 + l + 2)*32+i] = 0;
                }
            }
        }
    }
    return 3 * 12;
}
static int mp_decode_frame(L2DecodeContext *s,
                           short *samples)
{
    int i, nb_frames=0, ch;
    short *samples_ptr;

    init_get_bits(&s->gb, s->inbuf + HEADER_SIZE,
                  (s->inbuf_ptr - s->inbuf - HEADER_SIZE)*8);

    /* skip error protection field */
    if (s->error_protection)
        get_bits(&s->gb, 16);

    switch(s->layer)
    {
    case 1:
        nb_frames = mp_decode_layer1(s);
        break;
    case 2:
        nb_frames = mp_decode_layer2(s);
        break;
    case 3:
    default:
        break;
    }
    /* apply the synthesis filter */
    for(ch=0;ch<s->nb_channels;ch++)
    {
        samples_ptr = samples + ch;
        for(i=0;i<nb_frames;i++)
        {
            synth_filter(s, ch, samples_ptr, s->nb_channels,
                         s->sb_samples+ch*nb_frames*32+i*32);
            samples_ptr += 32 * s->nb_channels;
        }
    }
    return nb_frames * 32 * s->nb_channels;
}
int L2_decode_frame(L2DecodeContext *l2Dec,
                    void *data, int *data_size,
                    uint8_t * buf, int buf_size)
{
    L2DecodeContext *s = (L2DecodeContext *)(l2Dec);

    uint32_t header;
    short *out_samples = data;
    memcpy(&header,buf,4);
    if(mpa_decode_header(s, header)<=0)
    {
        *data_size = 0;
        return 0;
    }
    s->inbuf = buf;
    s->inbuf_ptr = buf + buf_size;

    *data_size = mp_decode_frame(s, out_samples);

    return *data_size;
}

uint32_t L2_GetFrameSize(L2DecodeContext *s,uint32_t head)
{
    uint32_t nHead = SWAP32(head);
    
    if (check_header(nHead) != 0)
        return 0;

    if (decode_header(s, nHead) != 0)
    {
        return 0;
    }
    return s->frame_size;
}
#endif /*CONFIG_APP_MP3PLAYER*/
