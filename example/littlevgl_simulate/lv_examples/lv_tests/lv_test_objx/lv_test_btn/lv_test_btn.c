/**
 * @file lv_test_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_btn.h"

#if USE_LV_BTN && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t width_inc(lv_obj_t * btn);
static lv_res_t width_dec(lv_obj_t * btn);

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
 * Create buttons to test their functionalities
 */
void lv_test_btn_1(void)
{
    /* Create a button which looks well */
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);

    /* Create a default button manually set to toggled state*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_btn_set_state(btn2, LV_BTN_STATE_TGL_REL);

    /* Create a button which can be toggled */
    lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn3, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_btn_set_toggle(btn3, true);

    /* Test actions:
     * Press: increase width, Release: decrease width, Long press: delete */
    lv_obj_t * btn4 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn4, btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_btn_set_action(btn4, LV_BTN_ACTION_PR, width_inc);
    lv_btn_set_action(btn4, LV_BTN_ACTION_CLICK, width_dec);
    lv_btn_set_action(btn4,  LV_BTN_ACTION_LONG_PR, lv_obj_del);

    /* Test styles and copy. Same as 'btn4' but different styles */
    static lv_style_t style_rel;
    lv_style_copy(&style_rel, &lv_style_pretty);
    style_rel.body.main_color = LV_COLOR_ORANGE;
    style_rel.body.grad_color = LV_COLOR_BLACK;
    style_rel.body.border.color = LV_COLOR_RED;
    style_rel.body.shadow.color = LV_COLOR_MARRON;
    style_rel.body.shadow.width = 10;

    static lv_style_t style_pr;
    lv_style_copy(&style_pr,  &lv_style_pretty);
    style_pr.body.empty = 1;
    style_pr.body.border.color = LV_COLOR_RED;
    style_pr.body.border.width = 4;

    /*Skip 'tpr' because it will be let the same*/

    static lv_style_t style_tpr;
    lv_style_copy(&style_tpr, &lv_style_pretty);
    style_tpr.body.empty = 1;
    style_tpr.body.border.color = LV_COLOR_RED;
    style_tpr.body.border.width = 4;

    static lv_style_t style_ina;
    lv_style_copy(&style_ina, &lv_style_pretty);
    style_ina.body.main_color = LV_COLOR_SILVER;
    style_ina.body.grad_color = LV_COLOR_GRAY;
    style_ina.body.border.color = LV_COLOR_RED;

    /*Create styled button*/
    lv_obj_t * btn5 = lv_btn_create(lv_scr_act(), btn4);
    lv_obj_align(btn5, btn4, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_btn_set_style(btn5, LV_BTN_STYLE_REL, &style_rel);
    lv_btn_set_style(btn5, LV_BTN_STYLE_PR, &style_pr);
    lv_btn_set_style(btn5, LV_BTN_STYLE_TGL_PR, &style_tpr);
    lv_btn_set_style(btn5, LV_BTN_STYLE_INA, &style_ina);
    lv_btn_set_toggle(btn5, true);

    /* Test style copy and inactive state*/
    lv_obj_t * btn6 = lv_btn_create(lv_scr_act(), btn5);
    lv_obj_align(btn6, btn5, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_btn_set_state(btn6, LV_BTN_STATE_INA);

    /*Test horizontal fit and default layout (CENTER)*/
    lv_obj_t * btn7 = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_fit(btn7, true, false);
    lv_obj_t *label = lv_label_create(btn7, NULL);
    lv_label_set_text(label, "A quite long text");
    label = lv_label_create(btn7, NULL);
    lv_label_set_text(label, "Short text");
    lv_obj_align(btn7, btn4, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Increase the width of the button.
 * @param btn pointer to a button object
 */
static lv_res_t width_inc(lv_obj_t * btn)
{
    lv_obj_set_width(btn, lv_obj_get_width(btn) + (10));
    return LV_RES_OK;
}

/**
 * Decrease the width of the button.
 * @param btn pointer to a button object
 */
static lv_res_t width_dec(lv_obj_t * btn)
{
    lv_obj_set_width(btn, lv_obj_get_width(btn) - (10));
    return LV_RES_OK;
}

#endif /*USE_LV_BTN && USE_LV_TESTS*/
