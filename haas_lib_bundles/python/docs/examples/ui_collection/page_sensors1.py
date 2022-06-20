import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

images = [RESOURCES_ROOT + "temperature.png",
          RESOURCES_ROOT + "humidity.png",
          RESOURCES_ROOT + "gas.png",
          RESOURCES_ROOT + "heartrate.png",
          RESOURCES_ROOT + "waterlevel.png",
          RESOURCES_ROOT + "angle.png"]

titles = ["温度",
          "湿度",
          "燃气",
          "心率",
          "水位",
          "舵机"]

def element_pressed_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_cb(e, name):
    if (name == "Temperature"):
        import page_temperature
        page_temperature.load_page()
    elif (name == "Humiture"):
        import page_humiture
        page_humiture.load_page()
    elif (name == "Gas"):
        import page_gas
        page_gas.load_page()
    elif (name == "Heartrate"):
        import page_heartrate
        page_heartrate.load_page()
    elif (name == "Waterlevel"):
        import page_waterlevel
        page_waterlevel.load_page()
    elif (name == "Angle"):
        import page_angle
        page_angle.load_page()

def backImg_click_callback(e, win):
    import page_haas
    page_haas.load_page()

def nextImg_click_callback(e, win):
    import page_sensors2
    page_sensors2.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    # init grid layout
    col_dsc = [89, 89, 89, lv.GRID_TEMPLATE.LAST]
    row_dsc = [107, 107, lv.GRID_TEMPLATE.LAST]
    gridLayout = lv.obj(scr)
    gridLayout.set_style_bg_opa(0, 0)
    gridLayout.set_style_border_opa(0, 0)
    gridLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
    gridLayout.set_style_grid_column_dsc_array(col_dsc, 0)
    gridLayout.set_style_grid_row_dsc_array(row_dsc, 0)
    gridLayout.set_size(scr.get_width(), scr.get_height())
    gridLayout.set_style_pad_left(20, 0)
    gridLayout.set_style_pad_right(20, 0)
    gridLayout.set_style_pad_top(9, 0)
    gridLayout.set_style_pad_bottom(8, 0)
    gridLayout.set_layout(lv.LAYOUT_GRID.value)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.align(lv.ALIGN.LEFT_MID, 0, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: backImg_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    nextImg=lv.img(scr)
    nextImg.set_src(RESOURCES_ROOT + "forward.png")
    nextImg.align(lv.ALIGN.RIGHT_MID, 0, 0)
    nextImg.add_flag(lv.obj.FLAG.CLICKABLE)
    nextImg.add_event_cb(lambda e: nextImg_click_callback(e, scr), lv.EVENT.CLICKED, None)
    nextImg.set_ext_click_area(20)


    for i in range(6):
        col = i % 3
        row = i // 3
        element = lv.obj(gridLayout)
        element.set_style_border_opa(0, 0)
        element.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        element.set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.CENTER)
        element.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
        element.clear_flag(lv.obj.FLAG.SCROLLABLE)
        element.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)
        element.set_style_radius(0, 0)
        element.set_grid_cell(lv.GRID_ALIGN.STRETCH, col, 1, lv.GRID_ALIGN.STRETCH, row, 1)
        element.add_event_cb(element_pressed_cb, lv.EVENT.PRESSED, None)
        element.add_event_cb(element_released_cb, lv.EVENT.RELEASED, None)

        img=lv.img(element)
        img.set_src(images[i])

        label = lv.label(element)
        font_Alibaba_PuHuiTi.set_text_size(label, 13)
        label.set_text(titles[i])
        label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
        label.align_to(img, lv.ALIGN.OUT_BOTTOM_MID, 0, 0)
        if (titles[i] == "温度"):
            element.add_event_cb(lambda e: element_click_cb(e, "Temperature"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "湿度"):
            element.add_event_cb(lambda e: element_click_cb(e, "Humiture"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "燃气"):
            element.add_event_cb(lambda e: element_click_cb(e, "Gas"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "心率"):
            element.add_event_cb(lambda e: element_click_cb(e, "Heartrate"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "水位"):
            element.add_event_cb(lambda e: element_click_cb(e, "Waterlevel"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "舵机"):
            element.add_event_cb(lambda e: element_click_cb(e, "Angle"), lv.EVENT.CLICKED, None)
    lv.scr_load(scr)
