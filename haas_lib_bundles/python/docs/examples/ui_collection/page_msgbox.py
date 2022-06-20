import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_cb(e):
    mbox = e.get_current_target()
    print("Button %s clicked" % mbox.get_active_btn_text())

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    btns = ["Apply", "Close", ""]

    mbox1 = lv.msgbox(scr, "Hello", "This is a message box with two buttons.", btns, True)
    mbox1.add_event_cb(event_cb, lv.EVENT.VALUE_CHANGED, None)
    mbox1.center()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
