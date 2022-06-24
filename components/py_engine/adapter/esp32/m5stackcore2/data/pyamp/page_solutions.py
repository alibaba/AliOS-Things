import lvgl as lv
from aliyunIoT import Device
import _thread
import ujson as json
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"
IMAGE_NAME = "plate.jpg"

label11 = None
label22 = None
device = None

def element_pressed_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def backImg_click_callback(e, win):
    import page_haas
    page_haas.load_page()

def nextImg_click_callback(e, win):
    import page_haas
    page_haas.load_page()


def __cb_lk_service(data):
    global label11
    global label22

    if data != None :
        params = json.loads(data['params'])
        ext = json.loads(params['ext'])
        print("confidence: ", ext['confidence'])
        print("plateNumber: ", ext['plateNumber'])
        print("plateType: ", ext['plateType'])
        label11.set_text(ext['plateNumber'])
        label22.set_text(ext['plateType'])

def ai_recognition():
    global device
    if (device == None):
        device = Device()
    ret = device.default(__cb_lk_service)
    if (ret == -1):
        print("otaput connection not existed, please check it.")
        return
    fileName = IMAGE_NAME
    file = open("/data/pyamp/images/" + IMAGE_NAME)
    frame = file.read()
    fileid = device.uploadContent(fileName, frame, None)
    file.close()

    if fileid != None:
        ext = { 'filePosition':'lp', 'fileName': fileName, 'fileId': fileid }
        ext_str = json.dumps(ext)
        all_params = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': "ocrCarNo", 'argInt': 1, 'ext': ext_str }}
        all_params_str = json.dumps(all_params)
        print(all_params_str)
        upload_file = {
            'topic': '/sys/' + 'xxx' + '/' + 'xxx' + '/thing/event/hli_event/post',
            'qos': 1,
            'payload': all_params_str
        }

        # 上传完成通知HaaS聚合平台
        print('upload--->' + str(upload_file))
        device.publish(upload_file)
    else:
        print('filedid is none, upload content fail')

def load_page():
    global label11
    global label22
    global thread_id

    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.align(lv.ALIGN.LEFT_MID, 0, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: backImg_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    img = lv.img(scr)
    img.set_src(RESOURCES_ROOT + IMAGE_NAME)
    img.set_style_align(lv.ALIGN.TOP_MID, 0)
    img.set_size(280, 180)

    label1 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label1, 16)
    label1.set_text("车牌:")
    label1.set_style_text_color(lv.color_white(), 0)
    label1.align_to(img, lv.ALIGN.OUT_BOTTOM_LEFT, 10, 20)
    label11 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label11, 16)
    label11.set_text("...")
    label11.set_style_text_color(lv.color_white(), 0)
    label11.align_to(label1, lv.ALIGN.OUT_RIGHT_MID, 5, 0)

    label2 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label2, 16)
    label2.set_text("类型:")
    label2.set_style_text_color(lv.color_white(), 0)
    label2.align_to(img, lv.ALIGN.OUT_BOTTOM_LEFT, 160, 20)
    label22 = lv.label(scr)
    font_Alibaba_PuHuiTi.set_text_size(label22, 16)
    label22.set_text("...")
    label22.set_style_text_color(lv.color_white(), 0)
    label22.align_to(label2, lv.ALIGN.OUT_RIGHT_MID, 5, 0)

    # 将控件显示在屏幕上
    lv.scr_load(scr)

    _thread.stack_size(10 * 1024)
    _thread.start_new_thread(ai_recognition, ())
