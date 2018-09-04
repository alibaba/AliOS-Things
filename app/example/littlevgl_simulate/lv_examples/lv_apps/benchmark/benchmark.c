/**
 * @file benchmark.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "benchmark.h"
#if USE_LV_BENCHMARK

#include "lvgl/lv_core/lv_refr.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define TEST_CYCLE_NUM  10              /*How many times run the test (will calculate the average)*/
#define SHADOW_WIDTH    (LV_DPI / 8)
#define IMG_RECOLOR     LV_OPA_20
#define OPACITY         LV_OPA_60

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refr_monitor(uint32_t time_ms, uint32_t px_num);
static lv_res_t run_test_click(lv_obj_t * btn);
static lv_res_t wp_click(lv_obj_t * btn);
static lv_res_t recolor_click(lv_obj_t * btn);
static lv_res_t shadow_click(lv_obj_t * btn);
static lv_res_t opa_click(lv_obj_t * btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * holder_page;
static lv_obj_t * wp;
static lv_obj_t * result_label;

static lv_style_t style_wp;
static lv_style_t style_btn_rel;
static lv_style_t style_btn_pr;
static lv_style_t style_btn_tgl_rel;
static lv_style_t style_btn_tgl_pr;

static uint32_t time_sum;
static uint32_t refr_cnt;

extern lv_img_t benchmark_bg;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Open a graphics benchmark
 */
void benchmark_create(void)
{

    /*Styles of the buttons*/
    lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
    lv_style_copy(&style_btn_pr, &lv_style_btn_pr);
    lv_style_copy(&style_btn_tgl_rel, &lv_style_btn_tgl_rel);
    lv_style_copy(&style_btn_tgl_pr, &lv_style_btn_tgl_pr);

    style_btn_rel.body.opa = LV_OPA_COVER;
    style_btn_pr.body.opa  = LV_OPA_COVER;
    style_btn_tgl_rel.body.opa =LV_OPA_COVER;
    style_btn_tgl_pr.body.opa = LV_OPA_COVER;

    style_btn_rel.body.shadow.width =  0;
    style_btn_pr.body.shadow.width  =  0;
    style_btn_tgl_rel.body.shadow.width = 0;
    style_btn_tgl_pr.body.shadow.width =  0;

    /*Style of the wallpaper*/
    lv_style_copy(&style_wp, &lv_style_plain);
    style_wp.image.color = LV_COLOR_RED;

    /*Create a holder page (the page become scrollable on small displays )*/
    holder_page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_size(holder_page, LV_HOR_RES, LV_VER_RES);
    lv_page_set_style(holder_page, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(holder_page, LV_PAGE_STYLE_SCRL, &lv_style_transp);
    lv_page_set_scrl_layout(holder_page, LV_LAYOUT_PRETTY);

    /*Create a wallpaper on the page*/
	wp = lv_img_create(holder_page, NULL);
	lv_obj_set_protect(wp, LV_PROTECT_PARENT);          /*Don't let to move the wallpaper by the layout */
	lv_obj_set_parent(wp, holder_page);
	lv_obj_set_parent(lv_page_get_scrl(holder_page), holder_page);
	lv_img_set_src(wp, &benchmark_bg);
	lv_obj_set_size(wp, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_pos(wp, 0, 0);
	lv_obj_set_hidden(wp, true);
    lv_img_set_style(wp, &style_wp);
    lv_img_set_auto_size(wp, false);

	/*Create a label to show the test result*/
    result_label = lv_label_create(holder_page, NULL);
    lv_label_set_text(result_label, "Run the test");
	lv_label_set_body_draw(result_label, true);
	lv_label_set_style(result_label, &lv_style_pretty);

	/*Create a "Run test" button*/
	lv_obj_t * btn;
	btn = lv_btn_create(holder_page, NULL);
    lv_page_glue_obj(btn, true);
	lv_cont_set_fit(btn, true, true);
	lv_btn_set_style(btn, LV_BTN_STYLE_REL, &style_btn_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, &style_btn_pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, &style_btn_tgl_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, &style_btn_tgl_pr);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, run_test_click);

	lv_obj_t * btn_l;
	btn_l = lv_label_create(btn, NULL);
	lv_label_set_text(btn_l, "Run\ntest!");
	lv_obj_set_protect(btn, LV_PROTECT_FOLLOW);     /*Line break in layout*/


    /*Create a "Wallpaper show" button*/
    btn = lv_btn_create(holder_page, btn);
    lv_btn_set_toggle(btn, true);
    lv_obj_clear_protect(btn, LV_PROTECT_FOLLOW);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, wp_click);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Wallpaper");


    /*Create a "Wallpaper re-color" button*/
    btn = lv_btn_create(holder_page, btn);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, recolor_click);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Wp. recolor!");

    /*Create a "Shadow draw" button*/
    btn = lv_btn_create(holder_page, btn);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, shadow_click);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Shadow");

    /*Create an "Opacity enable" button*/
    btn = lv_btn_create(holder_page, btn);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, opa_click);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Opacity");
}


void benchmark_start(void)
{
    lv_refr_set_monitor_cb(refr_monitor);
    lv_obj_invalidate(lv_scr_act());
    time_sum = 0;
    refr_cnt = 0;
}

bool benchmark_is_ready(void)
{
    if(refr_cnt == TEST_CYCLE_NUM) return true;
    else return false;
}

uint32_t benchmark_get_refr_time(void)
{
    if(benchmark_is_ready()) return time_sum / TEST_CYCLE_NUM;
    else return 0;
}

/*--------------------
 * OTHER FUNCTIONS
 ---------------------*/

/**
 * Called when a the library finished rendering to monitor its performance
 * @param time_ms time of rendering in milliseconds
 * @param px_num Number of pixels drawn
 */
static void refr_monitor(uint32_t time_ms, uint32_t px_num)
{
    time_sum += time_ms;
    refr_cnt ++;
    lv_obj_invalidate(lv_scr_act());

    if(refr_cnt >= TEST_CYCLE_NUM) {
        float time_avg = (float)time_sum / (float)TEST_CYCLE_NUM;
        char buf[256];
        if(time_sum != 0) sprintf(buf, "Screen load: %0.1f ms\nAverage of %d", time_avg, TEST_CYCLE_NUM);
        lv_label_set_text(result_label, buf);

        lv_refr_set_monitor_cb(NULL);
    } else {
        char buf[256];
        sprintf(buf, "Running %d/%d", refr_cnt, TEST_CYCLE_NUM);
        lv_label_set_text(result_label, buf);

    }
}

/**
 * Called when the "Run test" button is clicked
 * @param btn pointer to the button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t run_test_click(lv_obj_t * btn)
{
    benchmark_start();

    return LV_RES_OK;
}

/**
 * Called when the "Wallpaper" button is clicked
 * @param btn pointer to the button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t wp_click(lv_obj_t * btn)
{
    if(lv_btn_get_state(btn) == LV_BTN_STATE_TGL_REL) lv_obj_set_hidden(wp, false);
    else lv_obj_set_hidden(wp, true);

    return LV_RES_OK;
}

/**
 * Called when the "Wp. recolor" button is clicked
 * @param btn pointer to the button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t recolor_click(lv_obj_t * btn)
{

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TGL_REL) style_wp.image.intense = IMG_RECOLOR;
    else style_wp.image.intense = LV_OPA_TRANSP;

    lv_obj_refresh_style(wp);

    return LV_RES_OK;
}

/**
 * Called when the "Shadow" button is clicked
 * @param btn pointer to the button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t shadow_click(lv_obj_t * btn)
{
    if(lv_btn_get_state(btn) == LV_BTN_STATE_TGL_REL) {
        style_btn_rel.body.shadow.width = SHADOW_WIDTH;
        style_btn_pr.body.shadow.width  =  SHADOW_WIDTH;
        style_btn_tgl_rel.body.shadow.width = SHADOW_WIDTH;
        style_btn_tgl_pr.body.shadow.width =  SHADOW_WIDTH;
    } else {
        style_btn_rel.body.shadow.width = 0;
        style_btn_pr.body.shadow.width  = 0;
        style_btn_tgl_rel.body.shadow.width = 0;
        style_btn_tgl_pr.body.shadow.width = 0;
    }

    lv_obj_report_style_mod(&style_btn_rel);
    lv_obj_report_style_mod(&style_btn_pr);
    lv_obj_report_style_mod(&style_btn_tgl_rel);
    lv_obj_report_style_mod(&style_btn_tgl_pr);

    return LV_RES_OK;
}

/**
 * Called when the "Opacity" button is clicked
 * @param btn pointer to the button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static lv_res_t opa_click(lv_obj_t * btn)
{
    if(lv_btn_get_state(btn) == LV_BTN_STATE_TGL_REL) {
        style_btn_rel.body.opa = OPACITY;
        style_btn_pr.body.opa  = OPACITY;
        style_btn_tgl_rel.body.opa = OPACITY;
        style_btn_tgl_pr.body.opa = OPACITY;
    } else {
        style_btn_rel.body.opa = LV_OPA_COVER;
        style_btn_pr.body.opa  = LV_OPA_COVER;
        style_btn_tgl_rel.body.opa =LV_OPA_COVER;
        style_btn_tgl_pr.body.opa = LV_OPA_COVER;
    }

    lv_obj_report_style_mod(&style_btn_rel);
    lv_obj_report_style_mod(&style_btn_pr);
    lv_obj_report_style_mod(&style_btn_tgl_rel);
    lv_obj_report_style_mod(&style_btn_tgl_pr);

    return LV_RES_OK;
}

#endif /*USE_LV_BENCHMARK*/
