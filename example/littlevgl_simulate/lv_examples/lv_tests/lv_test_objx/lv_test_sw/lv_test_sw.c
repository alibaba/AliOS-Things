/**
 * @file lv_test_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/
#include "lv_test_sw.h"

#if USE_LV_SW && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t sw_action(lv_obj_t *sw);

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
 * Create switches to test their functionalities
 */
void lv_test_sw_1(void)
{
    /* Default object */
    lv_obj_t *sw1 = lv_sw_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw1, 10, 10);
    lv_sw_set_action(sw1, sw_action);

    static lv_style_t bg;
    static lv_style_t indic;

    lv_style_copy(&bg, &lv_style_pretty);
    bg.body.padding.hor = -5;
    bg.body.padding.ver = -5;

    lv_style_copy(&indic, &lv_style_pretty_color);
    indic.body.padding.hor = 8;
    indic.body.padding.ver = 8;

    lv_obj_t *sw2 = lv_sw_create(lv_scr_act(), sw1);
    lv_sw_set_style(sw2, LV_SW_STYLE_BG, &bg);
    lv_sw_set_style(sw2, LV_SW_STYLE_INDIC, &indic);
    lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_OFF, &lv_style_btn_pr);
    lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_ON, &lv_style_btn_tgl_pr);

    lv_sw_on(sw2);
    lv_obj_align(sw2, sw1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    lv_obj_t *sw3 = lv_sw_create(lv_scr_act(), sw2);
    lv_obj_align(sw3, sw2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t sw_action(lv_obj_t *sw)
{
#if LV_EX_PRINTF
    printf("Switch state: %d\n", lv_sw_get_state(sw));
#endif
    return LV_RES_OK;
}


#endif /*USE_LV_SW && USE_LV_TESTS*/
