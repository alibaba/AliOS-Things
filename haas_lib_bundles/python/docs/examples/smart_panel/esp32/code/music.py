import lvgl as lv

from audio import Player

# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/MicroPython/smart_panel/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

functionImage = [
        RESOURCES_ROOT + "images/prev.png",
        RESOURCES_ROOT + "images/play.png",
        RESOURCES_ROOT + "images/next.png",
        RESOURCES_ROOT + "images/favorite.png"]


currentMusic = 0

musicData = [
    {
        "title":"Counting Stars",
        "album":"OneRepublic",
        "album_url": RESOURCES_ROOT + "images/album_one_republic.jpg",
        "url":"file://data/pyamp/audio/test_long.mp3",
        "duration":11,
        "favorite": False
    },
    {
        "title":"Aube",
        "album":"Darius",
        "album_url": RESOURCES_ROOT + "images/album_darius.jpg",
        "url":"file://data/pyamp/audio/spring.mp3",
        "duration":155,
        "favorite": False
    },
]


start = False
anim = None
playedTime = None
slider = None
anim_timeline = None
player = None
durationTime = 0
currentValue = 0
image = [None, None, None, None]
albumCover = None
songTitle = None
albumTitle = None
totalTime = None

music_alive = False

def music_back_click_callback(e, win):
    global anim_timeline
    global start
    global player
    global music_alive

    start = False

    # stop animation
    if (anim_timeline != None):
        lv.anim_timeline_stop(anim_timeline)
        lv.anim_timeline_del(anim_timeline)
        anim_timeline = None

    if (player != None):
        player.pause()
        player.close()
        player = None

    # load smart panel desktop
    if (music_alive):
        from smart_panel import load_smart_panel
        load_smart_panel()
        music_alive = False

def music_back_press_callback(e, image):
    image.set_zoom(280)

def music_back_release_callback(e, image):
    image.set_zoom(250)

def setLabelValue(label, value):
    global slider
    global anim
    global start
    global anim_timeline
    global durationTime

    minute = value / 60
    second = value  % 60

    # if (slider.is_dragged() == True):
    #     print("drag: %d" % value)
    #     start = False
    #
    #     lv.anim_timeline_stop(anim_timeline)
    #     lv.anim_timeline_del(anim_timeline)
    #     anim_timeline = None
    #
    #     slider.set_value(value, lv.ANIM.ON)
    #     anim.set_time((durationTime - currentValue) * 1000)
    #     anim.set_values(currentValue, durationTime)
    #     anim_timeline = lv.anim_timeline_create()
    #     lv.anim_timeline_add(anim_timeline, 0, anim)

    label.set_text('%02d:%02d'%(minute, second))

def setSpentTime(slider, value):
    global playedTime
    global currentValue
    global durationTime
    global image
    global start
    global anim_timeline
    global player
    global albumCover
    global songTitle
    global albumTitle
    global totalTime
    global currentMusic
    global musicData

    if (value >= durationTime):
        # currentMusic += 1
        # if (len(musicData) == currentMusic):
        #     currentMusic = 0
        start = False
        reset_music()
    else:
        currentValue = value
        setLabelValue(playedTime, value)
        slider.set_value(value, lv.ANIM.ON)

def cb(data):
    print(data)

def reset_music():
    global albumCover
    global songTitle
    global albumTitle
    global totalTime
    global musicData
    global currentMusic
    global durationTime
    global slider
    global anim
    global image
    global start
    global currentValue
    global anim_timeline
    global playedTime
    global player

    if (anim_timeline != None):
        lv.anim_timeline_stop(anim_timeline)
        lv.anim_timeline_del(anim_timeline)
        anim_timeline = None

    albumCover.set_src(musicData[currentMusic]["album_url"])
    songTitle.set_text(musicData[currentMusic]["title"])
    albumTitle.set_text(musicData[currentMusic]["album"])
    durationTime = musicData[currentMusic]["duration"]

    currentValue = 0

    slider.set_range(0, durationTime)
    slider.set_value(0, lv.ANIM.ON)
    anim.set_time(durationTime * 1000)
    anim.set_values(0, durationTime)
    anim_timeline = lv.anim_timeline_create()
    lv.anim_timeline_add(anim_timeline, 0, anim)

    setLabelValue(totalTime, durationTime)
    setLabelValue(playedTime, 0)

    if (player != None):
        player.pause()
        player.close()
        player = None

    if (start == False):
        image[1].set_src(RESOURCES_ROOT + "images/play.png")
    else:
        image[1].set_src(RESOURCES_ROOT + "images/pause.png")
        lv.anim_timeline_start(anim_timeline)
        player = Player()
        player.open()
        player.play(musicData[currentMusic]["url"], sync=False)
        player.on(cb)

    if (musicData[currentMusic]["favorite"] == False):
        image[3].set_src(RESOURCES_ROOT + "images/favorite.png")
    else:
        image[3].set_src(RESOURCES_ROOT + "images/favorited.png")

def controller_click_cb(e, func):
    global anim
    global start
    global anim_timeline
    global durationTime
    global player
    global image
    global currentValue
    global musicData
    global currentMusic

    print(func, anim_timeline)
    if (func == "play"):
        if (start == False):
            start = True

            if (currentValue == durationTime):
                currentValue = 0
                anim.set_time((durationTime - currentValue) * 1000)
                anim.set_values(currentValue, durationTime)
                anim_timeline = lv.anim_timeline_create()
                lv.anim_timeline_add(anim_timeline, 0, anim)

            lv.anim_timeline_start(anim_timeline)
            image[1].set_src(RESOURCES_ROOT + "images/pause.png")

            if (player == None):
                player = Player()
                player.open()
                player.play(musicData[currentMusic]["url"], sync=False)
                player.on(cb)

            else:
                player.resume()
                # state = player.getState()
                # print(state)
                # if (state == 2):
                #     player.resume()
                #     image[1].set_src(RESOURCES_ROOT + "images/pause.png")
                # else:
                #     player.pause()
                #     image[1].set_src(RESOURCES_ROOT + "images/play.png")
        else:
            start = False

            image[1].set_src(RESOURCES_ROOT + "images/play.png")

            lv.anim_timeline_stop(anim_timeline)
            lv.anim_timeline_del(anim_timeline)
            anim_timeline = None

            anim.set_time((durationTime - currentValue) * 1000)
            anim.set_values(currentValue, durationTime)
            anim_timeline = lv.anim_timeline_create()
            lv.anim_timeline_add(anim_timeline, 0, anim)
            player.pause()
    elif (func == "fav"):
        if (musicData[currentMusic]["favorite"] == False):
            image[3].set_src(RESOURCES_ROOT + "images/favorited.png")
            musicData[currentMusic]["favorite"] = True
        else:
            musicData[currentMusic]["favorite"] = False
            image[3].set_src(RESOURCES_ROOT + "images/favorite.png")

    elif (func == "next"):
        currentMusic += 1
        if (len(musicData) == currentMusic):
            currentMusic = 0

        reset_music()


    elif (func == "prev"):
        currentMusic -= 1
        if (currentMusic < 0):
            currentMusic = len(musicData) - 1

        reset_music()

class Music:
    def createPage(self):
        global anim
        global playedTime
        global durationTime
        global slider
        global audio_src
        global player
        global image
        global music_alive
        global currentMusic
        global albumCover
        global songTitle
        global albumTitle
        global totalTime
        global anim_timeline
        global scr

        print("Enter Music")

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
        backImg.add_event_cb(lambda e: music_back_click_callback(e, win), lv.EVENT.CLICKED, None)
        backImg.add_event_cb(lambda e: music_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
        backImg.add_event_cb(lambda e: music_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
        backImg.set_ext_click_area(30)

        albumCover = lv.img(win)
        albumCover.set_style_pad_left(12, 0)
        albumCover.set_style_pad_top(10, 0)

        songTitle = lv.label(win)
        songTitle.set_style_text_font(lv.font_montserrat_20, 0)
        songTitle.set_style_text_color(lv.color_white(), 0)
        songTitle.align_to(albumCover, lv.ALIGN.TOP_LEFT, 130, 3)

        albumTitle = lv.label(win)
        albumTitle.set_style_text_font(lv.font_montserrat_16, 0)
        albumTitle.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        albumTitle.align_to(songTitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 12)

        props = [lv.STYLE.BG_COLOR, 0]
        transition_dsc = lv.style_transition_dsc_t()
        transition_dsc.init(props, lv.anim_t.path_linear, 300, 0, None)

        style_main = lv.style_t()
        style_indicator =  lv.style_t()
        style_pressed_color = lv.style_t()
        style_main.init()
        style_main.set_bg_opa(lv.OPA.COVER)
        style_main.set_bg_color(lv.color_make(0x66, 0x66, 0x66))
        style_main.set_radius(lv.RADIUS.CIRCLE)
        style_main.set_line_dash_width(1)

        style_indicator.init()
        style_indicator.set_bg_opa(lv.OPA.COVER)
        style_indicator.set_bg_color(lv.color_white())
        style_indicator.set_radius(lv.RADIUS.CIRCLE)
        style_indicator.set_transition(transition_dsc)

        style_pressed_color.init()
        style_pressed_color.set_bg_color(lv.color_white())

        # Create a slider and add the style
        slider = lv.slider(win)
        slider.remove_style_all()                   # Remove the styles coming from the theme

        slider.add_style(style_main, lv.PART.MAIN)
        slider.add_style(style_indicator, lv.PART.INDICATOR)
        slider.add_style(style_pressed_color, lv.PART.INDICATOR | lv.STATE.PRESSED)
        slider.align_to(albumTitle, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 25)
        slider.set_size(140, 1)

        anim = lv.anim_t()
        anim.init()
        anim.set_var(slider)

        playedTime = lv.label(win)
        setLabelValue(playedTime, 0)
        playedTime.set_style_text_font(lv.font_montserrat_16, 0)
        playedTime.set_style_text_color(lv.color_white(), 0)
        playedTime.align_to(slider, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 15)

        totalTime = lv.label(win)
        totalTime.set_style_text_font(lv.font_montserrat_16, 0)
        totalTime.set_style_text_color(lv.color_white(), 0)
        totalTime.align_to(slider, lv.ALIGN.OUT_BOTTOM_RIGHT, 0, 15)

        func_col_dsc = [80, 80, 80, 80, lv.GRID_TEMPLATE.LAST]
        func_row_dsc = [40, lv.GRID_TEMPLATE.LAST]

        funcContainer = lv.obj(win)
        funcContainer.set_style_bg_opa(0x00, 0)
        funcContainer.set_style_border_opa(0x00, 0)
        funcContainer.set_layout(lv.LAYOUT_GRID.value)
        funcContainer.set_grid_dsc_array(func_col_dsc, func_row_dsc)
        funcContainer.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
        funcContainer.set_align(lv.ALIGN.BOTTOM_MID)
        funcContainer.set_size(320, 70)

        for i in range(4):
            image[i] = lv.img(funcContainer)
            image[i].set_src(functionImage[i])
            image[i].add_flag(lv.obj.FLAG.CLICKABLE)
            image[i].set_ext_click_area(20)
            image[i].set_grid_cell(lv.GRID_ALIGN.CENTER, i, 1, lv.GRID_ALIGN.CENTER, 0, 1)

            if (i == 0):
                image[i].add_event_cb(lambda e: controller_click_cb(e, "prev"), lv.EVENT.CLICKED, None)
            elif (i == 1):
                image[i].add_event_cb(lambda e: controller_click_cb(e, "play"), lv.EVENT.CLICKED, None)
            elif (i == 2):
                image[i].add_event_cb(lambda e: controller_click_cb(e, "next"), lv.EVENT.CLICKED, None)
            elif (i == 3):
                image[i].add_event_cb(lambda e: controller_click_cb(e, "fav"), lv.EVENT.CLICKED, None)

        anim.set_custom_exec_cb(lambda a1, val: setSpentTime(slider, val))
        reset_music()

        from smart_panel import needAnimation
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_LEFT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)

        music_alive = True
