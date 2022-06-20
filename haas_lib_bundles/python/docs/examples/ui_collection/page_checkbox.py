import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED:
        txt = obj.get_text()
        if obj.get_state() & lv.STATE.CHECKED:
            state = "Checked"
        else:
            state = "Unchecked"
        print(txt + ":" + state)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # 设置画面布局样式
    scr.set_flex_flow(lv.FLEX_FLOW.COLUMN)
    scr.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

    # 创建复选框，并设定复选框名称、属性等信息并设置复选框相关动作回调函数
    cb = lv.checkbox(scr)
    cb.set_text("Apple")
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.set_text("Banana")
    cb.add_state(lv.STATE.CHECKED)
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.set_text("Lemon")
    cb.add_state(lv.STATE.DISABLED)
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    cb = lv.checkbox(scr)
    cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
    cb.set_text("Melon")
    cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 更新画面布局
    cb.update_layout()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
