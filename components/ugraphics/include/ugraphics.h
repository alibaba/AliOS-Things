#ifndef TETRIS_GRAPHICS
#define TETRIS_GRAPHICS

#include <stdint.h>
#ifdef AOS_COMP_SDL2
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#endif

#define RGB565

#ifdef RGB565
#define COLOR_BLACK  0x000000FF
#define COLOR_RED    0xFF4D40FF
#define COLOR_GREEN  0x008000FF
#define COLOR_BLUE   0x0047ABFF
#define COLOR_CYAN   0x00FFFFFF
#define COLOR_YELLOW 0xFFFF00FF
#define COLOR_PURPLE 0x9400D3FF
#define COLOR_ORANGE 0xFFA000FF
#define COLOR_WHITE  0xFFFFFFFF
#define COLOR_SHADOW 0x808080FF
#else
#define COLOR_BLACK  0x000000FF
#define COLOR_RED    0xFF0000FF
#define COLOR_GREEN  0x00FF00FF
#define COLOR_BLUE   0x0000FFFF
#define COLOR_CYAN   0x00FFFFFF
#define COLOR_YELLOW 0xFFFF00FF
#define COLOR_PURPLE 0xA000FFFF
#define COLOR_ORANGE 0xFFA000FF
#define COLOR_WHITE  0xFFFFFFFF
#define COLOR_SHADOW 0x606060FF
#endif

#define SCREEN_W 320
#define SCREEN_H 240

#ifdef AOS_COMP_SDL2
#define UGRAPHICS_FONT_STYLE TTF_STYLE_BOLD
#else
#define UGRAPHICS_FONT_STYLE 0
#endif

/** @defgroup ugraphics_aos_api
 * @{
 */

/**
 * Init the ugraphics module.
 *
 * @param[in]  width   width of screen.
 * @param[in]  height  height of screen.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_init(int32_t width, int32_t height);

/**
 * load ttf font file.
 *
 * @param[in]  filename  ttf font file name.
 * @param[in]  size  font size.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_load_font(const char *filename, int32_t size);

/**
 * load ttf font file.
 *
 * @param[in]  style  ttf font style.
 *                    Refer to SDL_ttf.h: TTF_STYLE_NORMAL/TTF_STYLE_BOLD...
 *
 */
void ugraphics_set_font_style(int32_t style);

/**
 * Release graphics all resources.
 *
 */
void ugraphics_quit(void);

/**
 * Flip graphics to lcd.
 *
 */
void ugraphics_flip(void);

/**
 * Clear graphics on window.
 *
 */
int32_t ugraphics_clear(void);

/**
 * Set graphics default color.
 *
 * @param[in]  color  grapchic color.
 *
* @return  0 on success, negative error on failure.
 */
int32_t ugraphics_set_color(uint32_t color);

/**
 * Draw empty rectangle to the window.
 *
 * @param[in]  x  grapchic x location on window.
 * @param[in]  y  grapchic y location on window.
 * @param[in]  w  grapchic width on window.
 * @param[in]  h  grapchic height on window.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_draw_rect(int32_t x, int32_t y, int32_t w, int32_t h);

/**
 * Fill full rectangle to the window.
 *
 * @param[in]  x  grapchic x location on window.
 * @param[in]  y  grapchic y location on window.
 * @param[in]  w  grapchic width on window.
 * @param[in]  h  grapchic height on window.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_fill_rect(int32_t x, int32_t y, int32_t w, int32_t h);

/**
 * Draw line on window.
 *
 * @param[in]  x1  line x1 start position.
 * @param[in]  y1  line y1 start position.
 * @param[in]  x2  line x2 end position.
 * @param[in]  y2  line y2 end position.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * Draw string on window.
 *
 * @param[in]  string  string you want to draw.
 * @param[in]  x       draw string at x postion of window.
 * @param[in]  y       draw string at y position of window.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_draw_string(char *string, int32_t x, int32_t y);

/**
 * Count string width.
 *
 * @param[in]  string  string you want to draw.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_string_width(char *string);

/**
 * Save image graphic buffer to a image file.
 * @param[in]  buffer   image buffer.
 * @param[in]  len      image buffer length.
 * @param[in]  path     image file path in file system.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_save_image(char *buffer, int32_t len, const char *path);

/**
 * Release graphics all resources.
 *
 * @param[in]  file  image file location.
 * @param[in]  x     x position on window.
 * @param[in]  y     y position on window.
 *
 * @return  0 on success, negative error on failure.
 */
int32_t ugraphics_draw_image(const char *file, int32_t x, int32_t y);

/**
 * @}
 */
#endif // TETRIS_GRAPHICS
