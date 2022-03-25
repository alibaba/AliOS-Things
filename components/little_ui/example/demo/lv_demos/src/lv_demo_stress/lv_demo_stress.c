/**
 * @file lv_demo_stress.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demo.h"
#include "esp_log.h"
#define TAG "stress"

#if LV_USE_DEMO_STRESS
/*********************
 *      DEFINES
 *********************/
#define TIME_STEP   50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void auto_del(lv_obj_t * obj, uint32_t delay);
static void msgbox_del(lv_timer_t * tmr);
static void set_y_anim(void * obj, int32_t v);
static void set_width_anim(void * obj, int32_t v);
static void arc_set_end_angle_anim(void * obj, int32_t v);
static void obj_test_task_cb(lv_timer_t * tmr);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * main_page;
static lv_obj_t * ta;
static const char * mbox_btns[] = {"Ok", "Cancel", ""};
static uint32_t mem_free_start = 0;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_stress(void)
{
    //lv_timer_create(obj_test_task_cb, TIME_STEP, NULL);
	lv_anim_t a;
	lv_obj_t * obj;
#if 0
	lv_res_t res = lv_mem_test();
	if(res != LV_RES_OK) {
		LV_LOG_ERROR("Memory integrity error");
	}

	lv_mem_monitor_t mon;
	lv_mem_monitor(&mon);

	if(mem_free_start == 0)  mem_free_start = mon.free_size;
	LV_LOG_USER("mem leak since start: %d, frag: %3d %%",  mem_free_start - mon.free_size, mon.frag_pct);
#endif
	/* Holder for all object types */
	main_page = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_page, LV_HOR_RES / 2 , LV_VER_RES);
	lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN);


	obj = lv_btn_create(main_page);
	lv_obj_set_size(obj, 100, 70);
	obj = lv_label_create(obj);
	//lv_label_set_text(obj, "Multi line\n"LV_SYMBOL_OK LV_SYMBOL_CLOSE LV_SYMBOL_WIFI);
	lv_label_set_text(obj, "zilintest\n");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void obj_test_task_cb(lv_timer_t * tmr)
{
    (void) tmr;    /*Unused*/
    static int16_t state = -1;

    lv_anim_t a;
    lv_obj_t * obj;

    //printf("step start: %d\n", state);
    ESP_LOGE(TAG, "step start: %d\n", state);

    switch(state) {
        case -1:
        {
            lv_res_t res = lv_mem_test();
            if(res != LV_RES_OK) {
                LV_LOG_ERROR("Memory integrity error");
            }

            lv_mem_monitor_t mon;
            lv_mem_monitor(&mon);

            if(mem_free_start == 0)  mem_free_start = mon.free_size;

            LV_LOG_USER("mem leak since start: %d, frag: %3d %%",  mem_free_start - mon.free_size, mon.frag_pct);
        }
            break;
        case 0:
            /* Holder for all object types */
            main_page = lv_obj_create(lv_scr_act());
            lv_obj_set_size(main_page, LV_HOR_RES / 2 , LV_VER_RES);
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN);


            obj = lv_btn_create(main_page);
            lv_obj_set_size(obj, 100, 70);
            obj = lv_label_create(obj);
            lv_label_set_text(obj, "Multi line\n"LV_SYMBOL_OK LV_SYMBOL_CLOSE LV_SYMBOL_WIFI);
            break;
#if 0
        case 1:
              {
                  obj = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
                  lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
                  lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
                  lv_obj_t * t = lv_tabview_add_tab(obj, "First");

                  t = lv_tabview_add_tab(obj, "Second");
                  lv_obj_t * c = lv_colorwheel_create(t, true);
                  lv_obj_set_size(c,  150, 150);
                  //                  c = lv_led_create(t, NULL);
                  //                  lv_obj_set_pos(c, 160, 20);
                  t = lv_tabview_add_tab(obj, LV_SYMBOL_EDIT " Edit");
                  t = lv_tabview_add_tab(obj, LV_SYMBOL_CLOSE);

                  lv_tabview_set_act(obj, 1, LV_ANIM_ON);
                  auto_del(obj, TIME_STEP * 5 + 30);
              }
              break;

        case 2:
            obj = lv_btn_create(main_page);
            lv_obj_set_size(obj, 200, 70);

            /*Move to disabled state very slowly*/
            lv_obj_add_state(obj, LV_STATE_DISABLED);

            /*Add an infinite width change animation*/
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_time(&a, TIME_STEP * 2);
            lv_anim_set_exec_cb(&a, set_width_anim);
            lv_anim_set_values(&a, 100, 200);
            lv_anim_set_playback_time(&a, TIME_STEP * 2);
            lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
            lv_anim_start(&a);

            /*Delete the object a few sec later*/
            auto_del(obj, TIME_STEP * 10);

            obj = lv_label_create(obj);
            lv_label_set_text_fmt(obj, "Formatted:\n%d %s", 12, "Volt");
            break;

        case 3:
            ta = lv_textarea_create(lv_scr_act());
            lv_obj_align_to(ta, main_page, LV_ALIGN_OUT_RIGHT_TOP, 10, 10);
            lv_obj_set_size(ta, LV_HOR_RES / 3, LV_VER_RES / 4);
            lv_textarea_set_placeholder_text(ta, "The placeholder");
            break;

        case 4:
            obj = lv_btn_create(main_page);
            lv_obj_set_size(obj, 100, 70);
            lv_obj_set_style_bg_img_src(obj, LV_SYMBOL_DUMMY"Text from\nstyle", 0);
            lv_obj_del_async(obj);  /*Delete on next call of `lv_task_handler` (so not now)*/

//            obj = lv_btn_create(main_page);
//            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
//            lv_obj_set_style_bg_img_src(obj, LV_SYMBOL_LEFT);
//            lv_obj_set_style_bg_img_opa(obj, LV_OPA_50);
//            lv_obj_set_style_bg_img_tiled(obj, true);
//            lv_obj_scroll_to_view(obj, LV_ANIM_ON);
            break;

        case 5:
            lv_textarea_set_one_line(ta, true);
            break;
        case 6:
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN_WRAP);
            break;

        case 7:
            obj = lv_bar_create(main_page);
            lv_bar_set_range(obj, -1000, 2000);
            lv_bar_set_value(obj, 1800, LV_ANIM_ON);
            lv_bar_set_start_value(obj, -500, LV_ANIM_ON);

            auto_del(obj, TIME_STEP * 2 + 70);

            obj = lv_slider_create(main_page);
            lv_obj_set_style_anim_time(obj, TIME_STEP * 8, 0);
            lv_slider_set_value(obj, 5000, LV_ANIM_ON);    /*Animate to out of range value*/
            auto_del(obj, TIME_STEP * 5 + 22);

            obj = lv_switch_create(main_page);

            obj = lv_switch_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED);
            auto_del(obj, 730);
            break;

        case 8:
              obj = lv_win_create(lv_scr_act(), 50);
              lv_obj_set_size(obj, LV_HOR_RES / 2, LV_VER_RES / 2);
              lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
              lv_win_add_title(obj, "Window title");
              lv_win_add_btn(obj, LV_SYMBOL_CLOSE, 40);
              lv_win_add_btn(obj, LV_SYMBOL_DOWN, 40);
              auto_del(obj, TIME_STEP * 3 + 5);

              obj = lv_calendar_create(lv_win_get_content(obj));
              break;
        case 9:
            lv_textarea_set_text(ta, "A very very long text which will should make the text area scrollable"
                    "Here area some dummy sentences to be sure the text area will be really scrollable.");
            break;
        case 10:
            obj = lv_keyboard_create(lv_scr_act());
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_TEXT_UPPER);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, LV_VER_RES, LV_VER_RES - lv_obj_get_height(obj));
            lv_anim_set_time(&a, TIME_STEP + 3);
            lv_anim_set_exec_cb(&a, set_y_anim);
            lv_anim_start(&a);

            auto_del(obj, TIME_STEP * 2 + 18);
            break;

        case 11:
            obj = lv_dropdown_create(main_page);
            lv_dropdown_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight");
            lv_dropdown_open(obj);
            lv_dropdown_set_selected(obj, 2);
            auto_del(obj, TIME_STEP * 3 + 11);
            break;

        case 12:
            obj = lv_roller_create(main_page);
            lv_roller_set_options(obj, "Zero\nOne\nTwo\nThree\nFour\nFive\nSix\nSeven\nEight", LV_ROLLER_MODE_INFINITE);
            lv_roller_set_selected(obj, 2, LV_ANIM_ON);
            auto_del(obj, TIME_STEP * 20 + 22);
            break;

        case 13:
            obj = lv_arc_create(main_page);
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, 180, 400);
            lv_anim_set_time(&a, TIME_STEP * 2);
            lv_anim_set_delay(&a, TIME_STEP + 25);
            lv_anim_set_playback_time(&a, TIME_STEP * 5);
            lv_anim_set_repeat_count(&a, 3);
            lv_anim_set_exec_cb(&a, arc_set_end_angle_anim);
            lv_anim_start(&a);
            break;

            obj = lv_meter_create(main_page);
            lv_obj_scroll_to_view(obj, LV_ANIM_ON);
            lv_meter_scale_t * scale = lv_meter_add_scale(obj);
            lv_meter_add_needle_line(obj, scale, 3, lv_palette_main(LV_PALETTE_RED), -10);

            auto_del(obj, TIME_STEP * 6 + 30);
            break;

        case 14:
            obj = lv_msgbox_create(NULL, "Title", "Some text on the message box with average length", mbox_btns, true);

            lv_timer_t * msgbox_tmr = lv_timer_create(msgbox_del, TIME_STEP * 5 + 30, obj);
            lv_timer_set_repeat_count(msgbox_tmr, 1);
            lv_obj_align(obj, LV_ALIGN_RIGHT_MID, -10, 0);
            break;

        case 15:
            lv_textarea_set_one_line(ta, false);
            break;

        case 16:
        {
              lv_obj_t * tv = lv_tileview_create(lv_scr_act());
              lv_obj_set_size(tv, 200, 200);
              auto_del(tv, TIME_STEP * 4 + 5);

              obj = lv_tileview_add_tile(tv, 0, 0, LV_DIR_ALL);
              obj = lv_label_create(obj);
              lv_label_set_text(obj, "Tile: 0;0");

              obj = lv_tileview_add_tile(tv, 0, 1, LV_DIR_ALL);
              obj = lv_label_create(obj);
              lv_label_set_text(obj, "Tile: 0;1");

              obj = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);
              obj = lv_label_create(obj);
              lv_label_set_text(obj, "Tile: 1;1");

              lv_obj_set_tile_id(tv, 1, 1, LV_ANIM_ON);
        }
        break;

        case 18:
            obj =  lv_list_create(main_page);
            {
                lv_obj_t * b;
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "1. Some very long text to scroll");
                auto_del(b, 10);
                lv_list_add_btn(obj, LV_SYMBOL_OK, "2. Some very long text to scroll");
                lv_list_add_btn(obj, LV_SYMBOL_OK, "3. Some very long text to scroll");
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "4. Some very long text to scroll");
                auto_del(b, TIME_STEP);
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "5. Some very long text to scroll");
                auto_del(b, TIME_STEP + 90);
                b = lv_list_add_btn(obj, LV_SYMBOL_OK, "6. Some very long text to scroll");
                auto_del(b, TIME_STEP + 10);
                lv_obj_scroll_to_view(lv_obj_get_child(obj, -1),  LV_ANIM_ON);
            }
            auto_del(obj, TIME_STEP * 5 + 15);

            break;

        case 19:
            lv_textarea_set_cursor_pos(ta, 10);
            lv_textarea_set_text(ta, "__INSERTED TEXT" LV_SYMBOL_EDIT"__");
            break;

        case 20:
            lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_ROW_WRAP);
            break;

        case 21:
            lv_textarea_set_cursor_pos(ta, 0);
            lv_textarea_add_char(ta, '0');
            lv_textarea_add_char(ta, '1');
            lv_textarea_add_char(ta, '2');
            lv_textarea_add_char(ta, '3');
            lv_textarea_add_char(ta, '4');
            lv_textarea_add_char(ta, '5');
            lv_textarea_add_char(ta, '6');
            lv_textarea_add_char(ta, '7');
            lv_textarea_add_char(ta, '8');
            lv_textarea_add_char(ta, '9');
            lv_textarea_add_char(ta, 'A');
            lv_textarea_add_char(ta, 'B');
            lv_textarea_add_char(ta, 'C');
            lv_textarea_add_char(ta, 'D');
            lv_textarea_add_char(ta, 'E');
            lv_textarea_add_char(ta, 'F');
            lv_textarea_add_text(ta, LV_SYMBOL_OK);
            lv_textarea_add_text(ta, LV_SYMBOL_CLOSE);
            lv_textarea_add_text(ta, LV_SYMBOL_COPY);
            lv_textarea_add_text(ta, LV_SYMBOL_SAVE);
            lv_textarea_add_text(ta, LV_SYMBOL_PASTE);
            break;

        case 22:
            obj = lv_spinbox_create(main_page);
            lv_spinbox_set_digit_format(obj, 6, 3);
            lv_spinbox_set_value(obj, 5678);
            lv_spinbox_set_step(obj, 10);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 100);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_increment(obj);
            lv_spinbox_increment(obj);
            auto_del(obj, TIME_STEP * 2 + 15);

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);

            break;

        case 23:
            obj = lv_chart_create(main_page);
            {
                lv_chart_series_t * s1 = lv_chart_add_series(obj, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
                lv_chart_set_next_value(obj, s1, 36);
                lv_chart_set_next_value(obj, s1, -29);
                lv_chart_set_next_value(obj, s1, 51);
                lv_chart_set_next_value(obj, s1, 107);
                lv_chart_set_next_value(obj, s1, 70);
                lv_chart_set_next_value(obj, s1, 36);
                lv_chart_set_next_value(obj, s1, -2);
                lv_chart_set_next_value(obj, s1, 63);
                lv_chart_set_next_value(obj, s1, 48);
                lv_chart_set_next_value(obj, s1, 72);

                auto_del(obj, TIME_STEP * 3);
            }

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);
            break;

        case 24:
            obj = lv_checkbox_create(main_page);
            lv_checkbox_set_text(obj, "An option to select");
            auto_del(obj, TIME_STEP * 2 + 20);

            obj = lv_checkbox_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED);

            obj = lv_checkbox_create(main_page);
            lv_obj_add_state(obj, LV_STATE_CHECKED | LV_STATE_DISABLED);
            auto_del(obj, TIME_STEP * 1 + 60);

            lv_obj_scroll_by(main_page, 0, 20, LV_ANIM_ON);

            break;

        case 25:
            lv_textarea_set_cursor_pos(ta, 20);
            {
                uint16_t i;
                for(i = 0; i < 64; i++) {
                    lv_textarea_del_char_forward(ta);
                }
            }
            break;

        case 26:
            lv_textarea_set_one_line(ta, true);
            break;
        case 29:
            lv_obj_clean(main_page);
            lv_obj_del(ta);
            ta = NULL;
            break;
        case 31:
            lv_obj_clean(lv_scr_act());
            main_page = NULL;
            state = -2;
            break;
#endif
        default:
            break;
    }

    //printf("step end: %d\n", state);
    ESP_LOGE(TAG, "step end: %d\n", state);
    state ++;
}

static void auto_del(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

}

static void msgbox_del(lv_timer_t * tmr)
{
    lv_msgbox_close(tmr->user_data);
}

static void set_y_anim(void * obj, int32_t v)
{
    lv_obj_set_y(obj, v);
}

static void set_width_anim(void * obj, int32_t v)
{
    lv_obj_set_width(obj, v);
}

static void arc_set_end_angle_anim(void * obj, int32_t v)
{
    lv_arc_set_end_angle(obj, v);
}

#endif /* LV_USE_DEMO_STRESS */
