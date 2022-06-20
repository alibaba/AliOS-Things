import lvgl as lv
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

images = [RESOURCES_ROOT + "FM.png",
          RESOURCES_ROOT + "humidity.png",
          RESOURCES_ROOT + "lux.png",
          RESOURCES_ROOT + "distance.png",
          RESOURCES_ROOT + "airpressure.png",
          RESOURCES_ROOT + "hcho.png"]

titles = ["调频收音机",
          "水质",
          "光照",
          "超声测距",
          "气压",
          "甲醛"]

def element_pressed_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_cb(e, name):
    if (name == "FM"):
        import page_fm
        page_fm.load_page()
    elif (name == "Humidity"):
        import page_humidity
        page_humidity.load_page()
    elif (name == "Lux"):
        import page_lux
        page_lux.load_page()
    elif (name == "Distance"):
        import page_distance
        page_distance.load_page()
    elif (name == "Airpressure"):
        import page_airpressure
        page_airpressure.load_page()
    elif (name == "Hcho"):
        import page_hcho
        page_hcho.load_page()

def environment_back_click_callback(e, win):
    import page_sensors1
    page_sensors1.load_page()

def environment_back_press_callback(e, back_image):
    back_image.set_zoom(280)

def environment_back_release_callback(e, back_image):
    back_image.set_zoom(250)

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
    backImg.add_event_cb(lambda e: environment_back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.add_event_cb(lambda e: environment_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
    backImg.add_event_cb(lambda e: environment_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
    backImg.set_ext_click_area(20)


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
        if (titles[i] == "调频收音机"):
            element.add_event_cb(lambda e: element_click_cb(e, "FM"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "水质"):
            element.add_event_cb(lambda e: element_click_cb(e, "Humidity"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "光照"):
            element.add_event_cb(lambda e: element_click_cb(e, "Lux"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "超声测距"):
            element.add_event_cb(lambda e: element_click_cb(e, "Distance"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "气压"):
            element.add_event_cb(lambda e: element_click_cb(e, "Airpressure"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "甲醛"):
            element.add_event_cb(lambda e: element_click_cb(e, "Hcho"), lv.EVENT.CLICKED, None)
    lv.scr_load(scr)
