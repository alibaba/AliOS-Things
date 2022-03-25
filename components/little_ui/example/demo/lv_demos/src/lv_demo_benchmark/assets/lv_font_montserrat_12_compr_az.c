#include "../../lv_demo.h"

/*******************************************************************************
 * Size: 12 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifndef LV_FONT_BENCHMARK_MONTSERRAT_12_COMPR_AZ
#define LV_FONT_BENCHMARK_MONTSERRAT_12_COMPR_AZ 1
#endif

#if LV_FONT_BENCHMARK_MONTSERRAT_12_COMPR_AZ

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t gylph_bitmap[] = {
    /* U+20 " " */

    /* U+61 "a" */
    0x8, 0xdf, 0xc3, 0x0, 0x14, 0xea, 0xf0, 0x2,
    0x90, 0xa0, 0x84, 0x23, 0x7b, 0xc0, 0xd1, 0xb3,
    0xb8, 0x1, 0x84, 0x1c, 0x0, 0x8f, 0xdb, 0xe0,
    0x0,

    /* U+62 "b" */
    0xe4, 0x0, 0xff, 0xe4, 0x1e, 0x7e, 0x20, 0x2,
    0x27, 0x52, 0xc8, 0x25, 0x4a, 0x12, 0x80, 0x80,
    0x23, 0x20, 0x20, 0x8, 0xc8, 0x25, 0x4a, 0x12,
    0x82, 0x63, 0x52, 0xc8,

    /* U+63 "c" */
    0x2, 0xbf, 0xe8, 0x0, 0x78, 0xed, 0x21, 0xa8,
    0x51, 0x26, 0x19, 0x30, 0x7, 0x13, 0x0, 0x72,
    0x85, 0x12, 0x69, 0x87, 0x8e, 0xd2, 0x98,

    /* U+64 "d" */
    0x0, 0xe1, 0xf1, 0x0, 0xff, 0xe2, 0x95, 0xfe,
    0xb8, 0x5, 0x85, 0xbd, 0x60, 0x5, 0x39, 0x23,
    0x90, 0x1, 0x30, 0x4, 0xa0, 0x2, 0x60, 0x8,
    0x80, 0xa, 0x72, 0x25, 0xc0, 0x16, 0x17, 0x65,
    0x80, 0x0,

    /* U+65 "e" */
    0x2, 0xbf, 0xd5, 0x0, 0x61, 0xf4, 0xda, 0x29,
    0x40, 0xa4, 0x79, 0x1f, 0x75, 0xe, 0x44, 0xee,
    0xd8, 0xa7, 0x24, 0x74, 0x41, 0x85, 0xba, 0x32,

    /* U+66 "f" */
    0x1, 0xcf, 0x60, 0x86, 0xe7, 0x2, 0xc1, 0x16,
    0xa4, 0xf9, 0xea, 0x4f, 0x98, 0x7, 0xff, 0x34,

    /* U+67 "g" */
    0x2, 0xbf, 0xe6, 0xe2, 0xc, 0x1d, 0xd5, 0x8,
    0x29, 0xd1, 0x1f, 0x0, 0x9, 0x80, 0x22, 0x0,
    0x13, 0x0, 0x44, 0x0, 0x53, 0xa2, 0x3f, 0x0,
    0xb0, 0x77, 0x50, 0x1, 0x15, 0xff, 0x38, 0x10,
    0x63, 0x11, 0xca, 0x80, 0x2a, 0xf7, 0x43, 0x60,

    /* U+68 "h" */
    0xe4, 0x0, 0xff, 0xe4, 0x1e, 0xfe, 0x10, 0x2,
    0x65, 0xad, 0xc0, 0x8, 0x42, 0x83, 0x0, 0x10,
    0x0, 0x80, 0x40, 0x3c, 0x60, 0x1f, 0xfc, 0x20,

    /* U+69 "i" */
    0xd, 0x40, 0x56, 0x8, 0x20, 0xe4, 0x0, 0xff,
    0xe3, 0x0,

    /* U+6A "j" */
    0x0, 0xb5, 0x40, 0x2a, 0x70, 0x9, 0xc8, 0x2,
    0xd4, 0x0, 0xff, 0xec, 0x11, 0x4c, 0x1f, 0xae,
    0x40,

    /* U+6B "k" */
    0xe4, 0x0, 0xff, 0xe5, 0xdd, 0x0, 0x6b, 0xa,
    0x0, 0xad, 0xec, 0x2, 0xa4, 0x10, 0xc, 0x5c,
    0x12, 0x1, 0x40, 0xfb, 0x20, 0x6, 0x29, 0xa1,

    /* U+6C "l" */
    0xe4, 0x0, 0xff, 0xe3, 0x0,

    /* U+6D "m" */
    0xe7, 0xdf, 0xb2, 0x9e, 0xe4, 0x0, 0x2b, 0xb5,
    0xf4, 0xb3, 0x59, 0x42, 0xcc, 0xa0, 0x28, 0x8d,
    0xf0, 0x8, 0x0, 0xe0, 0x20, 0x1, 0x30, 0xf,
    0xfe, 0xa8,

    /* U+6E "n" */
    0xe6, 0xdf, 0xc2, 0x0, 0x44, 0xf3, 0x70, 0x2,
    0x10, 0x6c, 0xc0, 0x4, 0x0, 0x10, 0x10, 0xf,
    0x18, 0x7, 0xff, 0x8,

    /* U+6F "o" */
    0x2, 0xbf, 0xe8, 0x0, 0xb0, 0x77, 0x4d, 0x0,
    0xa7, 0x44, 0x74, 0xe0, 0x4c, 0x1, 0x28, 0x9,
    0x30, 0x4, 0xa0, 0x2a, 0x14, 0x47, 0x4e, 0x0,
    0xf1, 0xdd, 0x34, 0x0,

    /* U+70 "p" */
    0xe7, 0xdf, 0xc4, 0x0, 0x44, 0xf5, 0x59, 0x4,
    0xa0, 0xb2, 0xd0, 0x10, 0x4, 0x44, 0x3, 0x0,
    0x8c, 0x82, 0x14, 0xa1, 0x28, 0x26, 0x5a, 0x96,
    0x40, 0x59, 0xf8, 0x80, 0x1f, 0xfc, 0x30,

    /* U+71 "q" */
    0x2, 0xbf, 0xd5, 0xf1, 0xc, 0x1d, 0xeb, 0x0,
    0x29, 0xd1, 0x1d, 0x0, 0x9, 0x80, 0x25, 0x0,
    0x13, 0x0, 0x4a, 0x0, 0x53, 0xa2, 0x3a, 0x0,
    0xb0, 0x77, 0xac, 0x2, 0x2b, 0xfd, 0x40, 0xf,
    0xfe, 0x30,

    /* U+72 "r" */
    0xe6, 0xd8, 0x8, 0xb9, 0x9, 0x61, 0x3, 0x0,
    0xff, 0xe1, 0x0,

    /* U+73 "s" */
    0x9, 0xef, 0xc2, 0x64, 0xcd, 0xa3, 0x10, 0x1,
    0x30, 0xba, 0x6f, 0x48, 0x84, 0xfe, 0x2c, 0x2b,
    0x10, 0x38, 0x8a, 0xb7, 0x6, 0x0,

    /* U+74 "t" */
    0x5, 0x30, 0xb, 0x94, 0x1, 0xa9, 0x3e, 0x7a,
    0x93, 0xe6, 0x1, 0xff, 0xc4, 0x1c, 0x11, 0x4,
    0x3f, 0x38,

    /* U+75 "u" */
    0xf3, 0x0, 0x17, 0x80, 0x7f, 0xf2, 0x85, 0xc0,
    0xc, 0x0, 0x4f, 0x22, 0x40, 0x2, 0xc3, 0x72,
    0x80, 0x0,

    /* U+76 "v" */
    0xd, 0x50, 0xa, 0x60, 0x2e, 0x40, 0x26, 0xa0,
    0x73, 0x20, 0x64, 0x20, 0x15, 0xa0, 0xae, 0x0,
    0xa5, 0x8c, 0x94, 0x2, 0x33, 0x73, 0x0, 0x75,
    0x95, 0x0, 0x0,

    /* U+77 "w" */
    0xc5, 0x0, 0x1f, 0x88, 0x1, 0xe2, 0xbc, 0x1,
    0x40, 0xe0, 0xa, 0xb7, 0x50, 0x3, 0xc6, 0x81,
    0x1, 0x8a, 0x2a, 0x13, 0xa8, 0xd2, 0x80, 0x37,
    0xf6, 0x81, 0x1d, 0x6c, 0x0, 0xea, 0xee, 0xe,
    0x80, 0x30, 0x0, 0xa2, 0x88, 0x20, 0x28, 0x0,

    /* U+78 "x" */
    0x5d, 0x0, 0x27, 0xa, 0xbc, 0x8d, 0x50, 0x42,
    0xdf, 0x1d, 0x0, 0x2, 0xc5, 0x0, 0x11, 0x32,
    0x58, 0x5, 0xd3, 0x6e, 0xc0, 0xe0, 0xe1, 0xd2,
    0x40,

    /* U+79 "y" */
    0xd, 0x50, 0xa, 0x60, 0x2b, 0x80, 0x27, 0xa0,
    0x64, 0x20, 0x52, 0x20, 0xa, 0xa8, 0x3a, 0x40,
    0x2e, 0xa1, 0x55, 0x0, 0x48, 0x74, 0xe0, 0x1c,
    0xc9, 0x40, 0x1d, 0xa4, 0x60, 0x12, 0x94, 0xd8,
    0x4, 0x33, 0xaa, 0xc0, 0x10,

    /* U+7A "z" */
    0x7f, 0xfd, 0x6f, 0xfe, 0xd0, 0x80, 0xb, 0xa0,
    0xc0, 0x12, 0xee, 0x0, 0x23, 0xd0, 0x0, 0x6a,
    0xc4, 0x1, 0x23, 0x5f, 0xed
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 52, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 115, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 131, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 110, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 131, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 110, .adv_w = 118, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 68, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 132, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 190, .adv_w = 131, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 54, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 55, .box_w = 5, .box_h = 13, .ofs_x = -2, .ofs_y = -3},
    {.bitmap_index = 241, .adv_w = 118, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 265, .adv_w = 54, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 203, .box_w = 11, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 131, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 316, .adv_w = 122, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 344, .adv_w = 131, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 375, .adv_w = 131, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 409, .adv_w = 79, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 79, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 130, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 107, .box_w = 8, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 505, .adv_w = 173, .box_w = 11, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 106, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 570, .adv_w = 107, .box_w = 8, .box_h = 10, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 607, .adv_w = 100, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 97, .range_length = 26, .glyph_id_start = 2,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 1, 2, 3, 4, 5, 6,
    0, 1, 0, 0, 7, 4, 1, 1,
    2, 2, 8, 9, 10, 11, 0, 12,
    12, 13, 12, 14
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 3, 3, 3, 0,
    3, 2, 4, 5, 2, 2, 4, 4,
    3, 4, 3, 4, 6, 7, 8, 9,
    9, 10, 9, 11
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 1, 0, 0, 0, 0, 0,
    -2, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, -3, -3, -2, 0, -1,
    -2, 0, 0, 0, 1, 0, -1, -3,
    -1, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, -2, -3, -1, -2,
    0, -2, 15, 8, 0, 0, 0, 2,
    0, 0, -2, 0, -4, -1, 0, -3,
    0, -2, -5, -4, -2, 0, 0, 0,
    0, 10, 0, 0, 0, 0, 0, 0,
    -2, -1, -2, -1, 0, -1, 3, 0,
    3, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -2, 0, 0, 0,
    -3, 0, 0, 0, -2, 0, -2, 0,
    0, -3, 0, -3, 0, -3, -1, 3,
    0, -2, -6, -2, -2, 0, -3, 0,
    1, -2, 0, -2, -6, 0, -2, 0,
    0, -2, 0, 0, 0, 0, 0, -2,
    -2, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 14,
    .right_class_cnt     = 11,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 1
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t lv_font_benchmark_montserrat_12_compr_az = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 13,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};

#endif /*#if LV_FONT_BENCHMARK_MONTSERRAT_12_COMPR_AZ*/

