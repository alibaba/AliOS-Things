import lvgl as lv
import wifi_module
import _thread
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

wifiSSID = ""
wifiPassword = ""

def wifi_connect():
    global wifiSSID
    global wifiPassword
    result = False

    result = wifi_module.wifi_connect(wifiSSID, wifiPassword)

    if (result == False):
        print("WARNING: Device activates failed !!")
        import page_failed
        page_failed.load_page()
    else:
        print("Device activates successllly !!")
        import page_success
        page_success.load_page()

def set_angle(img, v):
    img.set_angle(v)

def load_page(ssid, password):
    global wifiSSID
    global wifiPassword

    wifiSSID = ssid
    wifiPassword = password
    print("Wi-Fi SSID: ", wifiSSID)
    print("Wi-Fi PASSWORD: ", wifiPassword)
    print("Wi-Fi Connecting ...")

    scr = lv.obj()
    scr.set_style_bg_color(lv.color_make(20, 27, 31), 0)

    loadImg =lv.img(scr)
    loadImg.set_src(RESOURCES_ROOT + "loading.png")
    loadImg.align(lv.ALIGN.CENTER, 0, -20)
    loadImg.set_zoom(200)

    a1 = lv.anim_t()
    a1.init()
    a1.set_var(loadImg)
    a1.set_custom_exec_cb(lambda a,val: set_angle(loadImg,val))
    a1.set_values(0, 3600)
    a1.set_time(2000)
    a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    lv.anim_t.start(a1)

    label1 = lv.label(scr)
    label1.set_recolor(True)
    font_Alibaba_PuHuiTi.set_text_size(label1, 20)
    label1.set_text("#ffffff 网络连接中 #")
    label1.align_to(loadImg, lv.ALIGN.OUT_BOTTOM_MID, 0, 20)

    lv.scr_load(scr)

    _thread.stack_size(10 * 1024)
    _thread.start_new_thread(wifi_connect, ())
