import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

def event2_handler(evt):
    code = evt.get_code()
    ele = evt.get_target()
    if code == lv.EVENT.CLICKED:
        import page_haas
        page_haas.load_page()
    elif code == lv.EVENT.PRESSED:
        ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)
    elif code == lv.EVENT.RELEASED:
        ele.set_style_bg_color(lv.color_make(255, 106, 0), 0)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_make(20, 27, 31), 0)

    loadImg =lv.img(scr)
    loadImg.set_src(RESOURCES_ROOT + "success.png")
    loadImg.align(lv.ALIGN.CENTER, 0, -50)
    loadImg.set_zoom(100)

    label1 = lv.label(scr)
    label1.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label1, 20)
    label1.set_text("#ffffff 设备连接激活成功 #")
    label1.align_to(loadImg, lv.ALIGN.OUT_BOTTOM_MID, 0, -20)

    # create a simple button
    btn2 = lv.btn(scr)
    btn2.add_event_cb(event2_handler,lv.EVENT.ALL, None)
    btn2.align(lv.ALIGN.BOTTOM_MID, 0, -50)
    btn2.set_style_bg_color(lv.color_make(255, 106, 0), 0)
    btn2.set_style_border_color(lv.color_make(255, 106, 0), 0)
    btn2.set_style_radius(10, 0)
    label2=lv.label(btn2)
    font_Alibaba_PuHuiTi.set_text_size(label2, 20)
    label2.set_text("HaaSPython案例中心")

    lv.scr_load(scr)
