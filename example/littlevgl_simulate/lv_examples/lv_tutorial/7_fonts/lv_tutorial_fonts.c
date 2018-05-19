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
 * - You can add a font to a base font to extend its character set.
 * - This way when you set font in style and use a character in text,
 *   the glyph (image) of the character will be searched in both fonts
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
 * ADDING NEW FONTS
 *  - To add a new font you need to tools. Both of them are there in lv_utilsrepository:
 *    https://github.com/littlevgl/lv_utils
 *
 *  - BMFONT
 *     0. Install BMFont. It's a windows program but works fine on Linux using Wine
 *     1. Option -> Font settings
 *       - Choose a font
 *       - Set size in 'size (px)'
 *     2. Option ->Export options
 *       - Set texture width and height to 2048
 *       - File format: XML
 *       - Textures: png
 *     3. Select the characters you want to include
 *     4. Options -> Save bitmap font as...
 *       - Give a name to the font 'my_font1' and save it
 *       - It will create: 'my_font1.fnt' and 'my_font1.png' *
 * - FNT2C
 *    1. Open a terminal
 *    2. Copy font_create/fnt2c.py next to your 'fnt' and 'png' files
 *    3. Open a terminal and go to that folder
 *    4. Type  'python fnt2c.py -f <font name> -s <start unicode> -e <end unicode> -o <output file name>'
 *       - It will create a 'c' and a  a 'h' file
 *       - You can create more files using different unicode ranges and output name
 *
 *
 * USING THE FONT
 * 1. Initilaize the font
 *   - Copy the 'c' and 'h' files into your project
 *   - Include a the 'h' files in source file where you want to use them
 *   - Use the font like: style1->text->font = &font_new1;
 *   - You attache a a new character set to a font: lv_font_add(&font_extension, &font_base)
 *
 * 2. You can use the fonts in style like this: style1.text.font = &my_font1;
 *
 */



/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_fonts.h"
#include "lvgl/lvgl.h"
#include "ubuntu_40_ascii.h"
#include "ubuntu_40_cyrillic.h"

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
    /*Add the cyrillic character set to the ASCII*/
    lv_font_add(&font_ubuntu_40_cyrillic, &font_ubuntu_40_ascii);

    /*Create a style and use the new font*/
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.font = &font_ubuntu_40_ascii;

    /*Create a label and set new text*/
    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label, 10, 10);
    lv_label_set_style(label, &style1);
    lv_label_set_text(label, "Hello\nпривет");      /*Use ASCII and Cyrillic letters together*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
