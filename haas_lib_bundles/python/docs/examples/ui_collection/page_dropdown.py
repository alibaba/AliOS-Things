import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

# 下拉列表动作回调函数
def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED:
        option = " "*10 # should be large enough to store the option
        obj.get_selected_str(option, len(option))
        # .strip() removes trailing spaces
        print("Option: \"%s\"" % option.strip())

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # 创建下拉列表
    dd = lv.dropdown(scr)

    # 设置下拉列表下拉选项
    dd.set_options("\n".join([
        "Apple",
        "Banana",
        "Orange",
        "Cherry",
        "Grape",
        "Raspberry",
        "Melon",
        "Orange",
        "Lemon",
        "Nuts"]))

    # 设置下拉选项的样式
    dd.align(lv.ALIGN.TOP_MID, 0, 20)

    # 设置下拉列表回调函数
    dd.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
