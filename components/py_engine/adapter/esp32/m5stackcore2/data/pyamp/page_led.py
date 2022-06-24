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

    # Create a LED and switch it OFF
    led1  = lv.led(scr)
    led1.align(lv.ALIGN.CENTER, -80, 0)
    led1.off()

    # Copy the previous LED and set a brightness
    led2  = lv.led(scr)
    led2.align(lv.ALIGN.CENTER, 0, 0)
    led2.set_brightness(150)
    led2.set_color(lv.palette_main(lv.PALETTE.RED))

    # Copy the previous LED and switch it ON
    led3  = lv.led(scr)
    led3.align(lv.ALIGN.CENTER, 80, 0)
    led3.on()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
