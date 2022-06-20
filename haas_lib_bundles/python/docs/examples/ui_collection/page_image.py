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

    img1 = lv.img(scr)
    img1.set_src(RESOURCES_ROOT + "setting.png")
    img1.align(lv.ALIGN.CENTER, 0, -20)
    img1.set_size(200, 200)

    img2 = lv.img(scr)
    img2.set_src(lv.SYMBOL.OK + "Accept")
    img2.align_to(img1, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)

    lv.scr_load(scr)
