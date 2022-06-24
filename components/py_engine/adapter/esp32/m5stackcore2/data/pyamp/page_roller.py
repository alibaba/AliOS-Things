import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(e):
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.VALUE_CHANGED:
        option = " "*10
        obj.get_selected_str(option, len(option))
        print("Selected month: " + option.strip())

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    roller1 = lv.roller(scr)
    roller1.set_options("\n".join([
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"]),lv.roller.MODE.INFINITE)

    roller1.set_visible_row_count(4)
    roller1.center()
    roller1.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
