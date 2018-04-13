/**
 * @file tpcal.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "stdio.h"

/*********************
 *      DEFINES
 *********************/
#define CIRCLE_SIZE      20
#define TP_MAX_VALUE     5000

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    TP_CAL_STATE_INIT,
    TP_CAL_STATE_WAIT_TOP_LEFT,
    TP_CAL_STATE_WAIT_TOP_RIGHT,
    TP_CAL_STATE_WAIT_BOTTOM_RIGHT,
    TP_CAL_STATE_WAIT_BOTTOM_LEFT,
    TP_CAL_STATE_WAIT_LEAVE,
    TP_CAL_STATE_READY,
}tp_cal_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t btn_click_action(lv_obj_t *scr);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_point_t p[4]; /*Calibration points: p[0]: top-left; p[1]: top-right, p[2]: bottom-right, p[3]: bottom-left */
static tp_cal_state_t state;
static lv_obj_t *prev_scr;
static lv_obj_t *big_btn;
static lv_obj_t *label_main;
static lv_obj_t *circ_area;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a touch pad calibration screen
 */
void tpcal_create(void)
{
    state = TP_CAL_STATE_INIT;

    prev_scr = lv_scr_act();

    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_obj_set_size(scr, TP_MAX_VALUE, TP_MAX_VALUE);
    lv_scr_load(scr);

    /*Create a big transparent button screen to receive clicks*/
    big_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(big_btn, TP_MAX_VALUE, TP_MAX_VALUE);
    lv_btn_set_style(big_btn, LV_BTN_STYLE_REL, &lv_style_transp);
    lv_btn_set_style(big_btn, LV_BTN_STYLE_PR, &lv_style_transp);
    lv_btn_set_action(big_btn, LV_BTN_ACTION_CLICK, btn_click_action);
    lv_btn_set_layout(big_btn, LV_LAYOUT_OFF);

    label_main = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_main, "Click the circle in\n"
                                  "upper left-hand corner");
    lv_label_set_align(label_main, LV_LABEL_ALIGN_CENTER);

    lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                               (LV_VER_RES - lv_obj_get_height(label_main)) / 2);


    static lv_style_t style_circ;
    lv_style_copy(&style_circ, &lv_style_pretty_color);
    style_circ.body.radius = LV_RADIUS_CIRCLE;

    circ_area = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(circ_area, CIRCLE_SIZE, CIRCLE_SIZE);
    lv_obj_set_style(circ_area, &style_circ);
    lv_obj_set_click(circ_area, false);

    lv_anim_t a;
    a.var = circ_area;
    a.start = LV_HOR_RES / 2;
    a.end = 0;
    a.fp = (lv_anim_fp_t)lv_obj_set_x;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = -500;
    a.time = 200;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    lv_anim_create(&a);

    a.start = LV_VER_RES / 2;
    a.end = 0;
    a.fp = (lv_anim_fp_t)lv_obj_set_y;
    a.end_cb = NULL;
    a.time = 200;
    lv_anim_create(&a);

    state = TP_CAL_STATE_WAIT_TOP_LEFT;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t btn_click_action(lv_obj_t *scr)
{
    if(state == TP_CAL_STATE_WAIT_TOP_LEFT) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &p[0]);

        char buf[64];
        sprintf(buf, "x: %d\ny: %d", p[0].x, p[0].y);
        lv_obj_t *label_coord = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(label_coord, buf);

        lv_label_set_text(label_main, "Click the circle in\n"
                                      "upper right-hand corner");

        lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                                   (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

        lv_anim_t a;
        a.var = circ_area;
        a.start = 0;
        a.end = LV_HOR_RES - CIRCLE_SIZE;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = 200;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);

        a.start = 0;
        a.end = 0;
        a.fp = (lv_anim_fp_t)lv_obj_set_y;
        a.end_cb = NULL;
        a.time = 200;
        lv_anim_create(&a);

        state = TP_CAL_STATE_WAIT_TOP_RIGHT;
    }
    else if(state == TP_CAL_STATE_WAIT_TOP_RIGHT) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &p[1]);

        char buf[64];
        sprintf(buf, "x: %d\ny: %d", p[1].x, p[1].y);
        lv_obj_t *label_coord = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(label_coord, buf);
        lv_obj_set_pos(label_coord, LV_HOR_RES - lv_obj_get_width(label_coord), 0);

        lv_label_set_text(label_main, "Click the circle in\n"
                                      "lower right-hand corner");

        lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                                   (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

        lv_anim_t a;
        a.var = circ_area;
        a.start = LV_HOR_RES - CIRCLE_SIZE;
        a.end = LV_HOR_RES - CIRCLE_SIZE;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = 200;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);

        a.start = 0;
        a.end = LV_VER_RES - CIRCLE_SIZE;
        a.fp = (lv_anim_fp_t)lv_obj_set_y;
        a.end_cb = NULL;
        a.time = 200;
        lv_anim_create(&a);

        state = TP_CAL_STATE_WAIT_BOTTOM_RIGHT;
    }
    else if(state == TP_CAL_STATE_WAIT_BOTTOM_RIGHT) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &p[2]);

        char buf[64];
        sprintf(buf, "x: %d\ny: %d", p[2].x, p[2].y);
        lv_obj_t *label_coord = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(label_coord, buf);
        lv_obj_set_pos(label_coord, LV_HOR_RES - lv_obj_get_width(label_coord),
                                    LV_VER_RES - lv_obj_get_height(label_coord));

        lv_label_set_text(label_main, "Click the circle in\n"
                                      "lower left-hand corner");

        lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                                   (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

        lv_anim_t a;
        a.var = circ_area;
        a.start = LV_HOR_RES - CIRCLE_SIZE;
        a.end = 0;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = 200;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);

        a.start = LV_VER_RES - CIRCLE_SIZE;
        a.end = LV_VER_RES - CIRCLE_SIZE;
        a.fp = (lv_anim_fp_t)lv_obj_set_y;
        a.end_cb = NULL;
        a.time = 200;
        lv_anim_create(&a);

        state = TP_CAL_STATE_WAIT_BOTTOM_LEFT;
    }

    else if(state == TP_CAL_STATE_WAIT_BOTTOM_LEFT) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &p[3]);

        lv_label_set_text(label_main, "Click the screen\n"
                                      "to leave calibration");

        lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                                   (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

        char buf[64];
        sprintf(buf, "x: %d\ny: %d", p[3].x, p[3].y);
        lv_obj_t *label_coord = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(label_coord, buf);
        lv_obj_set_pos(label_coord, 0, LV_VER_RES - lv_obj_get_height(label_coord));

        lv_obj_del(circ_area);

        state = TP_CAL_STATE_WAIT_LEAVE;
    }
    else if(state == TP_CAL_STATE_WAIT_LEAVE) {
        lv_scr_load(prev_scr);

        /*
         * TODO Process 'p' points here to calibrate the touch pad
         */

        state = TP_CAL_STATE_READY;


    }
    else if(state == TP_CAL_STATE_READY) {
    }

    return LV_RES_OK;
}
