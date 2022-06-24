import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

slider_label = None

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def slider_event_cb(e):
    global slider_label
    slider = e.get_target()
    slider_label.set_text("{:d}%".format(slider.get_value()))
    slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

def load_page():
    global slider_label
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # Create a slider in the center of the display
    slider = lv.slider(scr)
    slider.center()
    slider.add_event_cb(slider_event_cb, lv.EVENT.VALUE_CHANGED, None)

    # Create a label below the slider
    slider_label = lv.label(scr)
    slider_label.set_text("0%")

    slider_label.align_to(slider, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
