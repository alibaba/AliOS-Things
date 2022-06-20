import lvgl as lv
import uos
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

def environment_back_click_callback(e, win):
    import page_settings
    page_settings.load_page()

def environment_back_press_callback(e, back_image):
    back_image.set_zoom(280)

def environment_back_release_callback(e, back_image):
    back_image.set_zoom(250)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: environment_back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.add_event_cb(lambda e: environment_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
    backImg.add_event_cb(lambda e: environment_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
    backImg.set_ext_click_area(30)

    #bg_color = lv.palette_lighten(lv.PALETTE.LIGHT_BLUE, 5)
    #fg_color = lv.palette_darken(lv.PALETTE.BLUE, 4)

    #qr1 = lv.qrcode(scr, 130, fg_color, bg_color)
    #data1 = "https://haas.iot.aliyun.com/solution"
    #qr1.update(data1,len(data1))
    #qr1.align(lv.ALIGN.CENTTER, -10, -20)
    #qr1.set_style_border_color(bg_color, 0)
    #qr1.set_style_border_width(5, 0)

    label1 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label1, 16)
    label1.set_text("更多趣味案例")
    label1.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
    label1.align(lv.ALIGN.BOTTOM_MID, -70, -50)

    #qr2 = lv.qrcode(scr, 130, fg_color, bg_color)
    #data2 = "https://h5.dingtalk.com/circle/healthCheckin.html?dtaction=os&corpId=dingb846e861c83d9a57ca21756a27946a0e&5730f=56fd0&cbdbhh=qwertyuiop"
    #qr2.update(data2,len(data2))
    #qr2.align(lv.ALIGN.CENTTER, 10, -20)
    #qr2.set_style_border_color(bg_color, 0)
    #qr2.set_style_border_width(5, 0)

    label2 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label2, 16)
    label2.set_text("扫码进入钉群")
    label2.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
    label2.align(lv.ALIGN.BOTTOM_MID, 70, -50)

    label3 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label3, 16)
    label3.set_style_text_color(lv.color_make(0, 0, 0xFF), 0)
    label3.set_text("固件: ")
    label3.align(lv.ALIGN.BOTTOM_LEFT, 20, -20)

    label33 = lv.label(scr)
    label33.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR) # Circular scroll
    label33.set_width(230)
    label33.set_style_text_color(lv.color_make(0, 0, 0xFF), 0)
    font_Alibaba_PuHuiTi.set_text_size(label33, 16)
    label33.set_text(uos.version_info())
    label33.align_to(label3, lv.ALIGN.OUT_RIGHT_MID, 5, 0)

    lv.scr_load(scr)
