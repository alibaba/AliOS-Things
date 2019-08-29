#include "lvgl.h"
#include "aos/kernel.h"

#if USE_FREETYPE != 0

#include "ft2build.h"
#include "freetype.h"

#define CACHE_FONT_MUN_MAX 20

typedef struct font_data_s {
    uint32_t unicode;
    uint16_t height;
    uint16_t width;
    const uint8_t *bitmap;
    struct font_data_s *next;
} font_data_t;

typedef struct font_data_list_s {
    font_data_t *head;
    font_data_t *tail;
    uint32_t count;
} font_data_list_t;

font_data_list_t font_data_list = { NULL, NULL, 0 };

static FT_Library library;
static FT_Face face;

static uint32_t     unicode_letter_pre = 0;
static font_data_t *font_data_read     = NULL;
static aos_mutex_t  mutex_ft;

const uint8_t * get_freetype_bitmap(const lv_font_t * font, uint32_t unicode_letter);
int16_t get_freetype_width(const lv_font_t * font, uint32_t unicode_letter);

int freetype_init(void)
{
    int ret = -1;

    ret = FT_Init_FreeType(&library);
    if (ret != 0) {
        return -1;
    }

    ret = FT_New_Face(library, "/sdcard/msyhs.ttf", 0, &face);
    if (ret != 0) {
        return -1;
    }

    ret = aos_mutex_new(&mutex_ft);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

font_data_t *load_font_from_cache(lv_font_t * font, uint32_t unicode_letter)
{
    font_data_t *font_data_c = font_data_list.head;

    while (font_data_c != NULL) {
        if ((font_data_c->unicode == unicode_letter)
           && (font_data_c->height == font->line_height)) {
            return font_data_c;
        }

        font_data_c = font_data_c->next;
    }

    return NULL;
}

void add_font_to_cache(font_data_t *font_data)
{
    font_data_t *font_data_f = NULL;
    font_data_t *font_data_c = NULL;

    if (font_data_list.head == NULL) {
        font_data_list.head = font_data;
    } else {
        font_data_c = font_data_list.head;
        font_data_list.head = font_data;
        font_data_list.head->next = font_data_c;
    }

    font_data_list.count++;

    if(font_data_list.count > CACHE_FONT_MUN_MAX) {
        font_data_c = font_data_list.head;
        while(font_data_c->next->next != NULL) {
            font_data_c = font_data_c->next;
        }

        free(font_data_c->next->bitmap);
        free(font_data_c->next);

        font_data_c->next = NULL;

        font_data_list.count--;
    }
}

int freetype_load_letter(lv_font_t *font, uint32_t unicode_letter)
{
    int ret   = -1;
    int pos   = 0;
    int spare = 0;

    font_data_t *font_data_c = NULL;
    font_data_t *font_data_m = NULL;

    /* load letter form cache */
    font_data_c = load_font_from_cache(font, unicode_letter);
    if (font_data_c != NULL) {
        font_data_read = font_data_c;

        return 0;
    }

    /* if can not find the letter in cache, load it by freetype */
    ret = FT_Set_Pixel_Sizes(face, 0, font->line_height);
    if (ret != 0) {
        return -1;
    }

    ret = FT_Load_Char(face, unicode_letter, FT_LOAD_RENDER);
    if (ret != 0) {
        return -1;
    }


    font_data_m = malloc(sizeof(font_data_t));
    if (font_data_m == NULL) {
        return -1;
    }

    font_data_m->bitmap = malloc(font->line_height * face->glyph->bitmap.width);
    if (font_data_m->bitmap == NULL) {
        free(font_data_m);
        return -1;
    }

    font_data_m->unicode = unicode_letter;
    font_data_m->height = font->line_height;
    font_data_m->width = face->glyph->bitmap.width;
    font_data_m->next = NULL;

    pos = face->glyph->bitmap.width * ((font->line_height - face->glyph->bitmap.rows) / 2);

    memset(font_data_m->bitmap, 0, pos);
    memcpy(&font_data_m->bitmap[pos], face->glyph->bitmap.buffer, 
        (face->glyph->bitmap.width * face->glyph->bitmap.rows));
    memset(&font_data_m->bitmap[pos + face->glyph->bitmap.width * face->glyph->bitmap.rows], 0,
        face->glyph->bitmap.width * (font->line_height - face->glyph->bitmap.rows) - pos);

    add_font_to_cache(font_data_m);

    font_data_read = font_data_m;

    return 0;
}

/* Get info about glyph of `unicode_letter` in `font` font. 
 * Store the result in `dsc_out`.
 * The next letter (`unicode_letter_next`) might be used to calculate the width required by this glyph (kerning)
 */
bool get_freetype_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    if (font == NULL) {
        return NULL;
    }

    aos_mutex_lock(&mutex_ft, AOS_WAIT_FOREVER);

    freetype_load_letter(font, unicode_letter);

    aos_mutex_unlock(&mutex_ft);

    /* Store the result.
     * For example ... 
     */
    dsc_out->adv_w = font->line_height;        /*Horizontal space required by the glyph in [px]*/
    dsc_out->box_h = font->line_height;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = font_data_read->width;         /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = 0;         /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = 0;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp   = 8;         /*Bit per pixel: 1/2/4/8*/
    
    return true;                /*true: glyph found; false: glyph was not found*/
}

/* Get the bitmap of `unicode_letter` from `font`. */
const uint8_t * get_freetype_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    if (font == NULL) {
        return NULL;
    }

    aos_mutex_lock(&mutex_ft, AOS_WAIT_FOREVER);

    freetype_load_letter(font, unicode_letter);

    aos_mutex_unlock(&mutex_ft);

    return font_data_read->bitmap;
}

#endif
