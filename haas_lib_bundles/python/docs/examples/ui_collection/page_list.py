import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"
list1 = None

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(e):
    global list1
    code = e.get_code()
    obj = e.get_target()
    if code == lv.EVENT.CLICKED:
            print("Clicked: list1." + list1.get_btn_text(obj))

def load_page():
    global list1
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    # Create a list
    list1 = lv.list(scr)
    list1.set_size(180, 220)
    list1.center()

    # Add buttons to the list
    list1.add_text("File")
    btn_new = list1.add_btn(lv.SYMBOL.FILE, "New")
    btn_new.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_open = list1.add_btn(lv.SYMBOL.DIRECTORY, "Open")
    btn_open.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_save = list1.add_btn(lv.SYMBOL.SAVE, "Save")
    btn_save.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_delete = list1.add_btn(lv.SYMBOL.CLOSE, "Delete")
    btn_delete.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_edit = list1.add_btn(lv.SYMBOL.EDIT, "Edit")
    btn_edit.add_event_cb(event_handler,lv.EVENT.ALL, None)

    list1.add_text("Connectivity")
    btn_bluetooth = list1.add_btn(lv.SYMBOL.BLUETOOTH, "Bluetooth")
    btn_bluetooth.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_navig = list1.add_btn(lv.SYMBOL.GPS, "Navigation")
    btn_navig.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_USB = list1.add_btn(lv.SYMBOL.USB, "USB")
    btn_USB.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_battery = list1.add_btn(lv.SYMBOL.BATTERY_FULL, "Battery")
    btn_battery.add_event_cb(event_handler,lv.EVENT.ALL, None)

    list1.add_text("Exit")
    btn_apply = list1.add_btn(lv.SYMBOL.OK, "Apply")
    btn_apply.add_event_cb(event_handler,lv.EVENT.ALL, None)
    btn_close = list1.add_btn(lv.SYMBOL.CLOSE, "Close")
    btn_close.add_event_cb(event_handler,lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
