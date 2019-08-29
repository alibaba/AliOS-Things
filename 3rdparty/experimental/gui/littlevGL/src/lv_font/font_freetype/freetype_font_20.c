#include "lvgl.h"
#include "aos/kernel.h"

#if USE_FREETYPE != 0

#include "ft2build.h"
#include "freetype.h"

extern bool get_freetype_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);
extern const uint8_t * get_freetype_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter);

lv_font_t freetype_font_20 =
{
        .get_glyph_dsc     = get_freetype_glyph_dsc_cb,
        .get_glyph_bitmap  = get_freetype_glyph_bitmap_cb,
        .line_height       = 20,
        .base_line         = 0,
        .dsc               = NULL
};

#endif