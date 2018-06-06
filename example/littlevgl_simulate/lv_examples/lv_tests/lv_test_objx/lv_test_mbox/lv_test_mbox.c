/**
 * @file lv_test_mbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_mbox.h"
#if USE_LV_MBOX && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t mbox_action(lv_obj_t *btn, const char *txt);

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
 * Create message boxes to test their functionalities
 */
void lv_test_mbox_1(void)
{
    /* Default object */
    lv_obj_t *mbox1 = lv_mbox_create(lv_scr_act(), NULL);
    lv_obj_set_pos(mbox1, 10, 10);

    /*Add buttons and modify text*/
    static const char * btns2[] = {"Ok", "Cancel", ""};
    lv_obj_t *mbox2 = lv_mbox_create(lv_scr_act(), NULL);
    lv_mbox_add_btns(mbox2, btns2, NULL);
    lv_mbox_set_text(mbox2, "Message");
    lv_obj_set_width(mbox2, LV_HOR_RES / 2);
    lv_obj_align(mbox2, mbox1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Add styles*/
    static lv_style_t bg;
    static lv_style_t btn_bg;
    lv_style_copy(&bg, &lv_style_pretty);
    lv_style_copy(&btn_bg, &lv_style_pretty);
    bg.body.padding.hor = 20;
    bg.body.padding.ver = 20;
    bg.body.padding.inner = 20;
    bg.body.main_color = LV_COLOR_BLACK;
    bg.body.grad_color = LV_COLOR_MARRON;
    bg.text.color = LV_COLOR_WHITE;

    btn_bg.body.padding.hor = 10;
    btn_bg.body.padding.ver = 5;
    btn_bg.body.padding.inner = 40;
    btn_bg.body.empty = 1;
    btn_bg.body.border.color = LV_COLOR_WHITE;
    btn_bg.text.color = LV_COLOR_WHITE;

    static lv_style_t btn_rel;
    lv_style_copy(&btn_rel, &lv_style_btn_rel);
    btn_rel.body.empty = 1;
    btn_rel.body.border.color = LV_COLOR_WHITE;

    lv_obj_t *mbox3 = lv_mbox_create(lv_scr_act(), mbox2);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BTN_REL, &btn_rel);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BTN_BG,  &btn_bg);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BG, &bg);
    lv_obj_align(mbox3, mbox1, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_mbox_set_action(mbox3, mbox_action);

    /*Copy with styles and set button width*/
    lv_obj_t *mbox4 = lv_mbox_create(lv_scr_act(), mbox3);
    lv_mbox_set_text(mbox4, "A quite long message text which is\n"
                            "manually broken into multiple lines");

    static const char * btns3[] = {"Ok", "Cancel", "Third", ""};
    lv_mbox_add_btns(mbox4, btns3, mbox_action);
    lv_obj_align(mbox4, mbox3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t mbox_action(lv_obj_t *btn, const char *txt)
{
    lv_obj_t *mbox = lv_mbox_get_from_btn(btn);

    lv_mbox_set_text(mbox, txt);

    return LV_RES_OK;
}

#endif /*USE_LV_MBOX && USE_LV_TESTS*/
