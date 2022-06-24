import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

def element_pressed_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_callback(e, name):
    import page_scan
    page_scan.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_make(20, 27, 31), 0)

    label1 = lv.label(scr)
    label1.set_recolor(True)                      # Enable re-coloring by commands in the text
    font_Alibaba_PuHuiTi.set_text_size(label1, 38)
    label1.set_text("欢迎使用HaaS")
    label1.set_style_text_color(lv.color_make(0xff, 0xff, 0xff), 0)
    label1.align(lv.ALIGN.CENTER, 0, -60)

    label2 = lv.label(scr)
    label2.set_recolor(True)                      # Enable re-coloring by commands in the text
    font_Alibaba_PuHuiTi.set_text_size(label2, 16)
    label2.set_text("请先连接网络激活设备")
    label2.set_style_text_color(lv.color_make(0xff, 0xff, 0xff), 0)
    label2.align(lv.ALIGN.CENTER, 0, -20)

    nextImg=lv.img(scr)
    nextImg.set_src(RESOURCES_ROOT + "next.png")
    nextImg.align(lv.ALIGN.CENTER, 0, 60)
    nextImg.add_flag(lv.obj.FLAG.CLICKABLE)
    nextImg.add_event_cb(element_pressed_cb, lv.EVENT.PRESSED, None)
    nextImg.add_event_cb(element_released_cb, lv.EVENT.RELEASED, None)
    nextImg.add_event_cb(lambda e: element_click_callback(e, scr), lv.EVENT.CLICKED, None)
    nextImg.set_zoom(150)

    lv.scr_load(scr)
