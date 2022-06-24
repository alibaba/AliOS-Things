import lvgl as lv
import wifi_module
import font_Alibaba_PuHuiTi

RESOURCES_ROOT = "S:/data/pyamp/images/"

images = [RESOURCES_ROOT + "basic_ui.png",
          RESOURCES_ROOT + "sensor_ui.png",
          RESOURCES_ROOT + "aiot_solution.png",
          RESOURCES_ROOT + "setting.png"]

titles = ["基础UI",
          "传感器UI",
          "云端一体案例",
          "设置"]

def element_pressed_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x19, 0x19, 0x19), 0)

def element_released_cb(e):
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_cb(e, name):
    if (name == "Settings"):
        import page_settings
        page_settings.load_page()
    elif (name == "Basic"):
        print("Basic demo to be done")
        import page_basic
        page_basic.load_page()
    elif (name == "Sensors"):
        import page_sensors1
        page_sensors1.load_page()
    elif (name == "Solutions"):
        if (wifi_module.is_wifi_connected() == False):
            import page_failed
            page_failed.load_page()
        else:
            import page_solutions
            page_solutions.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    # init grid layout
    col_dsc = [145, 145, lv.GRID_TEMPLATE.LAST]
    row_dsc = [105, 105, lv.GRID_TEMPLATE.LAST]
    gridLayout = lv.obj(scr)
    gridLayout.set_style_bg_opa(0, 0)
    gridLayout.set_style_border_opa(0, 0)
    gridLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
    gridLayout.set_style_grid_column_dsc_array(col_dsc, 0)
    gridLayout.set_style_grid_row_dsc_array(row_dsc, 0)
    gridLayout.set_style_pad_left(10, 0)
    gridLayout.set_style_pad_right(10, 0)
    gridLayout.set_style_pad_top(10, 0)
    gridLayout.set_style_pad_bottom(10, 0)
    gridLayout.set_layout(lv.LAYOUT_GRID.value)
    gridLayout.set_size(scr.get_width(), scr.get_height())
    gridLayout.align(lv.ALIGN.CENTER, 0, 0)

    for i in range(4):
        col = i % 2
        row = i // 2
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
        img.align(lv.ALIGN.CENTER, 0, -20)

        label = lv.label(element)
        font_Alibaba_PuHuiTi.set_text_size(label, 16)
        label.set_text(titles[i])
        label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
        label.align_to(img, lv.ALIGN.OUT_BOTTOM_MID, 0, -5)

        if (titles[i] == "设置"):
            element.add_event_cb(lambda e: element_click_cb(e, "Settings"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "基础UI"):
            element.add_event_cb(lambda e: element_click_cb(e, "Basic"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "传感器UI"):
            element.add_event_cb(lambda e: element_click_cb(e, "Sensors"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "云端一体案例"):
            element.add_event_cb(lambda e: element_click_cb(e, "Solutions"), lv.EVENT.CLICKED, None)
    lv.scr_load(scr)
