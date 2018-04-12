/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define DEMO_WALLPAPER  1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void write_create(lv_obj_t *parent);
static void list_create(lv_obj_t *parent);
static void chart_create(lv_obj_t *parent);
static lv_res_t slider_action(lv_obj_t *slider);
static lv_res_t list_btn_action(lv_obj_t *slider);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *chart;
static lv_obj_t *ta;
#if DEMO_WALLPAPER
LV_IMG_DECLARE(img_bubble_pattern);
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void demo_create(void)
{
#if DEMO_WALLPAPER
    lv_img_create_file("bg", img_bubble_pattern);
    lv_obj_t *wp = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_upscale(wp, true);
    lv_img_set_file(wp, "U:/bg");
    lv_obj_set_width(wp, LV_HOR_RES * 4);
    lv_obj_set_protect(wp, LV_PROTECT_POS);
#endif

    static lv_style_t style_tv_btn_bg;
    lv_style_copy(&style_tv_btn_bg, &lv_style_plain);
    style_tv_btn_bg.body.main_color = LV_COLOR_HEX(0x487fb7);
    style_tv_btn_bg.body.grad_color = LV_COLOR_HEX(0x487fb7);
    style_tv_btn_bg.body.padding.ver = 0;

    static lv_style_t style_tv_btn_rel;
    lv_style_copy(&style_tv_btn_rel, &lv_style_btn_rel);
    style_tv_btn_rel.body.empty = 1;
    style_tv_btn_rel.body.border.width = 0;

    static lv_style_t style_tv_btn_pr;
    lv_style_copy(&style_tv_btn_pr, &lv_style_btn_pr);
    style_tv_btn_pr.body.radius = 0;
    style_tv_btn_pr.body.opa = LV_OPA_50;
    style_tv_btn_pr.body.main_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.grad_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.border.width = 0;
    style_tv_btn_pr.text.color = LV_COLOR_GRAY;

    lv_obj_t *tv = lv_tabview_create(lv_scr_act(), NULL);

#if DEMO_WALLPAPER
    lv_obj_set_parent(wp, ((lv_tabview_ext_t *) tv->ext_attr)->content);
    lv_obj_set_pos(wp, 0, -5);
#endif

    lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Write");
    lv_obj_t *tab2 = lv_tabview_add_tab(tv, "List");
    lv_obj_t *tab3 = lv_tabview_add_tab(tv, "Chart");

#if DEMO_WALLPAPER == 0
    /*Blue bg instead of wallpaper*/
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BG, &style_tv_btn_bg);
#endif
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_BG, &style_tv_btn_bg);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_PR, &style_tv_btn_pr);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_PR, &style_tv_btn_pr);

    write_create(tab1);
    list_create(tab2);
    chart_create(tab3);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void write_create(lv_obj_t *parent)
{
//    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    static lv_style_t style_ta;
    lv_style_copy(&style_ta, &lv_style_pretty);
    style_ta.body.opa = LV_OPA_30;
    style_ta.body.radius = 0;
    style_ta.text.color = LV_COLOR_HEX3(0x222);

    static lv_style_t style_kb;
    lv_style_copy(&style_kb, &lv_style_plain);
    style_kb.body.opa = LV_OPA_70;
    style_kb.body.main_color = LV_COLOR_HEX3(0x333);
    style_kb.body.grad_color = LV_COLOR_HEX3(0x333);
    style_kb.body.padding.hor = 0;
    style_kb.body.padding.ver = 0;
    style_kb.body.padding.inner = 0;

    static lv_style_t style_kb_rel;
    lv_style_copy(&style_kb_rel, &lv_style_plain);
    style_kb_rel.body.empty = 1;
    style_kb_rel.body.radius = 0;
    style_kb_rel.body.border.width = 1;
    style_kb_rel.body.border.color = LV_COLOR_SILVER;
    style_kb_rel.body.border.opa = LV_OPA_50;
    style_kb_rel.text.color = LV_COLOR_WHITE;

    static lv_style_t style_kb_pr;
    lv_style_copy(&style_kb_pr, &lv_style_plain);
    style_kb_pr.body.radius = 0;
    style_kb_pr.body.opa = LV_OPA_50;
    style_kb_pr.body.main_color = LV_COLOR_WHITE;
    style_kb_pr.body.grad_color = LV_COLOR_WHITE;
    style_kb_pr.body.border.width = 1;
    style_kb_pr.body.border.color = LV_COLOR_SILVER;

    ta = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) / 2);
    lv_ta_set_style(ta, LV_TA_STYLE_BG, &style_ta);
    lv_ta_set_text(ta, "");

    lv_obj_t *kb = lv_kb_create(parent, NULL);
    lv_obj_set_size(kb, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) / 2);
    lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_kb_set_ta(kb, ta);
    lv_kb_set_style(kb, LV_KB_STYLE_BG, &style_kb);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &style_kb_rel);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &style_kb_pr);
}

static void list_create(lv_obj_t *parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_scrl_fit(parent, false, false);
    lv_page_set_scrl_height(parent, lv_obj_get_height(parent));
    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    /*Create styles for the buttons*/
    static lv_style_t style_btn_rel;
    static lv_style_t style_btn_pr;
    lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
    style_btn_rel.body.main_color = LV_COLOR_HEX3(0x333);
    style_btn_rel.body.grad_color = LV_COLOR_BLACK;
    style_btn_rel.body.border.color = LV_COLOR_SILVER;
    style_btn_rel.body.border.width = 1;
    style_btn_rel.body.border.opa = LV_OPA_50;
    style_btn_rel.body.radius = 0;

    lv_style_copy(&style_btn_pr, &style_btn_rel);
    style_btn_pr.body.main_color = LV_COLOR_MAKE(0x55, 0x96, 0xd8);
    style_btn_pr.body.grad_color = LV_COLOR_MAKE(0x37, 0x62, 0x90);
    style_btn_pr.text.color = LV_COLOR_MAKE(0xbb, 0xd5, 0xf1);

    lv_obj_t *list = lv_list_create(parent, NULL);
    lv_obj_set_height(list, 2 * lv_obj_get_height(parent) / 3);
    lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, &style_btn_rel);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_PR, &style_btn_pr);
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 4);

    lv_list_add(list, SYMBOL_FILE, "New", list_btn_action);
    lv_list_add(list, SYMBOL_DIRECTORY, "Open", list_btn_action);
    lv_list_add(list, SYMBOL_TRASH, "Delete", list_btn_action);
    lv_list_add(list, SYMBOL_EDIT, "Edit", list_btn_action);
    lv_list_add(list, SYMBOL_SAVE, "Save", list_btn_action);
    lv_list_add(list, SYMBOL_WIFI, "WiFi", list_btn_action);
    lv_list_add(list, SYMBOL_GPS, "GPS", list_btn_action);

    lv_obj_t *mbox= lv_mbox_create(parent, NULL);
    lv_mbox_set_text(mbox, "Click a button to copy its text to the Text area ");
    lv_obj_set_width(mbox, LV_HOR_RES - LV_DPI);
    static const char * mbox_btns[] = {"Got it", ""};
    lv_mbox_add_btns(mbox, mbox_btns, NULL);    /*The default action is close*/
    lv_obj_align(mbox, parent, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);
}

static void chart_create(lv_obj_t *parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_scrl_fit(parent, false, false);
    lv_page_set_scrl_height(parent, lv_obj_get_height(parent));
    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    static lv_style_t style_chart;
    lv_style_copy(&style_chart, &lv_style_pretty);
    style_chart.body.opa = LV_OPA_60;
    style_chart.body.radius = 0;
    style_chart.line.color = LV_COLOR_GRAY;

    chart = lv_chart_create(parent, NULL);
    lv_obj_set_size(chart, 2 * lv_obj_get_width(parent) / 3, lv_obj_get_height(parent) / 2);
    lv_obj_align(chart, NULL,  LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 4);
    lv_chart_set_type(chart, LV_CHART_TYPE_COLUMN);
    lv_chart_set_style(chart, &style_chart);
    lv_chart_set_series_opa(chart, LV_OPA_70);
    lv_chart_series_t *ser1;
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, ser1, 40);
    lv_chart_set_next(chart, ser1, 30);
    lv_chart_set_next(chart, ser1, 47);
    lv_chart_set_next(chart, ser1, 59);
    lv_chart_set_next(chart, ser1, 59);
    lv_chart_set_next(chart, ser1, 31);
    lv_chart_set_next(chart, ser1, 55);
    lv_chart_set_next(chart, ser1, 70);
    lv_chart_set_next(chart, ser1, 82);

    /*Create a bar, an indicator and a knob style*/
    static lv_style_t style_bar;
    static lv_style_t style_indic;
    static lv_style_t style_knob;

    lv_style_copy(&style_bar, &lv_style_pretty);
    style_bar.body.main_color =  LV_COLOR_BLACK;
    style_bar.body.grad_color =  LV_COLOR_GRAY;
    style_bar.body.radius = LV_RADIUS_CIRCLE;
    style_bar.body.border.color = LV_COLOR_WHITE;
    style_bar.body.opa = LV_OPA_60;
    style_bar.body.padding.hor = 0;
    style_bar.body.padding.ver = LV_DPI / 6;

    lv_style_copy(&style_indic, &lv_style_pretty);
    style_indic.body.grad_color =  LV_COLOR_MARRON;
    style_indic.body.main_color =  LV_COLOR_RED;
    style_indic.body.radius = LV_RADIUS_CIRCLE;
    style_indic.body.shadow.width = LV_DPI / 10;
    style_indic.body.shadow.color = LV_COLOR_RED;
    style_indic.body.padding.hor = LV_DPI / 30;
    style_indic.body.padding.ver = LV_DPI / 30;

    lv_style_copy(&style_knob, &lv_style_pretty);
    style_knob.body.radius = LV_RADIUS_CIRCLE;
    style_knob.body.opa = LV_OPA_70;

    /*Create a second slider*/
    lv_obj_t *slider = lv_slider_create(parent, NULL);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_BG, &style_bar);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_INDIC, &style_indic);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_KNOB, &style_knob);
    lv_obj_set_size(slider, lv_obj_get_width(chart), LV_DPI / 2);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, - LV_DPI / 4); /*Align below the slider*/
    lv_slider_set_action(slider, slider_action);
    lv_slider_set_range(slider, 10, 1000);
    lv_slider_set_value(slider, 700);
    slider_action(slider);          /*Simulate a user value set the refresh the chart*/
}

/**
 * Called when a new value on the slider on the Chart tab is set
 * @param slider pointer to the slider
 * @return LV_RES_OK because the slider is not deleted in the function
 */
static lv_res_t slider_action(lv_obj_t *slider)
{
    int16_t v = lv_slider_get_value(slider);
    v = 1000 * 100 / v; /*Convert to range modify values linearly*/
    lv_chart_set_range(chart, 0, v);

    return LV_RES_OK;
}

/**
 * Called when a a list button is clicked on the List tab
 * @param btn pointer to a list button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t list_btn_action(lv_obj_t *btn)
{
    lv_ta_add_char(ta, '\n');
    lv_ta_add_text(ta, lv_list_get_btn_text(btn));

    return LV_RES_OK;
}
