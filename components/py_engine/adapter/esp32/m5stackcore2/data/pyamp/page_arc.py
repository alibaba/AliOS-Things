import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # 创建Arc对象，设置其末端的角度、尺寸及位置
    arc = lv.arc(scr)
    arc.set_end_angle(200)
    arc.set_size(150, 150)
    arc.center()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
