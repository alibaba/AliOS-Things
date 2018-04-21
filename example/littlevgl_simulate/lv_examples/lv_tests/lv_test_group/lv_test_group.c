/**
 * @file lv_test_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_group.h"
#include "lvgl/lv_hal/lv_hal_indev.h"

#define ADD_KEYBOARD   0        /*Required: USE_KEYBOARD   1   in lv_drv_conf.h (lv_drivers repository)*/

#if ADD_KEYBOARD
#include "lv_drivers/indev/keyboard.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool win_btn_get(lv_indev_data_t *data);
static lv_res_t win_btn_acion(lv_obj_t *btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_key;
static lv_group_t *g;
static lv_obj_t *win;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create base groups to test their functionalities
 */
void lv_test_group_1(void)
{

    g = lv_group_create();

    /*A keyboard will be simulated*/
    lv_indev_drv_t kb_drv;
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read = win_btn_get;
    lv_indev_t *win_kb_indev = lv_indev_drv_register(&kb_drv);
    lv_indev_set_group(win_kb_indev, g);

#if ADD_KEYBOARD
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read = keyboard_read;
    lv_indev_t *kb_indev = lv_indev_register(&kb_drv);
    lv_indev_set_group(kb_indev, g);
#endif
    /*Create a window to hold all the objects*/
    static lv_style_t win_style;
    lv_style_copy(&win_style, &lv_style_transp);
    win_style.body.padding.hor = LV_DPI / 4;
    win_style.body.padding.ver = LV_DPI / 4;
    win_style.body.padding.inner = LV_DPI / 4;


    win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Group test");
    lv_win_set_layout(win, LV_LAYOUT_PRETTY);
    lv_win_set_style(win, LV_WIN_STYLE_CONTENT_SCRL, &win_style);

    lv_obj_t *win_btn = lv_win_add_btn(win, SYMBOL_RIGHT, win_btn_acion);
    lv_obj_set_free_num(win_btn, LV_GROUP_KEY_RIGHT);

    win_btn = lv_win_add_btn(win, SYMBOL_NEXT, win_btn_acion);
    lv_obj_set_free_num(win_btn, LV_GROUP_KEY_NEXT);

    win_btn = lv_win_add_btn(win, SYMBOL_OK, win_btn_acion);
    lv_obj_set_free_num(win_btn, LV_GROUP_KEY_ENTER);

    win_btn = lv_win_add_btn(win, SYMBOL_PREV, win_btn_acion);
    lv_obj_set_free_num(win_btn, LV_GROUP_KEY_PREV);

    win_btn = lv_win_add_btn(win, SYMBOL_LEFT, win_btn_acion);
    lv_obj_set_free_num(win_btn, LV_GROUP_KEY_LEFT);

#if LV_TXT_UTF8 == 0
    win_btn = lv_win_add_btn(win, "a", win_btn_acion);
    lv_obj_set_free_num(win_btn, 'a');
#else
    win_btn = lv_win_add_btn(win, "Ű", win_btn_acion);
    lv_obj_set_free_num(win_btn, 0xB0C5);               /*UTF-8 code of 'Ű'*/
#endif


    lv_obj_t *obj;

    obj = lv_obj_create(win, NULL);
    lv_obj_set_style(obj, &lv_style_plain_color);
    lv_group_add_obj(g, obj);

    obj = lv_label_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_btn_create(win, NULL);
    lv_group_add_obj(g, obj);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "Button");

    obj = lv_cb_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_bar_create(win, NULL);
    lv_bar_set_value(obj, 60);
    lv_group_add_obj(g, obj);

    obj = lv_slider_create(win, NULL);
    lv_slider_set_range(obj, 0, 10);
    lv_group_add_obj(g, obj);

    obj = lv_sw_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_ddlist_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_roller_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_btnm_create(win, NULL);
    lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 3);
    lv_group_add_obj(g, obj);

    lv_obj_t *ta = lv_ta_create(win, NULL);
    lv_ta_set_cursor_type(ta, LV_CURSOR_NONE);
    lv_group_add_obj(g, ta);

    obj = lv_kb_create(win, NULL);
    lv_obj_set_size(obj, lv_win_get_width(win), LV_VER_RES / 2);
    lv_kb_set_ta(obj, ta);
    lv_group_add_obj(g, obj);

    static const char *mbox_btns[] = {"Yes", "No", ""};
    obj = lv_mbox_create(win, NULL);
    lv_mbox_add_btns(obj, mbox_btns, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_list_create(win, NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 1", NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 2", NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 3", NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 4", NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 5", NULL);
    lv_list_add(obj, SYMBOL_FILE, "File 6", NULL);
    lv_group_add_obj(g, obj);


    obj = lv_page_create(win, NULL);
    lv_obj_set_size(obj, 2 * LV_DPI, LV_DPI);
    lv_group_add_obj(g, obj);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "I'm a page");

    obj = lv_lmeter_create(win, NULL);
    lv_lmeter_set_value(obj, 60);
    lv_group_add_obj(g, obj);

    static lv_color_t needle_color[] = {LV_COLOR_RED};
    obj = lv_gauge_create(win, NULL);
    lv_gauge_set_needle_count(obj, 1, needle_color);
    lv_gauge_set_value(obj, 0, 80);
    lv_group_add_obj(g, obj);

    obj = lv_chart_create(win, NULL);
    lv_chart_series_t * ser = lv_chart_add_series(obj, LV_COLOR_RED);
    lv_chart_set_next(obj, ser, 40);
    lv_chart_set_next(obj, ser, 30);
    lv_chart_set_next(obj, ser, 35);
    lv_chart_set_next(obj, ser, 50);
    lv_chart_set_next(obj, ser, 60);
    lv_chart_set_next(obj, ser, 75);
    lv_chart_set_next(obj, ser, 80);
    lv_group_add_obj(g, obj);

    obj = lv_led_create(win, NULL);
    lv_group_add_obj(g, obj);

    obj = lv_tabview_create(win, NULL);
    lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
    lv_tabview_add_tab(obj, "Tab 1");
    lv_tabview_add_tab(obj, "Tab 2");
    lv_group_add_obj(g, obj);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool win_btn_get(lv_indev_data_t *data)
{
    if(last_key) {
        data->state = LV_INDEV_STATE_PR;
        data->key = last_key;
        last_key = 0;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    return false;
}


static lv_res_t win_btn_acion(lv_obj_t *btn)
{
    LV_OBJ_FREE_NUM_TYPE c = lv_obj_get_free_num(btn);

    lv_win_focus(win, lv_group_get_focused(g), 200);

    last_key = c;


    return LV_RES_OK;
}
