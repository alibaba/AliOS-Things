import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

def element_click_callback(e, name):
    print("intent: ", name)
    if (name == "back" or name == "retry"):
        import page_scan
        page_scan.load_page()
    elif (name == "skip"):
        import page_haas
        page_haas.load_page()

def load_page():
    global elementList

    scr = lv.obj()
    scr.set_style_bg_color(lv.color_make(20, 27, 31), 0)

    rtnImg =lv.img(scr)
    rtnImg.set_src(RESOURCES_ROOT + "return.png")
    rtnImg.align(lv.ALIGN.TOP_LEFT, 0, 0)
    rtnImg.add_flag(lv.obj.FLAG.CLICKABLE)
    rtnImg.add_event_cb(lambda e: element_click_callback(e, "back"), lv.EVENT.CLICKED, None)
    rtnImg.set_zoom(150)

    label1 = lv.label(scr)
    label1.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label1, 22)
    label1.set_text("#ffffff Wi-Fi #")
    label1.align_to(rtnImg, lv.ALIGN.OUT_RIGHT_MID, 0, 0)
    # label1.align(lv.ALIGN.TOP_LEFT, 50, 13)

    # create a simple button
    obj = lv.obj(scr)
    obj.set_size(70, 50)
    obj.set_style_bg_color(lv.color_make(20, 27, 31), 0)
    obj.set_style_border_color(lv.color_make(20, 27, 31), 0)
    obj.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    obj.clear_flag(lv.obj.FLAG.SCROLLABLE)
    obj.align(lv.ALIGN.TOP_RIGHT, -10, -5)
    obj.add_flag(lv.obj.FLAG.CLICKABLE)
    obj.set_ext_click_area(50)
    obj.add_event_cb(lambda e: element_click_callback(e, "skip"), lv.EVENT.CLICKED, None)
    label2 = lv.label(obj)
    label2.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label2, 22)
    label2.set_text("#ffffff 跳过 #")

    loadImg =lv.img(scr)
    loadImg.set_src(RESOURCES_ROOT + "failed.png")
    loadImg.align(lv.ALIGN.CENTER, 0, -20)
    loadImg.set_zoom(120)

    label3 = lv.label(scr)
    label3.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label3, 20)
    label3.set_text("#ffffff 附近没有可用的Wi-Fi #")
    label3.align_to(loadImg, lv.ALIGN.OUT_BOTTOM_MID, 0, -20)

    # create a simple button
    btn1 = lv.btn(scr)
    btn1.add_event_cb(lambda e: element_click_callback(e, "retry"), lv.EVENT.CLICKED, None)
    btn1.align(lv.ALIGN.BOTTOM_MID, 0, -20)
    btn1.set_style_bg_color(lv.color_make(93, 112, 141), 0)
    btn1.set_style_border_color(lv.color_make(93, 112, 141), 0)
    btn1.set_style_radius(10, 0)
    label1=lv.label(btn1)
    font_Alibaba_PuHuiTi.set_text_size(label1, 20)
    label1.set_text("    重试    ")

    lv.scr_load(scr)
