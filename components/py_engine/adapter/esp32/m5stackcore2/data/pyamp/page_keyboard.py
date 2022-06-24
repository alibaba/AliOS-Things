import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.add_flag(lv.obj.FLAG.HIDDEN)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # Create a keyboard to use it with one of the text areas
    kb = lv.keyboard(scr)

    # Create a text area. The keyboard will write here
    ta = lv.textarea(scr)
    ta.set_width(140)
    ta.set_height(100)
    ta.align(lv.ALIGN.TOP_LEFT, 10, 10)
    ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)
    ta.set_placeholder_text("Hello")

    ta = lv.textarea(scr)
    ta.set_width(140)
    ta.set_height(100)
    ta.align(lv.ALIGN.TOP_RIGHT, -10, 10)
    ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)

    kb.set_textarea(ta)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
