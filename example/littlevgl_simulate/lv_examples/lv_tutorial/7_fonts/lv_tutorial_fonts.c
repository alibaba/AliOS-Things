/**
 * @file lv_tutorial_fonts.h
 *
 */

/*
 * --------------------------------------------------------------
 * How to add new fonts (even Unicode) to the graphic library
 * --------------------------------------------------------------
 *
 * BUILT-IN FONTS
 * - There are several built in font in the library which can be enabled in lv_conf.h
 * - For example: font_dejavu_20, font_dejavu_40
 *
 * USING A FONT
 * - Just set a font in style.text.font
 * - For example: style.text.font = &font_dejavu_20
 *
 * SYMBOLS
 * - There are symbols as well which stored as fonts.
 * - To reference a symbol use the defines: SYMBOL_... (SYMBOL_FILE, SYMBOL_OK ... see lv_symbol_def.h)
 * - You can enable the symbols in lv_conf. similarly to built-in fonts.
 *
 * CHARACTER-SET EXTENSION
 * - You can add fonts to a base font to extend its character set.
 * - This way the glyph (image) of the character will be searched all added fonts
 * - Use font_add(&font_extension, &font_base);
 * - For example:
 *   - font_add(&font_arial_20, &font_arial_20_latin_ext_a);
 *   - style.text.font = &font_dejavu_20;
 *   - Now: "abc AÁóÓ" letter will be drawn as well
 * - The built-in fonts are concatenated automatically in the same height
 * - The symbols are concatenated to the normal fonts as well
 *
 * UTF8 SUPPORT
 * - To use unicode characters (not only the basic ASCII: 32..126 and symbols)
 *   you have to enable LV_TXT_UTF8 in lv_conf.h
 * - You also need fonts which contains the required Unicode letters
 *
 * BIT-PER-PIXEL
 * - The fonts can describe a pixel with 1, 2, 4 or 8 bit. The higher value results smoother letters
 * but larger foot memory foot print.
 * - The built in fonts can be enabled with 1,2,4 or 8 values the select their bpp.
 *
 * ADDING NEW FONTS
 *  - You can generate your own fonts using the online font converter tool:
 *    https://littlevgl.com/ttf-font-to-c-array
 *  - All information is provided on the website
 *
 */



/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_fonts.h"
#if USE_LV_TUTORIALS

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
LV_FONT_DECLARE(arial_ascii_20);        /*Full ASCII range*/
LV_FONT_DECLARE(arial_cyrillic_20);   /*Continuous block of cyrillic characters*/
LV_FONT_DECLARE(arial_math_20);        /*Sparse mathematical characters*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Concat two font and create label with Unicode characters
 * LV_TXT_UTF8 has to be enabled
 */
void lv_tutorial_fonts(void)
{
    /*Concatenate the fonts into one*/
    lv_font_add(&arial_cyrillic_20, &arial_ascii_20);
    lv_font_add(&arial_math_20, &arial_ascii_20);

    /*Create a style and use the new font*/
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.font = &arial_ascii_20; /*Set the base font whcih is concatenated with the others*/

    /*Create a label and set new text*/
    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label, 10, 10);
    lv_label_set_style(label, &style1);
    lv_label_set_text(label, "Hello\nпривет\n∞∑");      /*Use ASCII and Cyrillic letters together*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_TUTORIALS*/
