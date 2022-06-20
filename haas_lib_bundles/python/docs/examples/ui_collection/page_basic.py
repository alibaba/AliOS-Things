import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

titles = ["line", "label", "image",
          "button", "bar", "arc",
          "btnmatrix", "checkbox", "dropdown",
          "roller", "slider", "switch",
          "table", "keyboard", "calendar",
          "led", "list", "meter",
          "msgbox", "spans", "tabview"]

def element_pressed_cb(e):
    print('pressed')
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    print('released')
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_cb(e, name):
    print("click: ", name)
    if (name == "line"):
        import page_line
        page_line.load_page()
    if (name == "label"):
        import page_label
        page_label.load_page()
    if (name == "image"):
        import page_image
        page_image.load_page()
    if (name == "button"):
        import page_button
        page_button.load_page()
    if (name == "bar"):
        import page_bar
        page_bar.load_page()
    if (name == "arc"):
        import page_arc
        page_arc.load_page()
    if (name == "btnmatrix"):
        import page_btnmatrix
        page_btnmatrix.load_page()
    if (name == "checkbox"):
        import page_checkbox
        page_checkbox.load_page()
    if (name == "dropdown"):
        import page_dropdown
        page_dropdown.load_page()
    if (name == "roller"):
        import page_roller
        page_roller.load_page()
    if (name == "slider"):
        import page_slider
        page_slider.load_page()
    if (name == "switch"):
        import page_switch
        page_switch.load_page()
    if (name == "table"):
        import page_table
        page_table.load_page()
    if (name == "keyboard"):
        import page_keyboard
        page_keyboard.load_page()
    if (name == "calendar"):
        import page_calendar
        page_calendar.load_page()
    if (name == "led"):
        import page_led
        page_led.load_page()
    if (name == "list"):
        import page_list
        page_list.load_page()
    if (name == "meter"):
        import page_meter
        page_meter.load_page()
    if (name == "msgbox"):
        import page_msgbox
        page_msgbox.load_page()
    if (name == "spans"):
        import page_spans
        page_spans.load_page()
    if (name == "tabview"):
        import page_tabview
        page_tabview.load_page()

def backImg_click_callback(e, win):
    import page_haas
    page_haas.load_page()

def nextImg_click_callback(e, win):
    import page_sensors2
    page_sensors2.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.align(lv.ALIGN.LEFT_MID, 0, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: backImg_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    style = lv.style_t()
    style.init()
    style.set_flex_flow(lv.FLEX_FLOW.ROW_WRAP)
    style.set_flex_main_place(lv.FLEX_ALIGN.SPACE_EVENLY)
    style.set_layout(lv.LAYOUT_FLEX.value)

    cont = lv.obj(scr)
    cont.set_size(290, 220)
    cont.align_to(backImg, lv.ALIGN.OUT_RIGHT_MID, -5, 0)
    cont.add_style(style, 0)
    cont.set_style_bg_opa(0, 0)
    cont.set_style_border_opa(0, 0)
    cont.set_style_pad_left(5, 0)
    cont.set_style_pad_right(5, 0)
    cont.set_style_pad_top(9, 0)
    cont.set_style_pad_bottom(9, 0)

    for i in range(21):
        obj = lv.obj(cont)
        obj.set_size(85, 60)
        obj.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        obj.clear_flag(lv.obj.FLAG.SCROLLABLE)
        obj.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)
        obj.set_style_border_color(lv.color_make(0xf, 0xf, 0xf), 0)
        obj.set_style_radius(0, 0)
        obj.add_event_cb(element_pressed_cb, lv.EVENT.PRESSED, None)
        obj.add_event_cb(element_released_cb, lv.EVENT.RELEASED, None)
        if (i == 0):
            obj.add_event_cb(lambda e: element_click_cb(e, "line"), lv.EVENT.CLICKED, None)
        elif (i == 1):
            obj.add_event_cb(lambda e: element_click_cb(e, "label"), lv.EVENT.CLICKED, None)
        elif (i == 2):
            obj.add_event_cb(lambda e: element_click_cb(e, "image"), lv.EVENT.CLICKED, None)
        elif (i == 3):
            obj.add_event_cb(lambda e: element_click_cb(e, "button"), lv.EVENT.CLICKED, None)
        elif (i == 4):
            obj.add_event_cb(lambda e: element_click_cb(e, "bar"), lv.EVENT.CLICKED, None)
        elif (i == 5):
            obj.add_event_cb(lambda e: element_click_cb(e, "arc"), lv.EVENT.CLICKED, None)
        elif (i == 6):
            obj.add_event_cb(lambda e: element_click_cb(e, "btnmatrix"), lv.EVENT.CLICKED, None)
        elif (i == 7):
            obj.add_event_cb(lambda e: element_click_cb(e, "checkbox"), lv.EVENT.CLICKED, None)
        elif (i == 8):
            obj.add_event_cb(lambda e: element_click_cb(e, "dropdown"), lv.EVENT.CLICKED, None)
        elif (i == 9):
            obj.add_event_cb(lambda e: element_click_cb(e, "roller"), lv.EVENT.CLICKED, None)
        elif (i == 10):
            obj.add_event_cb(lambda e: element_click_cb(e, "slider"), lv.EVENT.CLICKED, None)
        elif (i == 11):
            obj.add_event_cb(lambda e: element_click_cb(e, "switch"), lv.EVENT.CLICKED, None)
        elif (i == 12):
            obj.add_event_cb(lambda e: element_click_cb(e, "table"), lv.EVENT.CLICKED, None)
        elif (i == 13):
            obj.add_event_cb(lambda e: element_click_cb(e, "keyboard"), lv.EVENT.CLICKED, None)
        elif (i == 14):
            obj.add_event_cb(lambda e: element_click_cb(e, "calendar"), lv.EVENT.CLICKED, None)
        elif (i == 15):
            obj.add_event_cb(lambda e: element_click_cb(e, "led"), lv.EVENT.CLICKED, None)
        elif (i == 16):
            obj.add_event_cb(lambda e: element_click_cb(e, "list"), lv.EVENT.CLICKED, None)
        elif (i == 17):
            obj.add_event_cb(lambda e: element_click_cb(e, "meter"), lv.EVENT.CLICKED, None)
        elif (i == 18):
            obj.add_event_cb(lambda e: element_click_cb(e, "msgbox"), lv.EVENT.CLICKED, None)
        elif (i == 19):
            obj.add_event_cb(lambda e: element_click_cb(e, "spans"), lv.EVENT.CLICKED, None)
        elif (i == 20):
            obj.add_event_cb(lambda e: element_click_cb(e, "tabview"), lv.EVENT.CLICKED, None)

        label = lv.label(obj)
        label.set_text(titles[i])
        label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
        label.center()

    lv.scr_load(scr)
