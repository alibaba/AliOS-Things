import lvgl as lv
import _thread
import wifi_module
from driver import TIMER
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"
tim0 = None
ap_list = None
WIFI_SCAN_CHECK_PERIOD_MS = 2000 # ms
WIFI_SCAN_TIMEOUT_MS = 6000 # ms
wifi_scan_fail_count = 0

def element_click_callback(e, name):
    global tim0
    tim0.stop()
    tim0.close()
    print("intent: ", name)
    if (name == "back"):
        import page_welcome
        page_welcome.load_page()
    elif (name == "skip"):
        import page_haas
        page_haas.load_page()

def set_angle(img, v):
    img.set_angle(v)

def wifi_scan():
    global ap_list
    ap_list = wifi_module.wifi_scan()
    print("ap_list:", ap_list)

def oneshot_callback(args):
    global ap_list
    global wifi_scan_fail_count

    if (ap_list == None):
        print("wifi scan not finished.")
        wifi_scan_fail_count += 1
        if (wifi_scan_fail_count == WIFI_SCAN_TIMEOUT_MS/ WIFI_SCAN_CHECK_PERIOD_MS):
            import page_scanfail
            page_scanfail.load_page()
        else:
            tim0.open(mode=tim0.ONE_SHOT, period = WIFI_SCAN_CHECK_PERIOD_MS, callback=oneshot_callback)
            tim0.start()
        return

    tim0.stop()
    tim0.close()
    num = len(ap_list)
    print(ap_list)
    for ap in ap_list:
        print('ssid: ', bytes.decode(ap[0]))
        if (bytes.decode(ap[0]) == ""):
            ap_list.remove(ap)
            ap = ap_list[0]

    num = len(ap_list)
    print(ap_list)
    if (num > 0):
        if (num > 4):
            num = 4
        import page_wifi0
        page_wifi0.load_page(ap_list, num)
    else:
        import page_scanfail
        page_scanfail.load_page()

def load_page():
    global elementList
    global tim0
    global wifi_scan_fail_count

    _thread.stack_size(10 * 1024)
    _thread.start_new_thread(wifi_scan, ())

    wifi_scan_fail_count = 0
    tim0 = TIMER(0)
    tim0.open(mode=tim0.ONE_SHOT, period = WIFI_SCAN_CHECK_PERIOD_MS, callback=oneshot_callback)
    tim0.start()

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
    loadImg.set_src(RESOURCES_ROOT + "loading.png")
    loadImg.align(lv.ALIGN.CENTER, 0, -10)
    loadImg.set_zoom(200)

    label3 = lv.label(scr)
    label3.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label3, 20)
    label3.set_text("#ffffff 搜索Wi-Fi中 #")
    label3.align_to(loadImg, lv.ALIGN.OUT_BOTTOM_MID, 0, 10)
    label3.add_event_cb(lambda e: element_click_callback(e, "scan"), lv.EVENT.CLICKED, None)

    a1 = lv.anim_t()
    a1.init()
    a1.set_var(loadImg)
    a1.set_custom_exec_cb(lambda a,val: set_angle(loadImg,val))
    a1.set_values(0, 3600)
    a1.set_time(WIFI_SCAN_CHECK_PERIOD_MS)
    a1.set_repeat_count(int(WIFI_SCAN_TIMEOUT_MS/ WIFI_SCAN_CHECK_PERIOD_MS))
    lv.anim_t.start(a1)

    lv.scr_load(scr)
