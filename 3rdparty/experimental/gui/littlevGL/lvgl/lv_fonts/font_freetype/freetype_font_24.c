#include "lvgl/lvgl.h"
#include "aos/kernel.h"

#if USE_FREETYPE != 0

#include "ft2build.h"
#include "freetype.h"

extern const uint8_t * get_freetype_bitmap(const lv_font_t * font, uint32_t unicode_letter);
extern int16_t get_freetype_width(const lv_font_t * font, uint32_t unicode_letter);

lv_font_t freetype_font_24 =
    {
        .unicode_first = 32,                  /* First Unicode letter in this font */
        .unicode_last  = 65535,               /* Last Unicode letter in this font */
        .h_px          = 24,                  /* Font height in pixels */
        .glyph_bitmap  = NULL,                /* Bitmap of glyphs */
        .glyph_dsc     = NULL,                /* Description of glyphs */
        .unicode_list  = NULL,                /* List of unicode characters */
        .get_bitmap    = get_freetype_bitmap, /* Function pointer to get glyph's bitmap */
        .get_width     = get_freetype_width,  /* Function pointer to get glyph's width */
        .bpp           = 8,                   /* Bit per pixel */
        .next_page     = 0,                   /* Pointer to a font extension */
};

#endif