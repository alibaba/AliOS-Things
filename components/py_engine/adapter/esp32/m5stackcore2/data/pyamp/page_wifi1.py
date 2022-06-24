import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

wifiImg = [RESOURCES_ROOT + "wifi0.png",
           RESOURCES_ROOT + "wifi1.png",
           RESOURCES_ROOT + "wifi2.png",
           RESOURCES_ROOT + "wifi3.png"]

rssi = [3, 2, 1, 0]

ssidInfoList = [[3, "alibaba-inc", False, True],
                [3, "alibaba-guest", False, True],
                [2, "HaaS-IoT", False, True],
                [1, "CMCC-P4re", True, True],
                [0, "其他...", False, False]]

elementList = [None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None]

def element_pressed_cb(e, row):
    global elementList
    for i in range(4):
        element = elementList[row * 4 + i]
        element.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e, row):
    global elementList
    for i in range(4):
        element = elementList[row * 4 + i]
        element.set_style_bg_color(lv.color_make(52, 63, 80), 0)

def element_click_callback(e, name):
    print("intent: ", name)
    if (name == "back"):
        import page_welcome
        page_welcome.load_page()
    elif (name == "skip"):
        import page_welcome
        page_welcome.load_page()
    elif (name == "其他..."):
        import page_wifi3
        page_wifi3.load_page()
    else:
        import page_wifi2
        page_wifi2.load_page(name)

def set_angle(img, v):
    img.set_angle(v)

def load_page():
    global elementList

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
    loadImg.align(lv.ALIGN.TOP_MID, 0, 25)
    loadImg.set_zoom(120)

    a1 = lv.anim_t()
    a1.init()
    a1.set_var(loadImg)
    a1.set_custom_exec_cb(lambda a,val: set_angle(loadImg,val))
    a1.set_values(0, 3600)
    a1.set_time(2000)
    a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    lv.anim_t.start(a1)

    col_dsc = [40, 185, 40, 40, lv.GRID_TEMPLATE.LAST]
    row_dsc = [30, 30, 30, 30, 30, lv.GRID_TEMPLATE.LAST]
    gridLayout = lv.obj(scr)
    gridLayout.set_style_bg_opa(0, 0)
    gridLayout.set_style_border_opa(0, 0)
    gridLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
    gridLayout.set_style_grid_column_dsc_array(col_dsc, 0)
    gridLayout.set_style_grid_row_dsc_array(row_dsc, 0)
    gridLayout.set_size(310, 175)
    gridLayout.set_style_pad_left(0, 0)
    gridLayout.set_style_pad_right(0, 0)
    gridLayout.set_style_pad_top(3, 0)
    gridLayout.set_style_pad_bottom(3, 0)
    # gridLayout.align_to(loadImg, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
    gridLayout.align(lv.ALIGN.BOTTOM_MID, 3, 0)
    gridLayout.set_layout(lv.LAYOUT_GRID.value)
    # gridLayout.align(lv.ALIGN.RIGHT_MID, 0, 0)
    # gridLayout.set_size(scr.get_width(), scr.get_height())

    for i in range(20):
        col = i % 4   # 列
        row = i // 4  # 行

        element = lv.obj(gridLayout)
        elementList[i] = element
        element.set_style_border_opa(0, 0)
        element.set_style_bg_color(lv.color_make(52, 63, 80), 0)
        element.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        element.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
        element.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        element.clear_flag(lv.obj.FLAG.SCROLLABLE)
        element.set_style_radius(0, 0)
        element.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1)

        if (col == 0):
            img=lv.img(element)
            img.set_src(wifiImg[ssidInfoList[row][0]])
            img.align(lv.ALIGN.CENTER, 0, 0)
            img.set_zoom(120)

        if (col == 1):
            label = lv.label(scr)
            font_Alibaba_PuHuiTi.set_text_size(label, 16)
            label.set_text(ssidInfoList[row][1])
            label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
            label.align_to(element, lv.ALIGN.LEFT_MID, 0, 0)
            if (row == 0):
                element.add_event_cb(lambda e: element_pressed_cb(e, 0), lv.EVENT.PRESSED, None)
                element.add_event_cb(lambda e: element_released_cb(e, 0), lv.EVENT.RELEASED, None)
                element.add_event_cb(lambda e: element_click_callback(e, ssidInfoList[0][1]), lv.EVENT.CLICKED, None)
            elif (row == 1):
                element.add_event_cb(lambda e: element_pressed_cb(e, 1), lv.EVENT.PRESSED, None)
                element.add_event_cb(lambda e: element_released_cb(e, 1), lv.EVENT.RELEASED, None)
                element.add_event_cb(lambda e: element_click_callback(e, ssidInfoList[1][1]), lv.EVENT.CLICKED, None)
            elif (row == 2):
                element.add_event_cb(lambda e: element_pressed_cb(e, 2), lv.EVENT.PRESSED, None)
                element.add_event_cb(lambda e: element_released_cb(e, 2), lv.EVENT.RELEASED, None)
                element.add_event_cb(lambda e: element_click_callback(e, ssidInfoList[2][1]), lv.EVENT.CLICKED, None)
            elif (row == 3):
                element.add_event_cb(lambda e: element_pressed_cb(e, 3), lv.EVENT.PRESSED, None)
                element.add_event_cb(lambda e: element_released_cb(e, 3), lv.EVENT.RELEASED, None)
                element.add_event_cb(lambda e: element_click_callback(e, ssidInfoList[3][1]), lv.EVENT.CLICKED, None)
            elif (row == 4):
                element.add_event_cb(lambda e: element_pressed_cb(e, 4), lv.EVENT.PRESSED, None)
                element.add_event_cb(lambda e: element_released_cb(e, 4), lv.EVENT.RELEASED, None)
                element.add_event_cb(lambda e: element_click_callback(e, ssidInfoList[4][1]), lv.EVENT.CLICKED, None)

        if (col == 2):
            if (ssidInfoList[row][2] == True):
                img=lv.img(element)
                img.set_src(RESOURCES_ROOT + "correct.png")
                img.align(lv.ALIGN.CENTER, 0, 0)
                img.set_zoom(120)

        if (col == 3):
            if (ssidInfoList[row][3] == True):
                img=lv.img(element)
                img.set_src(RESOURCES_ROOT + "lock.png")
                img.align(lv.ALIGN.CENTER, 0, 0)
                img.set_zoom(120)

    lv.scr_load(scr)
