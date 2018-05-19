/**
 * @file lv_tutorial_antialiasing.h
 *
 */

/*
 * --------------------------------------------------------------
 * Learn how to make the drawings smoother with anti aliasing
 * --------------------------------------------------------------
 *
 * The graphics library support 2 types of anti aliasing:
 * 1. Full screen anti aliasing:
 *   - Everything is rendered in double size and the filtered (smoothed) to the real size
 *   - It results smooth image but need more computation resources and memory (VDB)
 *   - Only available in buffered mode (lv_conf.h   LV_VDB_SIZE != 0)
 *   - You can enable it in lv_conf.h: LV_ANTIALIAS    1
 *   - The fonts and images are down scaled to half during anti alaising.
 *     - to get a 20 px font you have to use 40 px ones
 *
 * 2. Font anti aliasing
 *   - It smoothes only the fonts.
 *   - Works in buffered and unbuffered mode as well, but makes much better result with buffering
 *   - Needs only a little extra computation resource
 *   - Only the fonts has to be doubled
 *   - You can enable it in lv_conf.h: LV_FONT_ANTIALIAS    1
 *   - Don't enable LV_ANTIALIAS and LV_FONT_ANTIALIAS at the same time because
 *     it will result quarter sized letters
 *
 * Try the example by hanging the anti aliasing modes in lv_conf.h
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_antialiasing.h"
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
LV_IMG_DECLARE(img_red_flower);


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create object to see how they change from the anti aliasing
 * Modify LV_ANTIALIAS and LV_FONT_ANTIALIAS to see what is changing
 */
void lv_tutorial_antialiasing(void)
{
    lv_obj_t *label;

    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_btn_rel);
    style1.body.radius = 20;
    style1.body.border.width = 8;

    lv_obj_t *btn1;
    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn1, 10, 10);
    lv_obj_set_size(btn1, 160, 80);
    lv_btn_set_style(btn1, LV_BTN_STYLE_REL, &style1);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");


    lv_img_create_file("red_flower", img_red_flower);       /*Create a file in the RAM FS*/

    /*Crate an image which is NOT automatically upscaled to compensate the anti aliasing*/
    lv_obj_t *img_normal = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_file(img_normal, "U:/red_flower");
    lv_img_set_upscale(img_normal, false);
    lv_obj_align(img_normal, btn1, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

    /*Crate an image which is automatically upscaled to compensate the anti aliasing*/
    lv_obj_t *img_scaled = lv_img_create(lv_scr_act(), img_normal);  /*Crate an image object*/
    lv_img_set_upscale(img_scaled, true);
    lv_obj_align(img_scaled, img_normal, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/
