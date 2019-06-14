/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lvgl/lvgl.h"
#include "freetype_display.h"

LV_IMG_DECLARE(freetype_font_20);
LV_IMG_DECLARE(freetype_font_22);
LV_IMG_DECLARE(freetype_font_24);
LV_IMG_DECLARE(freetype_font_26);

void freetype_display(void)
{
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.text.color = LV_COLOR_BLUE;
    style.text.font = &freetype_font_20;   /*Unicode and symbol fonts already assigned by the library*/

    lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label, 10, 20);
    lv_label_set_style(label, &style);
    lv_label_set_text(label, "拣选大家永远");

    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.color = LV_COLOR_BLUE;
    style1.text.font = &freetype_font_22;   /*Unicode and symbol fonts already assigned by the library*/

    lv_obj_t *label1 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label1, 10, 60);
    lv_label_set_style(label1, &style1);
    lv_label_set_text(label1, "拣选大家永远");

    static lv_style_t style2;
    lv_style_copy(&style2, &lv_style_plain);
    style2.text.color = LV_COLOR_BLUE;
    style2.text.font = &freetype_font_24;   /*Unicode and symbol fonts already assigned by the library*/

    lv_obj_t *label2 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label2, 10, 100);
    lv_label_set_style(label2, &style2);
    lv_label_set_text(label2, "拣选大家永远");


    static lv_style_t style3;
    lv_style_copy(&style3, &lv_style_plain);
    style3.text.color = LV_COLOR_BLUE;
    style3.text.font = &freetype_font_26;   /*Unicode and symbol fonts already assigned by the library*/

    lv_obj_t *label3 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label3, 10, 140);
    lv_label_set_style(label3, &style3);
    lv_label_set_text(label3, "拣选大家永远");
}
