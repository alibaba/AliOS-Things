import lvgl as lv

# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/MicroPython/smart_panel/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

sound_ttf_alive = False

def sound_ttf_back_click_callback(e, win):
    global sound_ttf_alive
    if (sound_ttf_alive):
        from smart_panel import load_smart_panel
        load_smart_panel()
        sound_ttf_alive = False

def sound_ttf_back_press_callback(e, image):
    image.set_zoom(280)

def sound_ttf_back_release_callback(e, image):
    image.set_zoom(250)

def chart_event_cb(e):
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if (dsc.part == lv.PART.ITEMS):
        draw_rect_dsc = lv.draw_rect_dsc_t()
        draw_rect_dsc.init()

        a = lv.area_t()
        a.x1 = dsc.draw_area.x1
        a.x2 = dsc.draw_area.x2
        a.y1 = dsc.draw_area.y1
        a.y2 = a.y1 + 6

        draw_rect_dsc.bg_color = lv.color_white()
        draw_rect_dsc.radius = 0
        draw_rect_dsc.shadow_opa = 0
        lv.draw_rect(a, dsc.clip_area, draw_rect_dsc)

value = 65


class SoundTTF:
    def createPage(self):
        global value
        global sound_ttf_alive

        print("Enter SoundTTF")

        # init scr
        scr = lv.obj()

        win = lv.obj(scr)
        win.set_size(scr.get_width(), scr.get_height())
        win.set_style_border_opa(0, 0)
        win.set_style_radius(0, 0)
        win.set_style_bg_color(lv.color_black(), 0)
        win.clear_flag(lv.obj.FLAG.SCROLLABLE)
        win.set_style_pad_right(30, 0)

        title=lv.label(win)
        title.set_text("Sound TTF")
        title.set_style_text_color(lv.color_white(), 0)
        title.set_style_text_font(lv.font_montserrat_28, 0)
        title.align(lv.ALIGN.TOP_LEFT, 20, 0)

        decibel = lv.label(win)
        decibel.set_text(str(value) + "dB")
        decibel.set_style_text_color(lv.color_make(0xFF, 0xA8, 0x48), 0)
        decibel.set_style_text_font(lv.font_montserrat_28, 0)
        decibel.align(lv.ALIGN.TOP_RIGHT, 0, 0)

        chart = lv.chart(win)
        chart.set_type(lv.chart.TYPE.BAR)
        chart.set_style_border_opa(0, 0)
        chart.set_style_bg_opa(0, 0)
        chart.set_style_line_opa(0, 0)
        chart.set_width(280)
        chart.set_height(160)
        chart.set_div_line_count(6, 0)
        chart.set_point_count(12)
        chart.align(lv.ALIGN.BOTTOM_MID, 20, -5)
        chart.add_event_cb(chart_event_cb, lv.EVENT.DRAW_PART_END, None)

        ser1 = chart.add_series(lv.color_make(0x56, 0x56, 0x56), lv.chart.AXIS.PRIMARY_Y)
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))
        chart.set_next_value(ser1, lv.rand(30, 100))

        backImg=lv.img(win)
        backImg.set_src(RESOURCES_ROOT + "images/back.png")
        backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
        backImg.add_flag(lv.obj.FLAG.CLICKABLE)
        backImg.add_event_cb(lambda e: sound_ttf_back_click_callback(e, win), lv.EVENT.CLICKED, None)
        backImg.set_ext_click_area(30)
        backImg.add_event_cb(lambda e: sound_ttf_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
        backImg.add_event_cb(lambda e: sound_ttf_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)

        from smart_panel import needAnimation
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_LEFT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)
            
        sound_ttf_alive = True
