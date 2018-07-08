/**
 * @file lv_test_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_list.h"

#if USE_LV_LIST && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t list_move(lv_obj_t *btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *list1;
static lv_obj_t *list2;
static lv_obj_t *list3;
static lv_obj_t *list4;

extern const lv_img_t img_flower_icon;        /*Comes from lv_test_img*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create lists to test their functionalities
 */
void lv_test_list_1(void)
{
    /* Default object. It will be an empty list*/
    list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_pos(list1, 10, 10);

    list2 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_align(list2, list1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_list_add(list2, SYMBOL_FILE, "File", NULL);
    lv_list_add(list2, SYMBOL_DIRECTORY, "Directory", NULL);
    lv_list_add(list2, &img_flower_icon, "Image icon", NULL);
    lv_obj_set_width(list2, 100);

    list3 = lv_list_create(lv_scr_act(), list2);
    lv_obj_align(list3, list2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_list_add(list3, NULL, "No icon", NULL);
    lv_list_add(list3, SYMBOL_CLOSE, "", NULL);
    lv_list_add(list3, SYMBOL_UP, "Up", NULL);
    lv_list_add(list3, SYMBOL_DOWN, "Down", NULL);

    static lv_style_t sb;
    static lv_style_t bg;
    lv_style_copy(&sb, &lv_style_pretty_color);
    lv_style_copy(&bg, &lv_style_pretty_color);
    sb.body.padding.hor = -10;
    sb.body.padding.inner = 10;

    bg.body.padding.hor = 20;

    list4 = lv_list_create(lv_scr_act(), list3);
    lv_list_set_style(list4, LV_LIST_STYLE_BG, &bg);
    lv_list_set_style(list4, LV_LIST_STYLE_SB, &sb);
    lv_obj_align(list4, list3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_width(list4, 200);

    /*Add list up/down buttons*/
    lv_obj_t *btn_up = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn_up, list1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_btn_set_action(btn_up, LV_BTN_ACTION_CLICK, list_move);
    lv_obj_set_free_num(btn_up, 0);
    lv_obj_t *label = lv_label_create(btn_up, NULL);
    lv_label_set_text(label, SYMBOL_UP);

    lv_obj_t *btn_down = lv_btn_create(lv_scr_act(), btn_up);
    lv_obj_align(btn_down, btn_up, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_free_num(btn_down, 1);
    label = lv_label_create(btn_down, NULL);
    lv_label_set_text(label, SYMBOL_DOWN);

}



/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t list_move(lv_obj_t *btn)
{
    if(lv_obj_get_free_num(btn) == 0) {
        lv_list_up(list1);
        lv_list_up(list2);
        lv_list_up(list3);
        lv_list_up(list4);
    } else {
        lv_list_down(list1);
        lv_list_down(list2);
        lv_list_down(list3);
        lv_list_down(list4);
    }
    return LV_RES_OK;
}

#endif /*USE_LV_LIST && USE_LV_TESTS*/
