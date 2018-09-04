/**
 * @file lv_test_win.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_win.h"

#if USE_LV_WIN && USE_LV_TESTS

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
 * Create windows to test their functionalities
 */
void lv_test_win_1(void)
{

    lv_obj_t *win1 = lv_win_create(lv_scr_act(), NULL);
    lv_obj_set_size(win1, LV_HOR_RES / 2 - LV_DPI / 20, LV_VER_RES / 2 - LV_DPI / 20);

    lv_obj_t *win2 = lv_win_create(lv_scr_act(), win1);
    lv_obj_align(win2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_win_set_title(win2, "Random title");
    lv_win_add_btn(win2, SYMBOL_CLOSE, NULL);
    lv_win_add_btn(win2, SYMBOL_OK, NULL);
    lv_win_add_btn(win2, SYMBOL_EDIT, NULL);

    lv_obj_t *label = lv_label_create(win2, NULL);
    lv_obj_set_pos(label, 10, 10);
    lv_label_set_text(label, "Long\n\n\ntext\n\n\nto\n\n\nsee\n\n\nthe\n\n\nscrollbars");


    static lv_style_t header;
    lv_style_copy(&header, &lv_style_plain);
    header.body.main_color = LV_COLOR_RED;
    header.body.grad_color = LV_COLOR_MARRON;
    header.body.padding.inner = 0;
    header.text.color = LV_COLOR_WHITE;

    lv_obj_t *win3 = lv_win_create(lv_scr_act(), win2);
    lv_obj_align(win3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_win_set_style(win3, LV_WIN_STYLE_HEADER, &header);
    lv_win_set_style(win3, LV_WIN_STYLE_BTN_REL, &lv_style_transp);
    lv_win_set_style(win3, LV_WIN_STYLE_CONTENT_BG, &lv_style_plain_color);
    lv_win_set_style(win3, LV_WIN_STYLE_CONTENT_SCRL, &lv_style_plain);
    lv_win_set_style(win3, LV_WIN_STYLE_BG, &lv_style_plain_color);
    lv_win_set_btn_size(win3, LV_DPI / 3);

    label = lv_label_create(win3, NULL);
    lv_obj_set_pos(label, 10, 10);
    lv_label_set_text(label, "Styled window\n\nThe content background has\ndifferent color");

    lv_obj_t *win4 = lv_win_create(lv_scr_act(), win3);
    lv_obj_align(win4, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_WIN && USE_LV_TESTS*/
