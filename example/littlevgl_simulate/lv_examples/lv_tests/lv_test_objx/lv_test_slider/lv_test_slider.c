/**
 * @file lv_test_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_slider.h"

#if USE_LV_SLIDER != 0

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
 * Create sliders to test their functionalities
 */
void lv_test_slider_1(void)
{
    /* Create a default object*/
    lv_obj_t * slider1 = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_pos(slider1, 10, 10);

    /* Modify size and position, range and set to 75 % */
    lv_obj_t * slider2 = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_size(slider2, 150, 50);
    lv_obj_align(slider2, slider1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_slider_set_range(slider2, -50, 50);
    lv_slider_set_value(slider2, 25);

    /* Copy 'slider2' but set its size to be vertical (indicator at 75%)*/
    lv_obj_t * slider3 = lv_slider_create(lv_scr_act(), slider2);
    lv_obj_set_size(slider3, 50, 150);
    lv_obj_align(slider3, slider2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /* Copy 'slider2' and set new style for it
     * (like 'slider2' on its left but dark bg, thin red indicator with big light)*/
    static lv_style_t slider_bg;
    lv_style_copy(&slider_bg, &lv_style_pretty);
    slider_bg.body.main_color = LV_COLOR_BLACK;

    static lv_style_t slider_indic;
    lv_style_copy(&slider_indic, &lv_style_pretty);
    slider_indic.body.main_color = LV_COLOR_RED;
    slider_indic.body.grad_color = LV_COLOR_MARRON;
    slider_indic.body.shadow.color = LV_COLOR_RED;
    slider_indic.body.shadow.width = 20;
    slider_indic.body.padding.ver = 0;
    slider_indic.body.padding.hor = 0;

    static lv_style_t slider_knob;
    lv_style_copy(&slider_knob, &lv_style_pretty);
    slider_knob.body.radius = LV_RADIUS_CIRCLE;
    slider_knob.body.border.color = LV_COLOR_BLUE;
    slider_knob.body.empty = 1;

    lv_obj_t * slider4 = lv_slider_create(lv_scr_act(), slider2);
    lv_obj_align(slider4, slider2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_slider_set_style(slider4, LV_SLIDER_STYLE_BG, &slider_bg);
    lv_slider_set_style(slider4, LV_SLIDER_STYLE_INDIC, &slider_indic);
    lv_slider_set_style(slider4, LV_SLIDER_STYLE_KNOB, &slider_knob);

    /* Copy 'slider4' but set its size to be vertical*/
    lv_obj_t * slider5 = lv_slider_create(lv_scr_act(), slider4);
    lv_obj_set_size(slider5, 50, 150);
    lv_obj_align(slider5, slider4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_SLIDER && USE_LV_TESTS*/
