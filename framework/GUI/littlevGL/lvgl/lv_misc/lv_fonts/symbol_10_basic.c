#include "../../../lv_conf.h"
#if USE_LV_FONT_SYMBOL_10_BASIC

#include <stdint.h>
#include "../lv_font.h"

/*Store the image of the letters (glyph) */
static const uint8_t symbol_10_basic_bitmap[] = 
{
    // ASCII: 57344, char width: 10
    0x00, 0x00,  // ----------......
    0xef, 0xc0,  // OOO-OOOOOO......
    0xff, 0xc0,  // OOOOOOOOOO......
    0x00, 0x00,  // ----------......
    0xef, 0xc0,  // OOO-OOOOOO......
    0xef, 0xc0,  // OOO-OOOOOO......
    0x00, 0x00,  // ----------......
    0xff, 0xc0,  // OOOOOOOOOO......
    0xef, 0xc0,  // OOO-OOOOOO......
    0x00, 0x00,  // ----------......

    // ASCII: 57345, char width: 10
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x01, 0x80,  // -------OO-......
    0x03, 0x80,  // ------OOO-......
    0x67, 0x00,  // -OO--OOO--......
    0x7e, 0x00,  // -OOOOOO---......
    0x3c, 0x00,  // --OOOO----......
    0x18, 0x00,  // ---OO-----......
    0x00, 0x00,  // ----------......

    // ASCII: 57346, char width: 8
    0x00,  // --------
    0x00,  // --------
    0x00,  // --------
    0xee,  // OOO-OOO-
    0x7c,  // -OOOOO--
    0x3c,  // --OOOO--
    0x7c,  // -OOOOO--
    0xfe,  // OOOOOOO-
    0x44,  // -O---O--
    0x00,  // --------

    // ASCII: 57347, char width: 9
    0x08, 0x00,  // ----O----.......
    0x08, 0x00,  // ----O----.......
    0x6a, 0x00,  // -OO-O-O--.......
    0x6b, 0x00,  // -OO-O-OO-.......
    0xc9, 0x00,  // OO--O--O-.......
    0x81, 0x80,  // O------OO.......
    0xc1, 0x80,  // OO-----OO.......
    0xc3, 0x00,  // OO----OO-.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x3c, 0x00,  // --OOOO---.......

    // ASCII: 57348, char width: 9
    0x00, 0x00,  // ---------.......
    0x08, 0x00,  // ----O----.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x67, 0x00,  // -OO--OOO-.......
    0xe3, 0x80,  // OOO---OOO.......
    0x67, 0x00,  // -OO--OOO-.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x08, 0x00,  // ----O----.......

    // ASCII: 57349, char width: 9
    0x00, 0x00,  // ---------.......
    0x00, 0x00,  // ---------.......
    0x0f, 0x00,  // ----OOOO-.......
    0x3f, 0x00,  // --OOOOOO-.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0xff, 0x80,  // OOOOOOOOO.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x77, 0x00,  // -OOO-OOO-.......
    0x37, 0x00,  // --OO-OOO-.......
    0x00, 0x00,  // ---------.......

    // ASCII: 57350, char width: 9
    0x00, 0x00,  // ---------.......
    0x18, 0x00,  // ---OO----.......
    0x7f, 0x00,  // -OOOOOOO-.......
    0x47, 0x00,  // -O---OOO-.......
    0xc7, 0x00,  // OO---OOO-.......
    0x00, 0x00,  // ---------.......
    0xe1, 0x00,  // OOO----O-.......
    0xe3, 0x00,  // OOO---OO-.......
    0xfe, 0x00,  // OOOOOOO--.......
    0x3c, 0x00,  // --OOOO---.......

    // ASCII: 57351, char width: 7
    0x00,  // -------.
    0x04,  // -----O-.
    0x0c,  // ----OO-.
    0x1c,  // ---OOO-.
    0x38,  // --OOO--.
    0x70,  // -OOO---.
    0x38,  // --OOO--.
    0x1c,  // ---OOO-.
    0x0c,  // ----OO-.
    0x04,  // -----O-.

    // ASCII: 57352, char width: 7
    0x00,  // -------.
    0x40,  // -O-----.
    0x60,  // -OO----.
    0x30,  // --OO---.
    0x18,  // ---OO--.
    0x1c,  // ---OOO-.
    0x18,  // ---OO--.
    0x30,  // --OO---.
    0x60,  // -OO----.
    0x40,  // -O-----.

    // ASCII: 57353, char width: 8
    0x00,  // --------
    0x18,  // ---OO---
    0x18,  // ---OO---
    0x18,  // ---OO---
    0xff,  // OOOOOOOO
    0xff,  // OOOOOOOO
    0x18,  // ---OO---
    0x18,  // ---OO---
    0x18,  // ---OO---
    0x00,  // --------

    // ASCII: 57354, char width: 8
    0x00,  // --------
    0x00,  // --------
    0x00,  // --------
    0x00,  // --------
    0xff,  // OOOOOOOO
    0xff,  // OOOOOOOO
    0x00,  // --------
    0x00,  // --------
    0x00,  // --------
    0x00,  // --------

    // ASCII: 57355, char width: 10
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x08, 0x00,  // ----O-----......
    0x1c, 0x00,  // ---OOO----......
    0x3e, 0x00,  // --OOOOO---......
    0x77, 0x00,  // -OOO-OOO--......
    0xe3, 0x80,  // OOO---OOO-......
    0x41, 0x00,  // -O-----O--......
    0x00, 0x00,  // ----------......

    // ASCII: 57356, char width: 10
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0xe3, 0x80,  // OOO---OOO-......
    0x77, 0x00,  // -OOO-OOO--......
    0x3e, 0x00,  // --OOOOO---......
    0x1c, 0x00,  // ---OOO----......
    0x08, 0x00,  // ----O-----......
    0x00, 0x00,  // ----------......

    // ASCII: 57357, char width: 11
    0x00, 0x00,  // -----------.....
    0x00, 0x00,  // -----------.....
    0xff, 0xc0,  // OOOOOOOOOO-.....
    0x80, 0x20,  // O---------O.....
    0x80, 0x20,  // O---------O.....
    0xa0, 0xa0,  // O-O-----O-O.....
    0x80, 0x20,  // O---------O.....
    0x8e, 0x20,  // O---OOO---O.....
    0xff, 0xe0,  // OOOOOOOOOOO.....
    0x00, 0x00,  // -----------.....
};

/*Store the start index of the glyphs in the bitmap array*/
static const uint32_t symbol_10_basic_map[] = 
{
     0, 20, 40, 50, 70, 90, 110, 130, 
    140, 150, 160, 170, 190, 210, 
};

/*Store the width (column count) of each glyph*/
static const uint8_t symbol_10_basic_width[] = 
{
    10, 10,  8,  9,  9,  9,  9,  7, 
     7,  8,  8, 10, 10, 11, 
};

lv_font_t lv_font_symbol_10_basic = 
{
#if LV_TXT_UTF8 == 0
    192,        /*First letter's unicode */
    207,        /*Last letter's unicode */
#else
    57344,        /*First letter's unicode */
    57358,        /*Last letter's unicode */
#endif
    10,        /*Letters height (rows) */
    symbol_10_basic_bitmap,    /*Glyph's bitmap*/
    symbol_10_basic_map,    /*Glyph start indexes in the bitmap*/
    symbol_10_basic_width,    /*Glyph widths (columns)*/
    NULL         /*No next page by default*/
};

#endif /*USE_LV_FONT_SYMBOL_10_BASIC*/
