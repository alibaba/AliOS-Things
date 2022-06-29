#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

void test_dropdown_create_delete(void);
void test_dropdown_set_options(void);
void test_dropdown_select(void);
void test_dropdown_click(void);
void test_dropdown_keypad(void);
void test_dropdown_encoder(void);
void test_dropdown_render_1(void);
void test_dropdown_render_2(void);

void test_dropdown_create_delete(void)
{
  lv_dropdown_create(lv_scr_act());
  TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(lv_scr_act()));
  
  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 200, 0);
  TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(lv_scr_act()));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  lv_dropdown_open(dd2);
  TEST_ASSERT_EQUAL(3, lv_obj_get_child_cnt(lv_scr_act()));
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd2));
  lv_dropdown_open(dd2);    /*Try to pen again*/
  TEST_ASSERT_EQUAL(3, lv_obj_get_child_cnt(lv_scr_act()));

  lv_obj_t * dd3 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd3, 400, 0);
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_open(dd3);
  TEST_ASSERT_EQUAL(5, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_close(dd3);
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_close(dd3);   /*Try to close again*/
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  
  lv_obj_del(dd2);
  TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(lv_scr_act()));
 
  lv_obj_clean(lv_scr_act());
  TEST_ASSERT_EQUAL(0, lv_obj_get_child_cnt(lv_scr_act()));

}

void test_dropdown_set_options(void)
{

  lv_mem_monitor_t m1;
  lv_mem_monitor(&m1);

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  TEST_ASSERT_EQUAL_STRING("Option 1\nOption 2\nOption 3", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(3, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_set_options(dd1, "a1\nb2\nc3\nd4\ne5\nf6");
  TEST_ASSERT_EQUAL_STRING("a1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(6, lv_dropdown_get_option_cnt(dd1));
  
  lv_obj_set_width(dd1, 200);
  lv_dropdown_open(dd1);
  lv_obj_update_layout(dd1);
  TEST_ASSERT_EQUAL(200, lv_obj_get_width(lv_dropdown_get_list(dd1)));
  
  lv_dropdown_close(dd1);

  lv_dropdown_add_option(dd1, "x0", 0);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(7, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "y0", 3);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(8, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "z0", LV_DROPDOWN_POS_LAST);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6\nz0", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(9, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_clear_options(dd1);
  TEST_ASSERT_EQUAL_STRING("", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_set_options(dd1, "o1\no2"); /*Just to add some content before lv_dropdown_set_options_static*/

  lv_dropdown_set_options_static(dd1, "a1\nb2\nc3\nd4\ne5\nf6");
  TEST_ASSERT_EQUAL_STRING("a1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(6, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_add_option(dd1, "x0", 0);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(7, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "y0", 3);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(8, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "z0", LV_DROPDOWN_POS_LAST);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6\nz0", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(9, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_clear_options(dd1);
  TEST_ASSERT_EQUAL_STRING("", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_option_cnt(dd1));

  lv_obj_del(dd1);

  lv_mem_monitor_t m2;
  lv_mem_monitor(&m2);
  TEST_ASSERT_UINT32_WITHIN(48, m1.free_size, m2.free_size);
}

void test_dropdown_select(void)
{
  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_selected(dd1, 2);
  
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
  
  char buf[32];
  memset(buf, 0x00, sizeof(buf));
  lv_dropdown_get_selected_str(dd1, buf, sizeof(buf));  
  TEST_ASSERT_EQUAL_STRING("Option 3", buf);
  
  memset(buf, 0x00, sizeof(buf));
  lv_dropdown_get_selected_str(dd1, buf, 4);
  TEST_ASSERT_EQUAL_STRING("Opt", buf);

  /*Out of range*/
  lv_dropdown_set_selected(dd1, 3);
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
}

void test_dropdown_click(void)
{
  lv_obj_clean(lv_scr_act());
  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_obj_update_layout(dd1);
  
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  lv_test_mouse_click_at(dd1->coords.x1 + 5, dd1->coords.y1 + 5); 
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
    
  lv_obj_t * list = lv_dropdown_get_list(dd1);
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_selected(dd1));
  lv_test_mouse_click_at(list->coords.x1 + 5, list->coords.y2 - 25);
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
}

static uint32_t event_cnt;
static void dd_event(lv_event_t * e) 
{
  LV_UNUSED(e);
  event_cnt++;
}

void test_dropdown_keypad(void)
{
  lv_obj_clean(lv_scr_act());

  lv_group_t * g = lv_group_create();
  lv_indev_set_group(lv_test_keypad_indev, g);

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd1, 20, 20);
  lv_dropdown_set_options(dd1, "1\n2\n3\n4\n5\n6\n7\n8");
  lv_group_add_obj(g, dd1);
  lv_obj_add_event_cb(dd1, dd_event, LV_EVENT_VALUE_CHANGED, NULL);
  
  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 300, 20);
  lv_group_add_obj(g, dd2);
    
  event_cnt = 0;  
    
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_RIGHT);  /*Same as down*/
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(1, event_cnt);
  
  lv_test_key_hit(LV_KEY_DOWN); /*Open the list too*/
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(3, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(2, event_cnt);
  
  lv_test_key_hit(LV_KEY_RIGHT); /*Open the list too*/
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  lv_test_key_hit(LV_KEY_RIGHT);
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(4, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(3, event_cnt);
  
  lv_test_key_hit(LV_KEY_LEFT); /*Open the list too*/
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  lv_test_key_hit(LV_KEY_LEFT);
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(3, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(4, event_cnt);
  
  lv_test_key_hit(LV_KEY_UP); /*Open the list too*/
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  lv_test_key_hit(LV_KEY_UP); 
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(5, event_cnt);
  
  lv_test_key_hit(LV_KEY_UP); 
  lv_test_key_hit(LV_KEY_UP); 
  lv_test_key_hit(LV_KEY_UP); 
  lv_test_key_hit(LV_KEY_UP); 
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(6, event_cnt);
  
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN); 
  lv_test_key_hit(LV_KEY_DOWN);
  lv_test_key_hit(LV_KEY_DOWN); 
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(7, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(7, event_cnt);
  
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  
  lv_test_key_hit(LV_KEY_NEXT); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  
  lv_test_key_hit(LV_KEY_ENTER); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd2));
  
  lv_indev_set_group(lv_test_keypad_indev, NULL);
  lv_group_del(g);
}


void test_dropdown_encoder(void)
{
  lv_obj_clean(lv_scr_act());

  lv_group_t * g = lv_group_create();
  lv_indev_set_group(lv_test_encoder_indev, g);

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd1, 20, 20);
  lv_dropdown_set_options(dd1, "1\n2\n3\n4\n5\n6\n7\n8");
  lv_group_add_obj(g, dd1);
  lv_obj_add_event_cb(dd1, dd_event, LV_EVENT_VALUE_CHANGED, NULL);
  
  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 300, 20);
  lv_group_add_obj(g, dd2);
    
  event_cnt = 0;  
    
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  lv_test_encoder_click(); 
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  
  lv_test_encoder_turn(5);
  lv_test_encoder_click(); 
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(5, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(1, event_cnt);
  
  lv_test_encoder_click(); 
  lv_test_encoder_turn(-1);
  lv_test_encoder_click(); 
  TEST_ASSERT_EQUAL(4, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(2, event_cnt);

  lv_test_encoder_click(); 
  lv_test_encoder_turn(2);
  lv_test_encoder_press(); 
  lv_test_indev_wait(1000);  //Long press
  lv_test_encoder_release(); 
  lv_test_indev_wait(50);
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_EQUAL(4, lv_dropdown_get_selected(dd1));
  TEST_ASSERT_EQUAL(2, event_cnt);
  
  lv_test_encoder_turn(1);
  lv_test_encoder_click();
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd1));
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd2));
  
  lv_indev_set_group(lv_test_encoder_indev, NULL);
  lv_group_del(g);
}


void test_dropdown_render_1(void)
{
  lv_obj_clean(lv_scr_act());

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd1, 10, 10);
  lv_dropdown_set_selected(dd1, 1);

  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 200, 10);
  lv_obj_set_width(dd2, 200);
  lv_dropdown_set_selected(dd2, 2);
  lv_dropdown_open(dd2);
  TEST_ASSERT_TRUE(lv_dropdown_get_selected_highlight(dd2));
  lv_dropdown_set_selected_highlight(dd2, false);
  TEST_ASSERT_FALSE(lv_dropdown_get_selected_highlight(dd2)); 

  lv_obj_t * dd3 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_style_pad_hor(dd3, 5, 0);
  lv_obj_set_style_pad_ver(dd3, 20, 0);
  lv_obj_set_pos(dd3, 500, 150);
  TEST_ASSERT_EQUAL_PTR(NULL, lv_dropdown_get_text(dd3));
  lv_dropdown_set_text(dd3, "A text");
  TEST_ASSERT_EQUAL_STRING("A text", lv_dropdown_get_text(dd3));
  
  lv_dropdown_set_selected(dd3, 2);
  
  TEST_ASSERT_EQUAL(LV_DIR_BOTTOM, lv_dropdown_get_dir(dd3));
  lv_dropdown_set_dir(dd3, LV_DIR_LEFT);
  TEST_ASSERT_EQUAL(LV_DIR_LEFT, lv_dropdown_get_dir(dd3));
  
  TEST_ASSERT_EQUAL_STRING(LV_SYMBOL_DOWN, lv_dropdown_get_symbol(dd3));
  lv_dropdown_set_symbol(dd3, LV_SYMBOL_LEFT);
  TEST_ASSERT_EQUAL_STRING(LV_SYMBOL_LEFT, lv_dropdown_get_symbol(dd3));

  lv_dropdown_set_options(dd3, "a0\na1\na2\na3\na4\na5\na6\na7\na8\na9\na10\na11\na12\na13\na14\na15\na16");
  lv_dropdown_open(dd3);
  lv_dropdown_set_selected(dd3, 3);
  lv_obj_t * list = lv_dropdown_get_list(dd3);
  lv_obj_set_style_text_line_space(list, 5, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex3(0xf00), LV_PART_SELECTED | LV_STATE_CHECKED);

  
  TEST_ASSERT_EQUAL_SCREENSHOT("dropdown_1.png");
}

void test_dropdown_render_2(void)
{
  lv_obj_clean(lv_scr_act());
  LV_IMG_DECLARE(img_caret_down);
  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd1, "Short");
  lv_dropdown_set_options(dd1, "1\n2");
  lv_dropdown_set_symbol(dd1, &img_caret_down);
  lv_dropdown_open(dd1);

  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd2, "Go Up");
  lv_dropdown_set_options(dd2, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15");
  lv_dropdown_set_symbol(dd2, NULL);
  lv_obj_align(dd2, LV_ALIGN_LEFT_MID, 150, 50);
  lv_dropdown_open(dd2);

  lv_obj_t * dd3 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd3, "Limit Down");
  lv_dropdown_set_options(dd3, "1aaaaaaaaaaaaaaaa\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15");
  lv_obj_align(dd3, LV_ALIGN_LEFT_MID, 300, -10);
  lv_dropdown_open(dd3);

  lv_obj_t * dd4 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd4, "Limit Top");
  lv_dropdown_set_options(dd4, "1aaaaaaaaaaaaaaaa\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15");
  lv_obj_align(dd4, LV_ALIGN_LEFT_MID, 450, 10);
  lv_dropdown_set_dir(dd4, LV_DIR_TOP);
  lv_dropdown_set_symbol(dd4, LV_SYMBOL_UP);
  lv_dropdown_open(dd4);

  lv_obj_t * dd5 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd5, "Go Down");
  lv_dropdown_set_options(dd5, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15");
  lv_dropdown_set_dir(dd5, LV_DIR_TOP);
  lv_dropdown_set_symbol(dd5, LV_SYMBOL_UP);
  lv_obj_align(dd5, LV_ALIGN_LEFT_MID, 650, -200);
  lv_dropdown_open(dd5);

  lv_obj_t * dd6 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd6, "Right");
  lv_dropdown_set_options(dd6, "1aaa\n2aa\n3aa");
  lv_dropdown_set_dir(dd6, LV_DIR_RIGHT);
  lv_dropdown_set_symbol(dd6, LV_SYMBOL_RIGHT);
  lv_obj_align(dd6, LV_ALIGN_BOTTOM_LEFT, 20, -20);
  lv_dropdown_open(dd6);
  lv_obj_set_style_text_align(lv_dropdown_get_list(dd6), LV_TEXT_ALIGN_RIGHT, 0);

  lv_obj_t * dd7 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_text(dd7, "Left");
  lv_dropdown_set_options(dd7, "1aaa\n2\n3");
  lv_dropdown_set_dir(dd7, LV_DIR_LEFT);
  lv_dropdown_set_symbol(dd7, LV_SYMBOL_LEFT);
  lv_obj_align(dd7, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
  lv_dropdown_open(dd7);
  
  TEST_ASSERT_EQUAL_SCREENSHOT("dropdown_2.png");
}



#endif
