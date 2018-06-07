/**
 * @file lv_test_tabview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_tabview.h"

#if USE_LV_TABVIEW && USE_LV_TESTS

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
 * Create tab views to test their functionalities
 */
void lv_test_tabview_1(void)
{
    /* Default object. It will be an empty tab view*/
    lv_obj_t *tv1 = lv_tabview_create(lv_scr_act(), NULL);
    lv_tabview_add_tab(tv1, "First");
    lv_tabview_add_tab(tv1, "Second");
    lv_obj_set_size(tv1, LV_HOR_RES / 2 - 10, LV_VER_RES / 2 - 10);

    /*Copy the first tabview and add some texts*/
    lv_obj_t *tv2 = lv_tabview_create(lv_scr_act(), tv1);
    lv_obj_align(tv2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t *tab = lv_tabview_get_tab(tv2, 0);
    lv_obj_t *label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv2, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is the second tab");


    /*Create styles*/
    static lv_style_t bg;
    static lv_style_t sb;
    static lv_style_t btns_bg;
    static lv_style_t tab_bg;
    static lv_style_t rel;
    static lv_style_t pr;
    static lv_style_t tgl_rel;
    static lv_style_t tgl_pr;
    static lv_style_t indic;

    lv_style_copy(&btns_bg, &lv_style_plain_color);
    lv_style_copy(&tab_bg, &lv_style_pretty_color);
    lv_style_copy(&bg, &lv_style_pretty_color);
    lv_style_copy(&sb, &lv_style_pretty);
    lv_style_copy(&btns_bg, &lv_style_transp_fit);
    lv_style_copy(&rel, &lv_style_plain);
    lv_style_copy(&pr, &lv_style_plain);
    lv_style_copy(&tgl_rel, &lv_style_plain);
    lv_style_copy(&tgl_pr, &lv_style_plain);
    lv_style_copy(&indic, &lv_style_plain);

    rel.body.main_color = LV_COLOR_SILVER;
    pr.body.main_color = LV_COLOR_GRAY;
    tgl_rel.body.main_color = LV_COLOR_RED;
    tgl_pr.body.main_color = LV_COLOR_MARRON;
    indic.body.main_color = LV_COLOR_ORANGE;
    indic.body.grad_color = LV_COLOR_ORANGE;
    indic.body.padding.inner = LV_DPI / 16;
    tab_bg.body.main_color = LV_COLOR_SILVER;
    tab_bg.body.grad_color = LV_COLOR_GREEN;
    tab_bg.text.color = LV_COLOR_YELLOW;

    /*Apply the styles*/
    lv_obj_t *tv3 = lv_tabview_create(lv_scr_act(), tv2);
    lv_obj_align(tv3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BG, &bg);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_BG, &btns_bg);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_REL, &rel);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_PR, &pr);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_TGL_REL, &tgl_rel);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_TGL_PR, &tgl_pr);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_INDIC, &indic);

    tab = lv_tabview_get_tab(tv3, 0);
    lv_page_set_style(tab, LV_PAGE_STYLE_BG, &tab_bg);
    lv_page_set_style(tab, LV_PAGE_STYLE_SB, &sb);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv3, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is the second tab");

    /*Copy the styles tab view*/
    lv_obj_t *tv4 = lv_tabview_create(lv_scr_act(), tv3);
    lv_obj_align(tv4, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}



/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_TABVIEW && USE_LV_TESTS*/
