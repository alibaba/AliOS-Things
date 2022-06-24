import lvgl as lv
from driver import TIMER

RESOURCES_ROOT = "S:/data/pyamp/images/"

images = [RESOURCES_ROOT + "wifi.png",
          RESOURCES_ROOT + "bluetooth.png",
          RESOURCES_ROOT + "brightness.png",
          RESOURCES_ROOT + "about.png"]

titles = ["Wi-Fi",
          "Bluetooth",
          "Brightness",
          "About"]

wifiImg = None
btImg = None
wifiSwitch = False
btSwitch = False
tim0 = None

def oneshot_callback(args):
    global tim0
    tim0.stop()
    tim0.close()
    import page_scan
    page_scan.load_page()

def switch_click_callback(e, para):
    global wifiImg
    global btImg
    global wifiSwitch
    global btSwitch
    global tim0
    print("params: ", para)
    if (para == "wifi"):
        if (wifiSwitch == False):
            wifiSwitch = True
            wifiImg.set_src(RESOURCES_ROOT + "on.png")
            tim0 = TIMER(0)
            tim0.open(mode=tim0.ONE_SHOT, period=1000, callback=oneshot_callback)
            tim0.start()
        else:
            wifiSwitch = False
            wifiImg.set_src(RESOURCES_ROOT + "off.png")
    elif (para == "about"):
        import page_about
        page_about.load_page()
    else:
        if (btSwitch == False):
            btSwitch = True
            btImg.set_src(RESOURCES_ROOT + "on.png")
        else:
            btSwitch = False
            btImg.set_src(RESOURCES_ROOT + "off.png")

def element_pressed_cb(e):
    print("back pressed")
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    print("back released")
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_callback(e, name):
    import page_haas
    page_haas.load_page()

def load_page():
    global wifiImg
    global btImg
    global wifiSwitch
    global wifiSwitch

    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    # init grid layout
    col_dsc = [20, 50, 150, scr.get_width()-225, lv.GRID_TEMPLATE.LAST]
    row_dsc = [54, 54, 54, 54, lv.GRID_TEMPLATE.LAST]
    gridLayout = lv.obj(scr)
    gridLayout.set_style_bg_opa(0, 0)
    gridLayout.set_style_border_opa(0, 0)
    gridLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
    gridLayout.set_style_grid_column_dsc_array(col_dsc, 0)
    gridLayout.set_style_grid_row_dsc_array(row_dsc, 0)
    gridLayout.set_size(scr.get_width(), scr.get_height())
    gridLayout.set_style_pad_left(0, 0)
    gridLayout.set_style_pad_right(0, 0)
    gridLayout.set_style_pad_top(3, 0)
    gridLayout.set_style_pad_bottom(3, 0)
    gridLayout.set_layout(lv.LAYOUT_GRID.value)
    gridLayout.align(lv.ALIGN.RIGHT_MID, 0, 0)

    for i in range(16):
        col = i % 4   # 列
        row = i // 4  # 行

        element = lv.obj(gridLayout)
        element.set_style_border_opa(0, 0)
        element.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        element.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
        element.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        element.clear_flag(lv.obj.FLAG.SCROLLABLE)
        element.set_style_radius(0, 0)

        if (col == 0):
            element.set_style_bg_color(lv.color_black(), 0)
            element.set_grid_cell(lv.GRID_ALIGN.STRETCH, 0, 1, lv.GRID_ALIGN.STRETCH, 1, 2)
            backImg=lv.img(element)
            backImg.set_src(RESOURCES_ROOT + "back.png")
            backImg.align(lv.ALIGN.CENTER, 0, 0)
            backImg.add_flag(lv.obj.FLAG.CLICKABLE)
            backImg.set_ext_click_area(20)
            backImg.add_event_cb(element_pressed_cb, lv.EVENT.PRESSED, None)
            backImg.add_event_cb(element_released_cb, lv.EVENT.RELEASED, None)
            backImg.add_event_cb(lambda e: element_click_callback(e, scr), lv.EVENT.CLICKED, None)
        else:
            element.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)
            element.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1)

        if (col == 1):
            img=lv.img(element)
            img.set_src(images[row])

        if (col == 2):
            label = lv.label(scr)
            label.set_text(titles[row])
            label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
            label.set_style_text_font(lv.font_montserrat_18, 0)
            label.align_to(element, lv.ALIGN.LEFT_MID, 0, 0)
            if (row == 3):
                element.add_event_cb(lambda e: switch_click_callback(e, "about"), lv.EVENT.CLICKED, None)

        if (col == 3):
            if (row == 0):
                wifiImg=lv.img(element)
                if (wifiSwitch == False):
                    wifiImg.set_src(RESOURCES_ROOT + "off.png")
                else:
                    wifiImg.set_src(RESOURCES_ROOT + "on.png")
                wifiImg.set_zoom(220)
                wifiImg.align(lv.ALIGN.CENTER, 0, 0)
                wifiImg.add_flag(lv.obj.FLAG.CLICKABLE)
                wifiImg.add_event_cb(lambda e: switch_click_callback(e, "wifi"), lv.EVENT.CLICKED, None)
                wifiImg.set_ext_click_area(20)

            if (row == 1):
                btImg=lv.img(element)
                if (btSwitch == False):
                    btImg.set_src(RESOURCES_ROOT + "off.png")
                else:
                    btImg.set_src(RESOURCES_ROOT + "on.png")
                btImg.set_zoom(220)
                btImg.align(lv.ALIGN.CENTER, 0, 0)
                btImg.add_flag(lv.obj.FLAG.CLICKABLE)
                btImg.add_event_cb(lambda e: switch_click_callback(e, "bt"), lv.EVENT.CLICKED, None)
                btImg.set_ext_click_area(20)

            if (row == 2):
                label = lv.label(element)
                label.set_text('65%')
                label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
                label.set_style_text_font(lv.font_montserrat_20, 0)
                label.align(lv.ALIGN.RIGHT_MID, 0, 0)
    lv.scr_load(scr)
