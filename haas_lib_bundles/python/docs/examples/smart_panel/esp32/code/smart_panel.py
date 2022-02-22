# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/MicroPython/smart_panel/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

from environment import Environment
from hvac import Hvac
from compass import Compass
from music import Music
from soundttf import SoundTTF
from timer import Timer

def init():
    print("main init")

    # import SDL as display
    # import lvgl as lv
    # lv.init()
    # display.init()

    # # init display
    # draw_buf = lv.disp_draw_buf_t()
    # buf1_1 = bytearray(38400)
    # buf1_2 = bytearray(38400)
    # draw_buf.init(buf1_1, buf1_2, 19200)
    # disp_drv = lv.disp_drv_t()
    # disp_drv.init()
    # disp_drv.draw_buf = draw_buf
    # disp_drv.flush_cb = display.monitor_flush
    # disp_drv.hor_res = 320
    # disp_drv.ver_res = 240
    # disp_drv.register()

    # # init touch
    # indev_drv=lv.indev_drv_t()
    # indev_drv.init()
    # indev_drv.read_cb = display.mouse_read
    # indev_drv.type = lv.INDEV_TYPE.POINTER
    # indev_drv.register()


    import lvgl as lv
    lv.init()

    # import display
    # display.init()
    from display import LVGL as disp
    disp.init()

    # # init display
    # draw_buf = lv.disp_draw_buf_t()
    # buf1_1 = bytearray(38400)
    # buf1_2 = bytearray(38400)
    # draw_buf.init(buf1_1, buf1_2, 19200)
    # disp_drv = lv.disp_drv_t()
    # disp_drv.init()
    # disp_drv.draw_buf = draw_buf
    # disp_drv.flush_cb = display.flush
    # disp_drv.hor_res = 320
    # disp_drv.ver_res = 240
    # disp_drv.register()
    #
    # # init touch
    # indev_drv=lv.indev_drv_t()
    # indev_drv.init()
    # indev_drv.read_cb = display.touch_read
    # indev_drv.type = lv.INDEV_TYPE.POINTER
    # indev_drv.register()

    # audio_path = '/data/pyamp/audio/'
    # audio_file = 'test_long.mp3'
    # audio_src = 'file:/' + audio_path + audio_file

    # from audio import Player
    # player = Player()
    # player.open()
    # player.play(uri=audio_src, sync=False)

    # # Compass().createPage()
    # # hvac = Hvac()
    # # hvac.createPage()
    # # music = Music()
    # # music.createPage()

    # # base demo
    # # scr = lv.obj()
    # # btn = lv.btn(scr)
    # # btn.align_to(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
    # # label = lv.label(btn)
    # # label.set_text("Hello World!")
    # # lv.scr_load(scr)

isFirst = True
main_alive = False

needAnimation = False

# set the grid data
images = [RESOURCES_ROOT + "images/env.png",
          RESOURCES_ROOT + "images/hvac.png",
          RESOURCES_ROOT + "images/music.png",
          RESOURCES_ROOT + "images/timer.png",
          RESOURCES_ROOT + "images/compass.png",
          RESOURCES_ROOT + "images/sound_ttf.png"]
titles = ["Environment",
          "Hvac",
          "Music",
          "Timer",
          "Compass",
          "Sound TTF"]

def element_pressed_cb(e):
    import lvgl as lv
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0x39, 0x39, 0x39), 0)

def element_released_cb(e):
    import lvgl as lv
    ele = e.get_target()
    ele.set_style_bg_color(lv.color_make(0xf, 0xf, 0xf), 0)

def element_click_cb(e, name):
    global main_alive

    print(name)

    if (main_alive):
        if (name == "Environment"):
            envPage = Environment()
            envPage.createPage()
        elif (name == "Hvac"):
            hvac = Hvac()
            hvac.createPage()
        elif (name == "Compass"):
            compassPage = Compass()
            compassPage.createPage()
        elif (name == "Sound"):
            ttfPage = SoundTTF()
            ttfPage.createPage()
        elif (name == "Music"):
            musicPage = Music()
            musicPage.createPage()
        elif (name == "Timer"):
            timePage = Timer()
            timePage.createPage()

        main_alive = False

def load_smart_panel():
    global isFirst
    global main_alive
    global needAnimation

    import lvgl as lv

    print("load_smart_panel, first: ", isFirst)

    # init scr
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    # init grid layout
    col_dsc = [96, 96, 96, lv.GRID_TEMPLATE.LAST]
    row_dsc = [107, 107, lv.GRID_TEMPLATE.LAST]
    gridLayout = lv.obj(scr)
    gridLayout.set_style_bg_opa(0, 0)
    gridLayout.set_style_border_opa(0, 0)
    gridLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
    gridLayout.set_style_grid_column_dsc_array(col_dsc, 0)
    gridLayout.set_style_grid_row_dsc_array(row_dsc, 0)
    gridLayout.set_size(scr.get_width(), scr.get_height())
    gridLayout.set_style_pad_left(10, 0)
    gridLayout.set_style_pad_right(10, 0)
    gridLayout.set_style_pad_top(9, 0)
    gridLayout.set_style_pad_bottom(9, 0)
    gridLayout.set_layout(lv.LAYOUT_GRID.value)

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

        if (titles[i] == "Environment"):
            element.add_event_cb(lambda e: element_click_cb(e, "Environment"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "Hvac"):
            element.add_event_cb(lambda e: element_click_cb(e, "Hvac"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "Music"):
            element.add_event_cb(lambda e: element_click_cb(e, "Music"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "Timer"):
            element.add_event_cb(lambda e: element_click_cb(e, "Timer"), lv.EVENT.CLICKED, None)
        elif (titles[i] == "Compass"):
            element.add_event_cb(lambda e: element_click_cb(e, "Compass"), lv.EVENT.CLICKED, None)
        else:
            element.add_event_cb(lambda e: element_click_cb(e, "Sound"), lv.EVENT.CLICKED, None)

        img=lv.img(element)
        img.set_src(images[i])

        label = lv.label(element)
        label.set_text(titles[i])
        label.set_style_text_color(lv.color_make(0xcc, 0xcc, 0xcc), 0)
        label.set_style_text_font(lv.font_montserrat_12, 0)

    if (isFirst):
        lv.scr_load(scr)
        isFirst = False
    else:
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_RIGHT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)

    main_alive = True
