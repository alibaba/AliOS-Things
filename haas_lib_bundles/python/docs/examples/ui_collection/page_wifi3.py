import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

hidenImg = None
hiden = False
wifiSSID = ""
wifiPswd = ""

def element_click_callback(e, name):
    if (name == "back"):
        import page_scan
        page_scan.load_page()
    elif (name == "correct"):
        if (wifiSSID != "" and wifiPswd != ""):
            import page_connect
            page_connect.load_page(wifiSSID, wifiPswd)

def ta1_event_cb(e, kb):
    global wifiSSID
    code = e.get_code()
    ta = e.get_target()
    if (code == lv.EVENT.VALUE_CHANGED or code == lv.EVENT.READY):
        wifiSSID = ta.get_text()
    elif (code == lv.EVENT.FOCUSED):
        kb.set_textarea(ta)
    elif (code == lv.EVENT.DEFOCUSED):
        kb.set_textarea(None)

def ta3_event_cb(e, kb):
    global wifiPswd
    code = e.get_code()
    ta = e.get_target()
    if (code == lv.EVENT.VALUE_CHANGED or code == lv.EVENT.READY):
        wifiPswd = ta.get_text()
    elif (code == lv.EVENT.FOCUSED):
        kb.set_textarea(ta)
    elif (code == lv.EVENT.DEFOCUSED):
        kb.set_textarea(None)

def hiden_event_cb(e, ta):
    global hiden
    global hidenImg
    if hiden == True:
        hiden = False
        hidenImg.set_src(RESOURCES_ROOT + "visable.png")
    else:
        hiden = True
        hidenImg.set_src(RESOURCES_ROOT + "hiden.png")
    ta.set_password_mode(hiden)
    print("hiden: ", hiden)

def load_page():
    global hidenImg

    scr = lv.obj()
    scr.set_style_bg_color(lv.color_make(20, 27, 31), 0)

    closeImg =lv.img(scr)
    closeImg.set_src(RESOURCES_ROOT + "close.png")
    closeImg.align(lv.ALIGN.TOP_LEFT, 0, 0)
    closeImg.add_flag(lv.obj.FLAG.CLICKABLE)
    closeImg.add_event_cb(lambda e: element_click_callback(e, "back"), lv.EVENT.CLICKED, None)
    closeImg.set_zoom(120)
    closeImg.add_flag(lv.obj.FLAG.CLICKABLE)


    label1 = lv.label(scr)
    label1.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label1, 22)
    label1.set_text("#ffffff 其他网络 #")
    label1.align_to(closeImg, lv.ALIGN.OUT_RIGHT_MID, -5, 0)

    choseImg =lv.img(scr)
    choseImg.set_src(RESOURCES_ROOT + "correct.png")
    choseImg.align(lv.ALIGN.TOP_RIGHT, 0, 8)
    choseImg.add_flag(lv.obj.FLAG.CLICKABLE)
    choseImg.add_event_cb(lambda e: element_click_callback(e, "correct"), lv.EVENT.CLICKED, None)
    choseImg.set_zoom(180)
    closeImg.add_flag(lv.obj.FLAG.CLICKABLE)

    kb = lv.keyboard(scr)
    kb.align(lv.ALIGN.BOTTOM_MID, 0, 0)
    kb.set_width(318)
    kb.set_height(110)
    kb.set_style_bg_color(lv.color_make(0, 0, 0), 0)

    ta0 = lv.textarea(scr)
    ta0.set_width(60)
    ta0.set_height(35)
    ta0.align(lv.ALIGN.TOP_LEFT, 10, 45)
    ta0.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    ta0.clear_flag(lv.obj.FLAG.SCROLLABLE)
    ta0.set_style_bg_color(lv.color_make(52, 63, 80), 0)
    ta0.set_style_border_color(lv.color_make(52, 63, 80), 0)
    label2 = lv.label(scr)
    label2.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label2, 16)
    label2.set_text("#ffffff 账号: #")
    label2.align_to(ta0, lv.ALIGN.CENTER, 0, 0)
    ta1 = lv.textarea(scr)
    ta1.set_width(250)
    ta1.set_height(35)
    ta1.align_to(ta0, lv.ALIGN.OUT_RIGHT_MID, -10, 0)
    ta1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    ta1.clear_flag(lv.obj.FLAG.SCROLLABLE)
    ta1.add_event_cb(lambda e: ta1_event_cb(e,kb), lv.EVENT.ALL, None)
    ta1.set_style_bg_color(lv.color_make(52, 63, 80), 0)
    ta1.set_style_border_color(lv.color_make(52, 63, 80), 0)


    ta2 = lv.textarea(scr)
    ta2.set_width(60)
    ta2.set_height(35)
    ta2.align(lv.ALIGN.TOP_LEFT, 10, 85)
    ta2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    ta2.clear_flag(lv.obj.FLAG.SCROLLABLE)
    ta2.set_style_bg_color(lv.color_make(52, 63, 80), 0)
    ta2.set_style_border_color(lv.color_make(52, 63, 80), 0)
    label3 = lv.label(scr)
    label3.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label3, 16)
    label3.set_text("#ffffff 密码: #")
    label3.align_to(ta2, lv.ALIGN.CENTER, 0, 0)
    ta3 = lv.textarea(scr)
    ta3.set_width(250)
    ta3.set_height(35)
    ta3.align_to(ta2, lv.ALIGN.OUT_RIGHT_MID, -10, 0)
    ta3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    ta3.clear_flag(lv.obj.FLAG.SCROLLABLE)
    ta3.set_password_mode(True)
    ta3.set_password_show_time(500) # 500ms
    ta3.add_event_cb(lambda e: ta3_event_cb(e,kb), lv.EVENT.ALL, None)
    ta3.set_style_bg_color(lv.color_make(52, 63, 80), 0)
    ta3.set_style_border_color(lv.color_make(52, 63, 80), 0)

    hidenImg =lv.img(ta3)
    hidenImg.set_src(RESOURCES_ROOT + "hiden.png")
    hidenImg.align_to(ta3, lv.ALIGN.RIGHT_MID, 0, 0)
    # delImg.align(lv.ALIGN.TOP_RIGHT, 50, 8)
    hidenImg.add_flag(lv.obj.FLAG.CLICKABLE)
    hidenImg.add_event_cb(lambda e: hiden_event_cb(e, ta3), lv.EVENT.CLICKED, None)
    hidenImg.set_zoom(150)
    hidenImg.add_flag(lv.obj.FLAG.CLICKABLE)
    hidenImg.set_ext_click_area(20)

    lv.scr_load(scr)
