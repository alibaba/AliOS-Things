import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def event_handler(evt):
    code = evt.get_code()

    if code == lv.EVENT.CLICKED:
            print("Clicked event seen")
    elif code == lv.EVENT.VALUE_CHANGED:
        print("Value changed seen")

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # 创建一个简单按钮
    btn1 = lv.btn(scr)

    # 设置按钮相关事件的callback，所有按下/松开灯事件都需要送到回调函数
    btn1.add_event_cb(event_handler,lv.EVENT.ALL, None)

    btn1.align(lv.ALIGN.CENTER,0,-40)
    label=lv.label(btn1)
    label.set_text("Button")

    # 创建一个状态开关按钮
    btn2 = lv.btn(scr)

    # 设置状态开关按钮相关事件的callback，所有按下/松开灯事件都需要送到回调函数
    btn2.add_event_cb(event_handler,lv.EVENT.ALL, None)
    #btn2.add_event_cb(event_handler,lv.EVENT.VALUE_CHANGED,None)

    btn2.align(lv.ALIGN.CENTER,0,40)
    btn2.add_flag(lv.obj.FLAG.CHECKABLE)
    btn2.set_height(lv.SIZE.CONTENT)

    label=lv.label(btn2)
    label.set_text("Toggle")
    label.center()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
