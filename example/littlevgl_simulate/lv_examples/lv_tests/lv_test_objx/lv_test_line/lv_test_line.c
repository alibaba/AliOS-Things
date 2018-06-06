/**
 * @file lv_test_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_line.h"

#if USE_LV_LINE && USE_LV_TESTS

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
 * Create lines to test their functionalities
 */
void lv_test_line_1(void)
{
    static const lv_point_t p[] = {{5, 5}, {60, 30}, {80, 10}};

    /* Create a default object*/
    lv_obj_t *line1 = lv_line_create(lv_scr_act(), NULL);
    lv_obj_set_pos(line1, 10, 10);
    lv_line_set_points(line1, p, 3);

    /*Test y invert*/
    lv_obj_t *line2 = lv_line_create(lv_scr_act(), line1);
    lv_line_set_y_invert(line2, true);
    lv_obj_align(line2, line1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Test styling*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);

    style.line.color = LV_COLOR_RED;
    style.line.width = 3;

    lv_obj_t *line3 = lv_line_create(lv_scr_act(), line2);
    lv_line_set_style(line3, &style);
    lv_line_set_y_invert(line3, true);
    lv_obj_align(line3, line2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_LINE && USE_LV_TESTS*/
