/**
 * @file lv_test_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_bar.h"
#if USE_LV_BAR && USE_LV_TESTS

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
 * Create bars to test their functionalities
 */
void lv_test_bar_1(void)
{
    /* Create a default object*/
    lv_obj_t * bar1 = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_pos(bar1, 10, 10);
    lv_bar_set_value(bar1, 40);

    /* Modify size and position, range and set to 75 % */
    lv_obj_t * bar2 = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(bar2, 200, 50);
    lv_obj_align(bar2, bar1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_bar_set_range(bar2, -50, 50);
    lv_bar_set_value(bar2, 25);

    /* Copy 'bar2' but set its size to be vertical (indicator at 75%)*/
    lv_obj_t * bar3 = lv_bar_create(lv_scr_act(), bar2);
    lv_obj_set_size(bar3, 50, 200);
    lv_obj_align(bar3, bar2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /* Copy 'bar2' and set new style for it
     * (like 'bar2' on its left but dark bg, thin red indicator with big light)*/
    static lv_style_t bar_bg;
    lv_style_copy(&bar_bg, &lv_style_pretty);
    bar_bg.body.main_color = LV_COLOR_BLACK;

    static lv_style_t bar_indic;
    lv_style_copy(&bar_indic, &lv_style_pretty);
    bar_indic.body.main_color = LV_COLOR_RED;
    bar_indic.body.grad_color = LV_COLOR_MARRON;
    bar_indic.body.shadow.color = LV_COLOR_RED;
    bar_indic.body.shadow.width = 20;
    bar_indic.body.padding.ver = 10;       /*Make the indicator thinner*/

    lv_obj_t * bar4 = lv_bar_create(lv_scr_act(), bar2);
    lv_obj_align(bar4, bar2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_bar_set_style(bar4, LV_BAR_STYLE_BG, &bar_bg);
    lv_bar_set_style(bar4, LV_BAR_STYLE_INDIC, &bar_indic);

    /* Copy 'bar4' but set its size to be vertical*/
    lv_obj_t * bar5 = lv_bar_create(lv_scr_act(), bar4);
    lv_obj_set_size(bar5, 50, 200);
    lv_obj_align(bar5, bar4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_BAR && USE_LV_TESTS*/
