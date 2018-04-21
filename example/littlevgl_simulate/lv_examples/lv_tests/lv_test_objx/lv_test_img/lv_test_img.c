/**
 * @file lv_test_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_img.h"

#if USE_LV_IMG != 0

/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(img_flower_icon);

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
 * Create images to test their functionalities
 */
void lv_test_img_1(void)
{

    /*Create an image file in the RAM file system*/
    lv_img_create_file("pic1", img_flower_icon);

    /*Create an image object with the newly created file*/
    lv_obj_t *img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_file(img1, "U:/pic1");
    lv_obj_set_pos(img1, 10, 10);

    /*Copy the previous image but set upscale*/
    lv_obj_t *img2 = lv_img_create(lv_scr_act(), img1);
    lv_img_set_upscale(img2, true);
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Copy the previous image and set a redish style*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.image.color = LV_COLOR_RED;
    style.image.intense = LV_OPA_70;

    lv_obj_t *img3 = lv_img_create(lv_scr_act(), img2);
    lv_img_set_style(img3, &style);
    lv_obj_align(img3, img2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Copy the previous image and test the mozic feature*/
    lv_obj_t *img4 = lv_img_create(lv_scr_act(), img3);
    lv_obj_set_size(img4, 100, 100);
    lv_obj_align(img4, img3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Test symbol drawing*/
    lv_obj_t *img5 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_file(img5, SYMBOL_SETTINGS SYMBOL_OK);
    lv_obj_align(img5, img4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_IMG*/
