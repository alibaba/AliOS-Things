import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def environment_back_click_callback(e, win):
    import page_sensors2
    page_sensors2.load_page()

def environment_back_press_callback(e, back_image):
    back_image.set_zoom(280)

def environment_back_release_callback(e, back_image):
    back_image.set_zoom(250)

def set_xy(obj, v):
    obj.setXY(v, v)

def set_scale(obj, v):
    obj.setScale(v)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)
    from lux import Lux
    lux = Lux(scr)
    lux.setValue(1200)
    lux.setXY(0, 0)

    a1 = lv.anim_t()
    a1.init()
    a1.set_var(lux)
    a1.set_custom_exec_cb(lambda a,val: set_xy(lux, val))
    a1.set_values(0, 130)
    a1.set_time(5000)
    a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    lv.anim_t.start(a1)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: environment_back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.add_event_cb(lambda e: environment_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
    backImg.add_event_cb(lambda e: environment_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
    backImg.set_ext_click_area(20)
    lv.scr_load(scr)
