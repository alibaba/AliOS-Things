import lvgl as lv
import utime

# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/HaaSPython/solutions/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

def drawOver(e):
    global g_clickTime

    if (g_clickTime != 0):
        currentTime = utime.ticks_ms()
        print("create Environment page use: %dms" % int((currentTime - g_clickTime)))
        g_clickTime = 0

environment_alive = False

def environment_back_click_callback(e, win):
    global environment_alive

    if (environment_alive):
        from smart_panel import load_smart_panel
        load_smart_panel()
        environment_alive = False

def environment_back_press_callback(e, back_image):
    back_image.set_zoom(280)

def environment_back_release_callback(e, back_image):
    back_image.set_zoom(250)

class Environment:
    def createPage(self):
        global environment_alive
        global g_clickTime

        g_clickTime = utime.ticks_ms()

        # init scr
        scr = lv.obj()

        win = lv.obj(scr)
        win.set_size(scr.get_width(), scr.get_height())
        win.set_style_border_opa(0, 0)
        win.set_style_bg_color(lv.color_black(), 0)
        win.set_style_radius(0, 0)
        win.clear_flag(lv.obj.FLAG.SCROLLABLE)
        win.add_event_cb(drawOver, lv.EVENT.DRAW_POST_END, None)

        backImg=lv.img(win)
        backImg.set_src(RESOURCES_ROOT + "images/back.png")
        backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
        backImg.add_flag(lv.obj.FLAG.CLICKABLE)
        backImg.add_event_cb(lambda e: environment_back_click_callback(e, win), lv.EVENT.CLICKED, None)
        backImg.add_event_cb(lambda e: environment_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
        backImg.add_event_cb(lambda e: environment_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
        backImg.set_ext_click_area(20)

        container = lv.obj(win)
        container.set_style_bg_opa(0, 0)
        container.set_style_border_opa(0, 0)
        container.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        container.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        container.set_style_align(lv.ALIGN.CENTER, 0)
        container.set_style_pad_left(0, 0)

        self.createItem(container, RESOURCES_ROOT + "images/temperature.png", "25",
                RESOURCES_ROOT + "images/centigrade_l.png", "Temperature")
        self.createInterval(container, 25)
        self.createItem(container, RESOURCES_ROOT + "images/humidity.png", "41 %", "", "Humidity")

        from smart_panel import needAnimation
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_LEFT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)

        environment_alive = True
        currentTime = utime.ticks_ms()
        print("run python code use: %dms" % int((currentTime - g_clickTime)))

    def createItem(self, parent, iconPath, value, unityPath, tips):
        col_dsc = [lv.GRID.CONTENT, 5, lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]
        row_dsc = [lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]

        cont = lv.obj(parent)
        cont.set_style_bg_opa(0, 0)
        cont.set_style_border_opa(0, 0)
        cont.set_style_pad_all(0, 0)
        cont.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        cont.set_style_grid_column_dsc_array(col_dsc, 0)
        cont.set_style_grid_row_dsc_array(row_dsc, 0)
        cont.set_layout(lv.LAYOUT_GRID.value)

        img = lv.img(cont)
        img.set_src(iconPath)
        img.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 2)

        label = lv.label(cont)
        label.set_text(value)
        label.set_style_text_color(lv.color_white(), 0)
        label.set_style_text_font(lv.font_montserrat_48, 0)
        label.set_style_pad_all(0, 0)
        label.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        if (unityPath.strip()):
            iconImg = lv.img(cont)
            iconImg.set_src(unityPath)
            iconImg.set_zoom(205)
            iconImg.set_style_pad_bottom(0, 0)
            iconImg.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        tip = lv.label(cont)
        tip.set_text(tips)
        tip.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        tip.set_grid_cell(lv.GRID_ALIGN.START, 2, 2, lv.GRID_ALIGN.START, 1, 1)


    def createInterval(self, parent, size):
        interval = lv.obj(parent)
        interval.set_style_bg_opa(0, 0)
        interval.set_style_border_opa(0, 0)
        interval.set_height(size)
        interval.set_width(0)
