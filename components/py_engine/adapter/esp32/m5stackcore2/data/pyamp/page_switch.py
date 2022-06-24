import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED:
        if obj.has_state(lv.STATE.CHECKED):
            print("State: on")
        else:
            print("State: off")

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    scr.set_flex_flow(lv.FLEX_FLOW.COLUMN)
    scr.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)

    sw = lv.switch(scr)
    sw.add_event_cb(event_handler,lv.EVENT.ALL, None)

    sw = lv.switch(scr)
    sw.add_state(lv.STATE.CHECKED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)

    sw = lv.switch(scr)
    sw.add_state(lv.STATE.DISABLED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)

    sw = lv.switch(scr)
    sw.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
    sw.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
