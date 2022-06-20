import lvgl as lv
import kv
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

def ta_event_cb(e, kb):
    global wifiPswd
    code = e.get_code()
    ta = e.get_target()
    if (code == lv.EVENT.VALUE_CHANGED or code == lv.EVENT.READY):
        wifiPswd = ta.get_text()


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

def load_page(ssid):
    global hiden
    global hidenImg
    global wifiSSID

    wifiSSID = ssid

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
    label1.set_text("#ffffff 输入Wi-Fi密码 #")
    label1.align_to(closeImg, lv.ALIGN.OUT_RIGHT_MID, 0, 0)

    choseImg =lv.img(scr)
    choseImg.set_src(RESOURCES_ROOT + "correct.png")
    choseImg.align(lv.ALIGN.TOP_RIGHT, 0, 8)
    choseImg.add_flag(lv.obj.FLAG.CLICKABLE)
    choseImg.add_event_cb(lambda e: element_click_callback(e, "correct"), lv.EVENT.CLICKED, None)
    choseImg.set_zoom(180)
    choseImg.add_flag(lv.obj.FLAG.CLICKABLE)

    kb = lv.keyboard(scr)
    kb.align(lv.ALIGN.BOTTOM_MID, 0, 0)
    kb.set_width(318)
    kb.set_height(120)
    kb.set_style_bg_color(lv.color_make(0, 0, 0), 0)

    ta = lv.textarea(scr)
    ta.set_width(300)
    ta.set_height(40)
    ta.align(lv.ALIGN.TOP_MID, 0, 60)
    ta.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
    ta.clear_flag(lv.obj.FLAG.SCROLLABLE)
    ta.set_password_mode(hiden)
    # ta.set_one_line(True)
    ta.set_password_show_time(200) # 200ms
    ta.add_event_cb(lambda e: ta_event_cb(e,kb), lv.EVENT.ALL, None)
    ta.set_style_bg_color(lv.color_make(52, 63, 80), 0)
    ta.set_style_border_color(lv.color_make(52, 63, 80), 0)
    if (ssid == kv.get('_amp_wifi_ssid')):
        ta.set_text(kv.get('_amp_wifi_passwd'))
    kb.set_textarea(ta)

    hidenImg =lv.img(ta)
    if (hiden):
        hidenImg.set_src(RESOURCES_ROOT + "hiden.png")
    else:
        hidenImg.set_src(RESOURCES_ROOT + "visable.png")
    hidenImg.align_to(ta, lv.ALIGN.RIGHT_MID, 0, 0)
    # delImg.align(lv.ALIGN.TOP_RIGHT, 50, 8)
    hidenImg.add_flag(lv.obj.FLAG.CLICKABLE)
    hidenImg.add_event_cb(lambda e: hiden_event_cb(e, ta), lv.EVENT.CLICKED, None)
    hidenImg.set_zoom(150)
    hidenImg.add_flag(lv.obj.FLAG.CLICKABLE)
    hidenImg.set_ext_click_area(20)

    lv.scr_load(scr)
