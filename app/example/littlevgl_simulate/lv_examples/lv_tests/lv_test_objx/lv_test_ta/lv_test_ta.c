/**
 * @file lv_test_ta.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_ta.h"

#if USE_LV_TA && USE_LV_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t cursor_move(lv_obj_t *btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *ta2_1;
static lv_obj_t *ta2_2;
static lv_obj_t *ta2_3;
static lv_obj_t *ta2_4;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create text areas to test their basic functionalities
 */
void lv_test_ta_1(void)
{
    /* Create a text area which looks well */
    lv_obj_t *ta1 = lv_ta_create(lv_scr_act(), NULL);

    /*A text area with the following text:
     * Next long test text and testing the automatic       (line break here)
     * and manual line break feature too.
     *(The cursor should be at the end of text)
     *
     *
     * If UTF-8 is enabled these characters are added at the beginning:
     * "űŰöÖ "
     * The cursor should be after these letters*/

    lv_obj_t *ta2 = lv_ta_create(lv_scr_act(), NULL);
    lv_ta_set_text(ta2, "New text");
    lv_ta_set_cursor_pos(ta2, 4);
    lv_ta_add_text(ta2, "test ");
    lv_ta_set_cursor_pos(ta2, 4);
    lv_ta_add_char(ta2, 'l');
    lv_ta_add_char(ta2, 'o');
    lv_ta_add_char(ta2, 'n');
    lv_ta_add_char(ta2, 'g');
    lv_ta_add_char(ta2, ' ');
    lv_ta_set_cursor_pos(ta2, LV_TA_CURSOR_LAST);
    lv_ta_add_text(ta2, " and testing the automatic\n"
                        "and manual line break feature too.");

#if LV_TXT_UTF8 != 0
    lv_ta_set_cursor_pos(ta2, 0);
    lv_ta_add_text(ta2, "ű");
    lv_ta_add_text(ta2, "Ű");
    lv_ta_add_text(ta2, "ö");
    lv_ta_add_text(ta2, "Ő");
    lv_ta_add_char(ta2, ' ');
#endif

    lv_obj_align(ta2, ta1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Test password mode and one line*/
    lv_obj_t *ta3 = lv_ta_create(lv_scr_act(), NULL);
    lv_ta_set_pwd_mode(ta3, true);
    lv_ta_set_one_line(ta3, true);

    lv_obj_align(ta3, ta2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_ta_set_text(ta3, "a A");
    lv_ta_set_cursor_pos(ta3, 1);
    lv_ta_add_char(ta3, 'b');
#if LV_TXT_UTF8 != 0
    lv_ta_add_text(ta3, "é");
    lv_ta_set_cursor_pos(ta3, 5);
#else
    lv_ta_set_cursor_pos(ta3, 4);
#endif

    lv_ta_add_char(ta3, 'B');
    lv_ta_add_text(ta3, "É");

    /*Get the password text and set in a new text area*/
    lv_obj_t *ta4 = lv_ta_create(lv_scr_act(), NULL);
    lv_ta_set_one_line(ta4, true);
    lv_ta_set_text(ta4, lv_ta_get_text(ta3));
    lv_obj_align(ta4, ta3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**
 * Test cursor modes
 */
void lv_test_ta_2(void)
{

    static lv_style_t bg;
    static lv_style_t sb;
    static lv_style_t cur;

    lv_style_copy(&bg, &lv_style_pretty);
    lv_style_copy(&sb, &lv_style_pretty);
    lv_style_copy(&cur, &lv_style_pretty);


    bg.body.main_color = LV_COLOR_BLACK;
    bg.body.padding.hor = 10;
    bg.body.padding.ver = 10;
    bg.text.color = LV_COLOR_BLUE;
    bg.text.letter_space = 4;
    bg.text.line_space = 10;

    sb.body.padding.hor = 3;
    sb.body.padding.inner = 10;
    sb.body.main_color = LV_COLOR_WHITE;
    sb.body.grad_color = LV_COLOR_WHITE;
    sb.body.opa = LV_OPA_70;

    cur.body.padding.hor = 2;
    cur.body.padding.ver = 4;
    cur.body.main_color = LV_COLOR_RED;
    cur.body.grad_color = LV_COLOR_YELLOW;
    cur.body.border.color = LV_COLOR_ORANGE;
    cur.body.opa = LV_OPA_70;
    cur.text.color = LV_COLOR_WHITE;
    cur.line.width = 4;


    ta2_1 = lv_ta_create(lv_scr_act(), NULL);
    lv_ta_set_style(ta2_1, LV_TA_STYLE_BG, &bg);
    lv_ta_set_style(ta2_1, LV_TA_STYLE_SB, &sb);
    lv_ta_set_style(ta2_1, LV_TA_STYLE_CURSOR, &cur);
    lv_ta_set_cursor_type(ta2_1, LV_CURSOR_LINE);
#if LV_TXT_UTF8 != 0
    lv_ta_set_text(ta2_1, "Some UTF-8 characters "
                          "űőabcŰŐABC\n"
                          "\n"
                          "Í\n"
                          "W\n"
                          "abc");

#endif
    ta2_2 = lv_ta_create(lv_scr_act(), ta2_1);
    lv_obj_align(ta2_2, ta2_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_ta_set_cursor_type(ta2_2, LV_CURSOR_BLOCK);

    ta2_3 = lv_ta_create(lv_scr_act(), ta2_1);
    lv_obj_align(ta2_3, ta2_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_ta_set_cursor_type(ta2_3, LV_CURSOR_OUTLINE);

    ta2_4 = lv_ta_create(lv_scr_act(), ta2_1);
    lv_obj_align(ta2_4, ta2_3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_ta_set_cursor_type(ta2_4, LV_CURSOR_UNDERLINE);

    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn, ta2_1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_free_num(btn, 0);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, cursor_move);
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Up");

    btn = lv_btn_create(lv_scr_act(), btn);
    lv_obj_align(btn, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_free_num(btn, 1);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Down");

    btn = lv_btn_create(lv_scr_act(), btn);
    lv_obj_align(btn, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_free_num(btn, 2);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Left");

    btn = lv_btn_create(lv_scr_act(), btn);
    lv_obj_align(btn, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_free_num(btn, 3);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Right");

    btn = lv_btn_create(lv_scr_act(), btn);
    lv_obj_align(btn, btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_free_num(btn, 4);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Del");



}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t cursor_move(lv_obj_t *btn)
{
    switch(lv_obj_get_free_num(btn)) {
        case 0:
            lv_ta_cursor_up(ta2_1);
            lv_ta_cursor_up(ta2_2);
            lv_ta_cursor_up(ta2_3);
            lv_ta_cursor_up(ta2_4);
            break;

        case 1:
            lv_ta_cursor_down(ta2_1);
            lv_ta_cursor_down(ta2_2);
            lv_ta_cursor_down(ta2_3);
            lv_ta_cursor_down(ta2_4);
            break;

        case 2:
            lv_ta_cursor_left(ta2_1);
            lv_ta_cursor_left(ta2_2);
            lv_ta_cursor_left(ta2_3);
            lv_ta_cursor_left(ta2_4);
            break;

        case 3:
            lv_ta_cursor_right(ta2_1);
            lv_ta_cursor_right(ta2_2);
            lv_ta_cursor_right(ta2_3);
            lv_ta_cursor_right(ta2_4);
            break;

        case 4:
            lv_ta_del_char(ta2_1);
            lv_ta_del_char(ta2_2);
            lv_ta_del_char(ta2_3);
            lv_ta_del_char(ta2_4);
            break;
    }

    return LV_RES_OK;

}
#endif /*USE_LV_TA && USE_LV_TESTS*/
