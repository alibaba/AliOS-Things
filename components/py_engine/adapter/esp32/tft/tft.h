/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
 * High level TFT functions
 * Author:  LoBo 02/2018, https://github/loboris
 * 
 */

#ifndef _TFT_H_
#define _TFT_H_

#include "sdkconfig.h"

#include <stdlib.h>
#include "tftspi.h"
#include "py/obj.h"

#define TFT_MODE_TFT    0
#define TFT_MODE_EPD    1
#define TFT_MODE_EVE    2
#define TFT_MODE_SPRITE     3

typedef struct {
    uint16_t        x1;
    uint16_t        y1;
    uint16_t        x2;
    uint16_t        y2;
} dispWin_t;

typedef struct {
    uint8_t     *font;
    uint8_t     x_size;
    uint8_t     y_size;
    uint8_t        offset;
    uint16_t    numchars;
    uint16_t    size;
    uint8_t     max_x_size;
    uint8_t     bitmap;
    color_t     color;
} Font;

typedef struct _tft_eve_obj_t {
    mp_obj_base_t base;
    uint32_t    addr;
    uint32_t    size;
    uint16_t    width;
    uint16_t    height;
    uint16_t    rowsize;
    uint8_t     type;
    uint8_t     byte_per_pixel;
    uint8_t     prev_tft_mode;
    uint8_t     loaded;
} tft_eve_obj_t;


//==========================================================================================
// ==== Global variables ===================================================================
//==========================================================================================

extern uint8_t   orientation;        // current screen orientation
extern uint16_t  font_rotate;       // current font font_rotate angle (0~395)
extern uint8_t   font_transparent;    // if not 0 draw fonts transparent
extern uint8_t   font_forceFixed;   // if not zero force drawing proportional fonts with fixed width
extern uint8_t   font_buffered_char;
extern uint8_t   font_line_space;    // additional spacing between text lines; added to font height
extern uint8_t   text_wrap;         // if not 0 wrap long text to the new line, else clip
extern color_t   _fg;                // current foreground color for fonts
extern color_t   _bg;                // current background for non transparent fonts
extern uint8_t   font_now;
extern dispWin_t dispWin;            // display clip window
extern float      _angleOffset;        // angle offset for arc, polygon and line by angle functions
extern uint8_t      image_debug;        // print debug messages during image decode if set to 1

extern Font cfont;                    // Current font structure

extern int    TFT_X;                    // X position of the next character after TFT_print() function
extern int    TFT_Y;                    // Y position of the next character after TFT_print() function
extern uint16_t image_width;
extern uint16_t image_hight;

extern uint32_t tp_calx;            // touch screen X calibration constant
extern uint32_t tp_caly;            // touch screen Y calibration constant

extern uint8_t tft_active_mode;     // used tft driver mode (TFT, EPD or EVE)
// =========================================================================================

// Buffer is created during jpeg decode for sending data
// Total size of the buffer is  2 * (JPG_IMAGE_LINE_BUF_SIZE * 3)
// The size must be multiple of 256 bytes !!
#define JPG_IMAGE_LINE_BUF_SIZE 512

// --- Constants for ellipse function ---
#define TFT_ELLIPSE_UPPER_RIGHT 0x01
#define TFT_ELLIPSE_UPPER_LEFT  0x02
#define TFT_ELLIPSE_LOWER_LEFT  0x04
#define TFT_ELLIPSE_LOWER_RIGHT 0x08

// Constants for Arc function
// number representing the maximum angle (e.g. if 100, then if you pass in start=0 and end=50, you get a half circle)
// this can be changed with setArcParams function at runtime
#define DEFAULT_ARC_ANGLE_MAX 360
// rotational offset in degrees defining position of value 0 (-90 will put it at the top of circle)
// this can be changed with setAngleOffset function at runtime
#define DEFAULT_ANGLE_OFFSET -90

#define PI 3.14159265359

#define MIN_POLIGON_SIDES    3
#define MAX_POLIGON_SIDES    60

// === Color names constants ===
extern const color_t TFT_BLACK;
extern const color_t TFT_NAVY;
extern const color_t TFT_DARKGREEN;
extern const color_t TFT_DARKCYAN;
extern const color_t TFT_MAROON;
extern const color_t TFT_PURPLE;
extern const color_t TFT_OLIVE;
extern const color_t TFT_LIGHTGREY;
extern const color_t TFT_DARKGREY;
extern const color_t TFT_BLUE;
extern const color_t TFT_GREEN;
extern const color_t TFT_CYAN;
extern const color_t TFT_RED;
extern const color_t TFT_MAGENTA;
extern const color_t TFT_YELLOW;
extern const color_t TFT_WHITE;
extern const color_t TFT_ORANGE;
extern const color_t TFT_GREENYELLOW;
extern const color_t TFT_PINK;

#define iTFT_BLACK       0
#define iTFT_NAVY        128
#define iTFT_DARKGREEN   32768
#define iTFT_DARKCYAN    32896
#define iTFT_MAROON      8388608
#define iTFT_PURPLE      8388736
#define iTFT_OLIVE       8421376
#define iTFT_LIGHTGREY   12632256
#define iTFT_DARKGREY    8421504
#define iTFT_BLUE        255
#define iTFT_GREEN       65280
#define iTFT_CYAN        65535
#define iTFT_RED         16515072
#define iTFT_MAGENTA     16515327
#define iTFT_YELLOW      16579584
#define iTFT_WHITE       16579836
#define iTFT_ORANGE      16557056
#define iTFT_GREENYELLOW 11336748
#define iTFT_PINK        16564426

// === Color invert constants ===
#define INVERT_ON        1
#define INVERT_OFF        0

// === Special coordinates constants ===
#define CENTER    -9003
#define RIGHT    -9004
#define BOTTOM    -9004

#define LASTX    7000
#define LASTY    8000

// === Embedded fonts constants ===
#define DEFAULT_FONT    0
#define DEJAVU18_FONT    1
#define DEJAVU24_FONT    2
#define UBUNTU16_FONT    3
#define COMIC24_FONT    4
#define MINYA24_FONT    5
#define TOONEY32_FONT    6
#define SMALL_FONT        7
#define DEF_SMALL_FONT    8
#define FONT_7SEG        9
#define USER_FONT        10  // font will be read from file

#define DEJAVU40_FONT   11
#define DEJAVU56_FONT   12
#define DEJAVU72_FONT   13
#define ARIAL12_FONT    14
#define ARIAL16_FONT    15
// #define UNICODE_FONT    16


#define IMAGE_TYPE_JPG    1
#define IMAGE_TYPE_BMP    2
#define IMAGE_TYPE_PNG  3

// ===== PUBLIC FUNCTIONS =========================================================================


/*
 * Draw pixel at given x,y coordinates
 * 
 * Params:
 *       x: horizontal position
 *       y: vertical position
 *   color: pixel color
 *     sel: if not 0 activate CS before and deactivat after sending pixel data to display
 *          when sending multiple pixels it is faster to activate the CS first,
 *          send all pixels an deactivate CS after all pixela was sent
*/
//-------------------------------------------------------------------
void TFT_drawPixel(int16_t x, int16_t y, color_t color, uint8_t sel);

/*
 * Read pixel color value from display GRAM at given x,y coordinates
 * 
 * Params:
 *       x: horizontal position
 *       y: vertical position
 * 
 * Returns:
 *      pixel color at x,y
*/
//------------------------------------------
color_t TFT_readPixel(int16_t x, int16_t y);


/*
 * Draw vertical line at given x,y coordinates
 * 
 * Params:
 *       x: horizontal start position
 *       y: vertical start position
 *       h: line height in pixels
 *   color: line color
*/
//---------------------------------------------------------------------
void TFT_drawFastVLine(int16_t x, int16_t y, int16_t h, color_t color);

/*
 * Draw horizontal line at given x,y coordinates
 * 
 * Params:
 *       x: horizontal start position
 *       y: vertical start position
 *       w: line width in pixels
 *   color: line color
*/
//---------------------------------------------------------------------
void TFT_drawFastHLine(int16_t x, int16_t y, int16_t w, color_t color);

/*
 * Draw line on screen
 * 
 * Params:
 *       x0: horizontal start position
 *       y0: vertical start position
 *       x1: horizontal end position
 *       y1: vertical end position
 *   color: line color
*/
//-------------------------------------------------------------------------------
void TFT_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color);


/*
 * Draw line on screen from (x,y) point at given angle
 * Line drawing angle starts at lower right quadrant of the screen and is offseted by
 * '_angleOffset' global variable (default: -90 degrees)
 *
 * Params:
 *       x: horizontal start position
 *       y: vertical start position
 *   start: start offset from (x,y)
 *     len: length of the line
 *   angle: line angle in degrees
 *   color: line color
*/
//-----------------------------------------------------------------------------------------------------------
void TFT_drawLineByAngle(uint16_t x, uint16_t y, uint16_t start, uint16_t len, uint16_t angle, color_t color);

/*
 * Fill given rectangular screen region with color
 * 
 * Params:
 *       x: horizontal rect start position
 *       y: vertical rect start position
 *       w: rectangle width
 *       h: rectangle height
 *   color: fill color
*/
//---------------------------------------------------------------------------
void TFT_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color);

/*
 * Draw rectangle on screen
 * 
 * Params:
 *       x: horizontal rect start position
 *       y: vertical rect start position
 *       w: rectangle width
 *       h: rectangle height
 *   color: rect line color
*/
//------------------------------------------------------------------------------
void TFT_drawRect(uint16_t x1,uint16_t y1,uint16_t w,uint16_t h, color_t color);

/*
 * Draw rectangle with rounded corners on screen
 * 
 * Params:
 *       x: horizontal rect start position
 *       y: vertical rect start position
 *       w: rectangle width
 *       h: rectangle height
 *       r: corner radius
 *   color: rectangle color
*/
//----------------------------------------------------------------------------------------------
void TFT_drawRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, color_t color);

/*
 * Fill given rectangular screen region with rounded corners with color
 * 
 * Params:
 *       x: horizontal rect start position
 *       y: vertical rect start position
 *       w: rectangle width
 *       h: rectangle height
 *       r: corner radius
 *   color: fill color
*/
//----------------------------------------------------------------------------------------------
void TFT_fillRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, color_t color);

/*
 * Fill the whole screen with color
 * 
 * Params:
 *   color: fill color
*/
//--------------------------------
void TFT_fillScreen(color_t color);

/*
 * Fill the current clip window with color
 *
 * Params:
 *   color: fill color
*/
//---------------------------------
void TFT_fillWindow(color_t color);

/*
 * Draw triangle on screen
 * 
 * Params:
 *       x0: first triangle point x position
 *       y0: first triangle point y position
 *       x0: second triangle point x position
 *       y0: second triangle point y position
 *       x0: third triangle point x position
 *       y0: third triangle point y position
 *   color: triangle color
*/
//-----------------------------------------------------------------------------------------------------------------
void TFT_drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_t color);

/*
 * Fill triangular screen region with color
 * 
 * Params:
 *       x0: first triangle point x position
 *       y0: first triangle point y position
 *       x0: second triangle point x position
 *       y0: second triangle point y position
 *       x0: third triangle point x position
 *       y0: third triangle point y position
 *   color: fill color
*/
//-----------------------------------------------------------------------------------------------------------------
void TFT_fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_t color);

/*
 * Draw circle on screen
 * 
 * Params:
 *       x: circle center x position
 *       y: circle center x position
 *       r: circle radius
 *   color: circle color
*/
//-------------------------------------------------------------------
void TFT_drawCircle(int16_t x, int16_t y, int radius, color_t color);

/*
 * Fill circle on screen with color
 * 
 * Params:
 *       x: circle center x position
 *       y: circle center x position
 *       r: circle radius
 *   color: circle fill color
*/
//-------------------------------------------------------------------
void TFT_fillCircle(int16_t x, int16_t y, int radius, color_t color);


//-------------------------------------------------------------------
void TFT_drawSwitchBtn(int16_t x, int16_t y, int width, int hight, color_t color, color_t switch_color, uint8_t state);

/*
 * Draw ellipse on screen
 * 
 * Params:
 *       x0: ellipse center x position
 *       y0: ellipse center x position
 *       rx: ellipse horizontal radius
 *       ry: ellipse vertical radius
 *   option: drawing options, multiple options can be combined
                1 (TFT_ELLIPSE_UPPER_RIGHT) draw upper right corner
                2 (TFT_ELLIPSE_UPPER_LEFT)  draw upper left corner
                4 (TFT_ELLIPSE_LOWER_LEFT)  draw lower left corner
                8 (TFT_ELLIPSE_LOWER_RIGHT) draw lower right corner
             to draw the whole ellipse use option value 15 (1 | 2 | 4 | 8)
 * 
 *   color: circle color
*/
//------------------------------------------------------------------------------------------------------
void TFT_drawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, color_t color, uint8_t option);

/*
 * Fill elliptical region on screen
 * 
 * Params:
 *       x0: ellipse center x position
 *       y0: ellipse center x position
 *       rx: ellipse horizontal radius
 *       ry: ellipse vertical radius
 *   option: drawing options, multiple options can be combined
                1 (TFT_ELLIPSE_UPPER_RIGHT) fill upper right corner
                2 (TFT_ELLIPSE_UPPER_LEFT)  fill upper left corner
                4 (TFT_ELLIPSE_LOWER_LEFT)  fill lower left corner
                8 (TFT_ELLIPSE_LOWER_RIGHT) fill lower right corner
             to fill the whole ellipse use option value 15 (1 | 2 | 4 | 8)
 * 
 *   color: fill color
*/
//------------------------------------------------------------------------------------------------------
void TFT_fillEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, color_t color, uint8_t option);


/*
 * Draw circle arc on screen
 * Arc drawing angle starts at lower right quadrant of the screen and is offseted by
 * '_angleOffset' global variable (default: -90 degrees)
 *
 * Params:
 *        cx: arc center X position
 *        cy: arc center Y position
 *        th: thickness of the drawn arc
 *        ry: arc vertical radius
 *     start: arc start angle in degrees
 *       end: arc end angle in degrees
 *     color: arc outline color
 * fillcolor: arc fill color
*/
//----------------------------------------------------------------------------------------------------------------------------
void TFT_drawArc(uint16_t cx, uint16_t cy, uint16_t r, uint16_t th, float start, float end, color_t color, color_t fillcolor);


/*
 * Draw polygon on screen
 *
 * Params:
 *        cx: polygon center X position
 *        cy: arc center Y position
 *     sides: number of polygon sides; MAX_POLIGON_SIDES ~ MAX_POLIGON_SIDES (3 ~ 60)
 *  diameter: diameter of the circle inside which the polygon is drawn
 *     color: polygon outline color
 *      fill: polygon fill color; if same as color, polygon is not filled
 *       deg: polygon rotation angle; 0 ~ 360
 *        th: thickness of the polygon outline
*/
//--------------------------------------------------------------------------------------------------------------
void TFT_drawPolygon(int cx, int cy, int sides, int diameter, color_t color, color_t fill, int deg, uint8_t th);


//--------------------------------------------------------------------------------------
//void TFT_drawStar(int cx, int cy, int diameter, color_t color, bool fill, float factor);


/*
 * Set the font used for writing the text to display.
 *
 * ------------------------------------------------------------------------------------
 * For 7 segment font only characters 0,1,2,3,4,5,6,7,8,9, . , - , : , / are available.
 *   Character ‘/‘ draws the degree sign.
 * ------------------------------------------------------------------------------------
 *
 * Params:
 *             font: font number; use defined font names
 *        font_file: pointer to font file name; NULL for embeded fonts
 */
//----------------------------------------------------
void TFT_setFont(uint8_t font, const char *font_file);

/*
 * Returns current font height & width in pixels.
 *
 * Params:
 *         width: pointer to returned font width
 *        height: pointer to returned font height
 */
//-------------------------------------------
int TFT_getfontsize(int *width, int* height);


/*
 * Returns current font height in pixels.
 *
 */
//----------------------
int TFT_getfontheight();

/*
 * Write text to display.
 *
 * Rotation of the displayed text depends on 'font_rotate' variable (0~360)
 * if 'font_transparent' variable is set to 1, no background pixels will be printed
 *
 * If the text does not fit the screen width it will be clipped (if text_wrap=0),
 * or continued on next line (if text_wrap=1)
 *
 * Two special characters are allowed in strings:
 *         ‘\r’ CR (0x0D), clears the display to EOL
 *         ‘\n’ LF (ox0A), continues to the new line, x=0
 *
 * Params:
 *       st:    pointer to null terminated string to be printed
 *        x:    horizontal position of the upper left point in pixels
 *                Special values can be entered:
 *                    CENTER, centers the text
 *                    RIGHT, right justifies the text
 *                    LASTX, continues from last X position; offset can be used: LASTX+n
 *        y: vertical position of the upper left point in pixels
 *                Special values can be entered:
 *                    CENTER, centers the text
 *                    BOTTOM, bottom justifies the text
 *                    LASTY, continues from last Y position; offset can be used: LASTY+n
 *
 */
//-------------------------------------
void TFT_print(char *st, int x, int y);
void TFT_print_unicode(uint16_t *unicode, uint16_t len, int x, int y);

/*
 * Set atributes for 7 segment vector font
 * == 7 segment font must be the current font to this function to have effect ==
 *
 * Params:
 *              l:    6~40; distance between bars in pixels
 *              w:    1~12, max l/2;  bar width in pixels
 *   outline:    draw font outline if set to 1
 *       color:    font outline color, only used if outline=1
 *
 */
//-------------------------------------------------------------------------
void set_7seg_font_atrib(uint8_t l, uint8_t w, int outline, color_t color);

/*
 * Sets the clipping area coordinates.
 * All writing to screen is clipped to that area.
 * Starting x & y in all functions will be adjusted to the clipping area.
 *
 * Params:
 *        x1,y1:    upper left point of the clipping area
 *        x2,y2:    bottom right point of the clipping area
 *
 */
//----------------------------------------------------------------------
void TFT_setclipwin(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/*
 * Resets the clipping area to full screen (0,0),(_wodth,_height)
 *
 */
//----------------------
void TFT_resetclipwin();

/*
 * Save current clipping area to temporary variable
 *
 */
//---------------------
void TFT_saveClipWin();

/*
 * Restore current clipping area from temporary variable
 *
 */
//------------------------
void TFT_restoreClipWin();

/*
 * Set the screen rotation
 * Also resets the clip window and clears the screen with current background color
 *
 * Params:
 *         rot:    0~3; screen rotation; use defined rotation constants:
 *                 PORTRAIT, LANDSCAPE, PORTRAIT_FLIP, LANDSCAPE_FLIP
 *
 */
//--------------------------------
void TFT_setRotation(uint8_t rot);

/*
 * Set inverted/normal colors
 *
 * Params:
 *         mode:    0 or 1; use defined constants: INVERT_ON or INVERT_OFF
 *
 */
//-----------------------------------------
void TFT_invertDisplay(const uint8_t mode);

/*
 * Select gamma curve
 * Params:
 *      gamma: gama curve, values 0~3
 */
//=================================
void TFT_setGammaCurve(uint8_t gm);

/*
 * Compare two color structures
 * Returns 0 if equal, 1 if not equal
 *
 * Params:
 *         c1, c2:    colors to be compared
 */
//---------------------------------------------
int TFT_compare_colors(color_t c1, color_t c2);

/*
 * returns the string width in pixels.
 * Useful for positions strings on the screen.
 */
//--------------------------------
int TFT_getStringWidth(char* str);


/*
 * Fills the rectangle occupied by string with current background color
 */
//------------------------------------------------
void TFT_clearStringRect(int x, int y, char *str);

/*
 * Converts the components of a color, as specified by the HSB model,
 * to an equivalent set of values for the default RGB model.

 * The color structure that is returned by HSBtoRGB encodes the value of a color as R, G & B component
 *
 * Params:
 *            _hue:    float;    any number, the floor of this number is subtracted from it to create a fraction between 0 and 1.
 *                             This fractional number is then multiplied by 360 to produce the hue angle in the HSB color model.
 *            _sat:    float; 0 ~ 1.0
 *    _brightness:    float; 0 ~ 1.0
 *
*/
//----------------------------------------------------------
color_t HSBtoRGB(float _hue, float _sat, float _brightness);

/*
 * Decodes and displays JPG image
 * Limits:
 *         Baseline only. Progressive and Lossless JPEG format are not supported.
 *        Image size: Up to 65520 x 65520 pixels
 *        Color space: YCbCr three components only. Gray scale image is not supported.
 *        Sampling factor: 4:4:4, 4:2:2 or 4:2:0.
 *
 * Params:
 *       x: image left position; constants CENTER & RIGHT can be used; negative value is accepted
 *       y: image top position;  constants CENTER & BOTTOM can be used; negative value is accepted
 *   scale: image scale factor: 0~3; if scale>0, image is scaled by factor 1/(2^scale) (1/2, 1/4 or 1/8)
 *   fname: pointer to the name of the file from which the image will be read
 *           if set to NULL, image will be read from memory buffer pointed to by 'buf'
 *     buf: pointer to the memory buffer from which the image will be read; used if fname=NULL
 *    size: size of the memory buffer from which the image will be read; used if fname=NULL & buf!=NULL
 *
 */
//-----------------------------------------------------------------------------------
void TFT_jpg_image(int x, int y, uint8_t scale, char *fname, uint8_t *buf, int size);

/*
 * Decodes and displays BMP image
 * Only uncompressed RGB 24-bit with no color space information BMP images can be displayed
 *
 * Params:
 *       x: image left position; constants CENTER & RIGHT can be used; negative value is accepted
 *       y: image top position;  constants CENTER & BOTTOM can be used; negative value is accepted
 *   scale: image scale factor: 0~7; if scale>0, image is scaled by factor 1/(scale+1)
 *   fname: pointer to the name of the file from which the image will be read
 *           if set to NULL, image will be read from memory buffer pointed to by 'imgbuf'
 *  imgbuf: pointer to the memory buffer from which the image will be read; used if fname=NULL
 *    size: size of the memory buffer from which the image will be read; used if fname=NULL & imgbuf!=NULL
 *
 */
//-------------------------------------------------------------------------------------
int TFT_bmp_image(int x, int y, uint8_t scale, char *fname, uint8_t *imgbuf, int size);

/*
 * Decodes and displays PNG image
 *
 * Params:
 *       x: image left position; constants CENTER & RIGHT can be used; negative value is accepted
 *       y: image top position;  constants CENTER & BOTTOM can be used; negative value is accepted
 *   scale: image scale factor: 0~7; if scale>0, image is scaled by factor 1/(scale+1)
 *   fname: pointer to the name of the file from which the image will be read
 *           if set to NULL, image will be read from memory buffer pointed to by 'imgbuf'
 *  imgbuf: pointer to the memory buffer from which the image will be read; used if fname=NULL
 *    size: size of the memory buffer from which the image will be read; used if fname=NULL & imgbuf!=NULL
 *
 */
//-------------------------------------------------------------------------------------
int TFT_png_image(int x, int y, uint8_t scale, char *fname, uint8_t *imgbuf, int size);

/*
 * Get the touch panel coordinates.
 * The coordinates are adjusted to screen orientation if raw=0
 *
 * Params:
 *         x: pointer to X coordinate
 *         y: pointer to Y coordinate
 *       raw: if 0 returns calibrated screen coordinates; if 1 returns raw touch controller coordinates
 *
 * Returns:
 *         0 if touch panel is not touched; x=y=0
 *         1 if touch panel is touched; x&y are the valid coordinates
 */
//----------------------------------------------
int TFT_read_touch(int *x, int* y, uint8_t raw);


/*
 * Compile font c source file to .fnt file
 * which can be used in TFT_setFont() function to select external font
 * Created file have the same name as source file and extension .fnt
 *
 * Params:
 *        fontfile: pointer to c source font file name; must have .c extension
 *             dbg: if set to 1, prints debug information
 *
 * Returns:
 *         0 on success
 *         err no on error
 *
 */
//------------------------------------------------
int compile_font_file(char *fontfile, uint8_t dbg);

/*
 * Get all font's characters to buffer
 */
void getFontCharacters(uint8_t *buf);

extern void TFT_pushBitMap(int x, int y, int w, int h, uint8_t* bitmap, color_t color, color_t bg_color);

void led_pwm_init();
void led_setBrightness(int duty);
// extern int16_t GetUnicodeWidth(uint32_t code);

#endif
