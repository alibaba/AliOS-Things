import lvgl as lv

# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/MicroPython/smart_panel/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

compass_ttf_alive = False

def compass_back_click_callback(e, win):
    global compass_ttf_alive
    if (compass_ttf_alive):
        from smart_panel import load_smart_panel
        load_smart_panel()
        compass_ttf_alive = False

def compass_back_press_callback(e, image):
    image.set_zoom(280)

def compass_back_release_callback(e, image):
    image.set_zoom(250)

value = 0

class Compass:
    def createPage(self):
        global value
        global compass_ttf_alive

        print("Enter Compass")

        # init scr
        scr = lv.obj()

        win = lv.obj(scr)
        win.set_size(scr.get_width(), scr.get_height())
        win.set_style_border_opa(0, 0)
        win.set_style_radius(0, 0)
        win.set_style_bg_color(lv.color_black(), 0)
        win.clear_flag(lv.obj.FLAG.SCROLLABLE)

        backImg=lv.img(win)
        backImg.set_src(RESOURCES_ROOT + "images/back.png")
        backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
        backImg.add_flag(lv.obj.FLAG.CLICKABLE)
        backImg.add_event_cb(lambda e: compass_back_click_callback(e, win), lv.EVENT.CLICKED, None)
        backImg.add_event_cb(lambda e: compass_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
        backImg.add_event_cb(lambda e: compass_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
        backImg.set_ext_click_area(30)

        # -------------- compass image ----------------
        compass_dial = lv.img(win)
        compass_dial.set_src(RESOURCES_ROOT + "images/compass_dial.png")
        compass_dial.set_style_max_height(scr.get_height(), 0)
        compass_dial.set_angle(value * 10)
        compass_dial.center()

        # -------------- indicator --------------------
        compass_indicator = lv.img(win)
        compass_indicator.set_src(RESOURCES_ROOT + "images/compass_indicator.png")
        # compass_indicator.set_style_max_height(scr.get_height(), 0)
        compass_indicator.set_angle(value * 10)
        compass_indicator.center()

        col_dsc = [35, 10, 25, lv.GRID_TEMPLATE.LAST]
        row_dsc = [20, 35, 20, lv.GRID_TEMPLATE.LAST]
        degreeArea = lv.obj(win)
        degreeArea.set_size(95, 100)
        degreeArea.set_style_bg_opa(0, 0)
        degreeArea.set_style_border_opa(0, 0)
        degreeArea.set_style_pad_all(0, 0)
        degreeArea.set_layout(lv.LAYOUT_GRID.value)
        degreeArea.set_grid_dsc_array(col_dsc, row_dsc)
        degreeArea.center()
        # degreeArea.set_style_pad_left(5, 0)
        degreeArea.clear_flag(lv.obj.FLAG.SCROLLABLE)

        red = lv.img(degreeArea)
        red.set_src(RESOURCES_ROOT + "images/angle_red.png")
        red.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 3, lv.GRID_ALIGN.CENTER, 0, 1)

        degreeLable = lv.label(degreeArea)
        degreeLable.set_text(str(value))
        degreeLable.set_style_text_color(lv.color_white(), 0)
        degreeLable.set_style_text_font(lv.font_montserrat_36, 0)
        degreeLable.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 1, 1)

        degreeIcon = lv.img(degreeArea)
        degreeIcon.set_src(RESOURCES_ROOT + "images/degree.png")
        degreeIcon.set_style_pad_top(5, 0)
        degreeIcon.set_grid_cell(lv.GRID_ALIGN.START, 1, 1, lv.GRID_ALIGN.START, 1, 1)

        directionLable = lv.label(degreeArea)
        directionLable.set_text("N")
        directionLable.set_style_text_color(lv.color_white(), 0)
        directionLable.set_style_text_font(lv.font_montserrat_36, 0)
        directionLable.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 1, 1)

        tips = lv.label(degreeArea)
        tips.set_text("Compass")
        tips.set_style_text_color(lv.color_white(), 0)
        tips.set_style_text_font(lv.font_montserrat_12, 0)
        tips.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 3, lv.GRID_ALIGN.CENTER, 2, 1)

        from smart_panel import needAnimation
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_LEFT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)

        compass_ttf_alive = True
