import lvgl as lv

isStarted = False
isAnimationComplete = False
arc = [None, None, None, None]
anim = [None, None, None, None]
timeCount = [1, 3, 5, 10]
currentSelect = 0
minuteLabel = None
secondLabel = None
millionLabel = None
anim_timeline = None
startLabel = None
currentValue = 0
lvglInitialized = False

def setLabelValue(value):
    global currentValue
    global minuteLabel
    global secondLabel
    global millionLabel
    global startLabel

    currentValue = value
    currentMillionSecond = value * 20
    minute = currentMillionSecond / 1000 / 60

    minuteLabel.set_text('%02d'%minute)
    second = currentMillionSecond / 1000 % 60
    secondLabel.set_text('%02d'%second)
    million = value % 60
    millionLabel.set_text('%02d'%million)

def set_time_value(obj, v):
    setLabelValue(v)
    obj.set_value(v)

def reset_button_event_handler(e):
    global isStarted
    global isAnimationComplete
    global currentValue
    global timeCount
    global arc
    global anim
    global anim_timeline
    global startLabel

    if (isStarted):
        return

    isAnimationComplete = False
    currentValue = timeCount[currentSelect] * 60 * 50
    arc[currentSelect].set_value(currentValue)

    anim[currentSelect] = lv.anim_t()
    anim[currentSelect].init()
    anim[currentSelect].set_var(arc[currentSelect])
    anim[currentSelect].set_time(currentValue * 20)
    anim[currentSelect].set_values(currentValue, 0)
    anim[currentSelect].set_custom_exec_cb(lambda a1, val: set_time_value(arc[currentSelect], val))

    anim_timeline = lv.anim_timeline_create()
    lv.anim_timeline_add(anim_timeline, 0, anim[currentSelect])

    startLabel.set_text("START")

    setLabelValue(currentValue)


def arc_event_handler(e, index):
    global isStarted
    global currentSelect
    global arc

    print("index: " + str(index) + " currentSelect: " + str(currentSelect))
    print("isStarted: " + str(isStarted))
    if (isStarted or currentSelect == index):
        return

    arc[currentSelect].set_value(0)
    arc[currentSelect].set_style_arc_width(2, lv.PART.INDICATOR)
    arc[currentSelect].set_style_arc_width(2, lv.PART.MAIN)
    currentSelect = index
    arc[currentSelect].set_style_arc_width(8, lv.PART.INDICATOR)
    arc[currentSelect].set_style_arc_width(8, lv.PART.MAIN)
    reset_button_event_handler(e)

def start_button_event_handler(e):
    global isStarted
    global isAnimationComplete
    global anim_timeline
    global startLabel
    global anim
    global currentSelect
    global currentValue

    if (isAnimationComplete):
        return

    if (isStarted):
        isStarted = False
        lv.anim_timeline_stop(anim_timeline)
        lv.anim_timeline_del(anim_timeline)
        anim_timeline = None
        startLabel.set_text("RESUME")

        anim[currentSelect] = lv.anim_t()
        anim[currentSelect].init()
        anim[currentSelect].set_var(arc[currentSelect])
        anim[currentSelect].set_time(currentValue * 20)
        anim[currentSelect].set_values(currentValue, 0)
        anim[currentSelect].set_custom_exec_cb(lambda a1, val: set_time_value(arc[currentSelect],val))

        anim_timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(anim_timeline, 0, anim[currentSelect])
    else:
        isStarted = True
        lv.anim_timeline_start(anim_timeline)
        startLabel.set_text("PAUSE")

class CountDown:
    def createPage(self, value1 = 1, value2 = 3, value3 = 5, value4 = 10):
        global isStarted
        global isAnimationComplete
        global arc
        global anim
        global timeCount
        global currentSelect
        global minuteLabel
        global secondLabel
        global millionLabel
        global anim_timeline
        global startLabel
        global currentValue

        global lvglInitialized

        if lvglInitialized == False:
            import display_driver
            lvglInitialized = True
        print("Enter count down")

        timeCount = [value1, value2, value3, value4]

        # init scr
        scr = lv.obj()

        win = lv.obj(scr)
        win.set_size(scr.get_width(), scr.get_height())
        win.set_style_border_opa(0, 0)
        win.set_style_radius(0, 0)
        win.set_style_bg_color(lv.color_black(), 0)
        win.clear_flag(lv.obj.FLAG.SCROLLABLE)

        isStarted = False
        currentSelect = 0

        # count down
        func_col_dsc = [40, 5, 30, 5, 20, lv.GRID_TEMPLATE.LAST]
        func_row_dsc = [30, lv.GRID_TEMPLATE.LAST]

        timeContainer = lv.obj(win)
        timeContainer.set_style_bg_opa(0, 0)
        timeContainer.set_style_border_opa(0, 0)
        timeContainer.set_layout(lv.LAYOUT_GRID.value)
        timeContainer.set_style_grid_column_dsc_array(func_col_dsc, 0)
        timeContainer.set_style_grid_row_dsc_array(func_row_dsc, 0)
        timeContainer.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
        timeContainer.set_style_pad_all(0, 0)
        timeContainer.set_size(240, 70)
        timeContainer.center()

        minuteLabel = lv.label(timeContainer)
        minuteLabel.set_style_text_font(lv.font_montserrat_48, 0)
        minuteLabel.set_style_text_color(lv.color_white(), 0)
        minuteLabel.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        signLabel = lv.label(timeContainer)
        signLabel.set_style_text_font(lv.font_montserrat_48, 0)
        signLabel.set_style_text_color(lv.color_white(), 0)
        signLabel.set_text(":")
        signLabel.set_grid_cell(lv.GRID_ALIGN.CENTER, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        secondLabel = lv.label(timeContainer)
        secondLabel.set_style_text_font(lv.font_montserrat_48, 0)
        secondLabel.set_style_text_color(lv.color_white(), 0)
        secondLabel.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        signLabel = lv.label(timeContainer)
        signLabel.set_style_text_font(lv.font_montserrat_48, 0)
        signLabel.set_style_text_color(lv.color_white(), 0)
        signLabel.set_text(":")
        signLabel.set_grid_cell(lv.GRID_ALIGN.CENTER, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        millionLabel = lv.label(timeContainer)
        millionLabel.set_style_text_font(lv.font_montserrat_36, 0)
        millionLabel.set_style_text_color(lv.color_white(), 0)
        millionLabel.set_grid_cell(lv.GRID_ALIGN.END, 4, 1, lv.GRID_ALIGN.START, 0, 1)

        setLabelValue(timeCount[currentSelect] * 60 * 50)

        startButton = lv.btn(win)
        startButton.align(lv.ALIGN.CENTER, 0, 40)
        startButton.set_size(126, 54)
        startButton.set_style_radius(45, lv.PART.MAIN)
        startButton.set_style_shadow_opa(0, 0)
        startButton.set_style_bg_color(lv.color_make(0xFF, 0xA8, 0x48), lv.PART.MAIN)
        startButton.align(lv.ALIGN.BOTTOM_LEFT, 12, -12)
        startButton.add_event_cb(start_button_event_handler, lv.EVENT.CLICKED, None)

        startLabel = lv.label(startButton)
        startLabel.set_text("START")
        startLabel.set_style_text_color(lv.color_black(), 0)
        startLabel.set_style_text_font(lv.font_montserrat_20, 0)
        startLabel.center()

        resetButton = lv.btn(win)
        resetButton.align(lv.ALIGN.CENTER, 0, 40)
        resetButton.set_size(126, 54)
        resetButton.set_style_radius(45, lv.PART.MAIN)
        resetButton.set_style_shadow_opa(0, 0)
        resetButton.set_style_bg_color(lv.color_white(), lv.PART.MAIN)
        resetButton.align(lv.ALIGN.BOTTOM_RIGHT, -12, -12)
        resetButton.add_event_cb(reset_button_event_handler, lv.EVENT.CLICKED, None)

        resetLabel = lv.label(resetButton)
        resetLabel.set_text("REST")
        resetLabel.set_style_text_color(lv.color_black(), 0)
        resetLabel.set_style_text_font(lv.font_montserrat_20, 0)
        resetLabel.center()

        # select time
        col_dsc = [75, 75, 75, 75, lv.GRID_TEMPLATE.LAST]
        row_dsc = [60, 80, 60, lv.GRID_TEMPLATE.LAST]
        funcContainer = lv.obj(win)
        funcContainer.set_layout(lv.LAYOUT_GRID.value)
        funcContainer.set_style_bg_opa(0, 0)
        funcContainer.set_style_border_opa(0, 0)
        funcContainer.set_style_grid_column_dsc_array(col_dsc, 0)
        funcContainer.set_style_grid_row_dsc_array(row_dsc, 0)
        funcContainer.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
        funcContainer.set_size(300, 90)
        funcContainer.set_style_align(lv.ALIGN.TOP_MID, 0)

        maxMillionSecond = timeCount[0] * 60 * 50
        arc[0] = lv.arc(funcContainer)
        arc[0].set_style_arc_color(lv.color_white(), lv.PART.INDICATOR)
        arc[0].set_style_arc_color(lv.color_make(0x33, 0x33, 0x33), lv.PART.MAIN)
        arc[0].set_range(0, maxMillionSecond)
        arc[0].set_size(55, 55)
        arc[0].set_rotation(90)
        arc[0].set_bg_angles(0, 360)
        arc[0].remove_style(None, lv.PART.KNOB)
        arc[0].set_value(maxMillionSecond)
        arc[0].set_style_arc_width(8, lv.PART.INDICATOR)
        arc[0].set_style_arc_width(8, lv.PART.MAIN)
        arc[0].set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        arc[0].clear_flag(lv.obj.FLAG.CLICKABLE)
        totalTime = lv.label(funcContainer)
        totalTime.set_text(str(timeCount[0]))
        totalTime.set_style_text_font(lv.font_montserrat_18, 0)
        totalTime.set_style_text_color(lv.color_white(), 0)
        totalTime.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        totalTime.add_flag(lv.obj.FLAG.CLICKABLE)
        totalTime.add_event_cb(lambda e: arc_event_handler(e, 0), lv.EVENT.CLICKED, None)
        totalTime.set_ext_click_area(30)

        anim[0] = lv.anim_t()
        anim[0].init()
        anim[0].set_var(arc[0])
        anim[0].set_time(maxMillionSecond * 20)
        anim[0].set_values(maxMillionSecond, 0)
        anim[0].set_custom_exec_cb(lambda a1, val: set_time_value(arc[0], val))
        anim_timeline = lv.anim_timeline_create()
        lv.anim_timeline_add(anim_timeline, 0, anim[0])

        arc[1] = lv.arc(funcContainer)
        arc[1].set_style_arc_color(lv.color_white(), lv.PART.INDICATOR)
        arc[1].set_style_arc_color(lv.color_make(0x33, 0x33, 0x33), lv.PART.MAIN)
        arc[1].set_range(0, maxMillionSecond)
        arc[1].set_size(55, 55)
        arc[1].set_rotation(90)
        arc[1].set_bg_angles(0, 360)
        arc[1].remove_style(None, lv.PART.KNOB)
        arc[1].set_value(0)
        arc[1].set_style_arc_width(2, lv.PART.INDICATOR)
        arc[1].set_style_arc_width(2, lv.PART.MAIN)
        arc[1].set_grid_cell(lv.GRID_ALIGN.CENTER, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        arc[1].clear_flag(lv.obj.FLAG.CLICKABLE)
        totalTime = lv.label(funcContainer)
        totalTime.set_text(str(timeCount[1]))
        totalTime.set_style_text_font(lv.font_montserrat_18, 0)
        totalTime.set_style_text_color(lv.color_white(), 0)
        totalTime.set_grid_cell(lv.GRID_ALIGN.CENTER, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        totalTime.add_flag(lv.obj.FLAG.CLICKABLE)
        totalTime.add_event_cb(lambda e: arc_event_handler(e, 1), lv.EVENT.CLICKED, None)
        totalTime.set_ext_click_area(30)

        arc[2] = lv.arc(funcContainer)
        arc[2].set_style_arc_color(lv.color_white(), lv.PART.INDICATOR)
        arc[2].set_style_arc_color(lv.color_make(0x33, 0x33, 0x33), lv.PART.MAIN)
        arc[2].set_range(0, maxMillionSecond)
        arc[2].set_size(55, 55)
        arc[2].set_rotation(90)
        arc[2].set_bg_angles(0, 360)
        arc[2].remove_style(None, lv.PART.KNOB)
        arc[2].set_value(0)
        arc[2].set_style_arc_width(2, lv.PART.INDICATOR)
        arc[2].set_style_arc_width(2, lv.PART.MAIN)
        arc[2].set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        arc[2].clear_flag(lv.obj.FLAG.CLICKABLE)
        totalTime = lv.label(funcContainer)
        totalTime.set_text(str(timeCount[2]))
        totalTime.set_style_text_font(lv.font_montserrat_18, 0)
        totalTime.set_style_text_color(lv.color_white(), 0)
        totalTime.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        totalTime.add_flag(lv.obj.FLAG.CLICKABLE)
        totalTime.add_event_cb(lambda e: arc_event_handler(e, 2), lv.EVENT.CLICKED, None)
        totalTime.set_ext_click_area(30)

        arc[3] = lv.arc(funcContainer)
        arc[3].set_style_arc_color(lv.color_white(), lv.PART.INDICATOR)
        arc[3].set_style_arc_color(lv.color_make(0x33, 0x33, 0x33), lv.PART.MAIN)
        arc[3].set_range(0, maxMillionSecond)
        arc[3].set_size(55, 55)
        arc[3].set_rotation(90)
        arc[3].set_bg_angles(0, 360)
        arc[3].remove_style(None, lv.PART.KNOB)
        arc[3].set_value(0)
        arc[3].set_style_arc_width(2, lv.PART.INDICATOR)
        arc[3].set_style_arc_width(2, lv.PART.MAIN)
        arc[3].set_grid_cell(lv.GRID_ALIGN.CENTER, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        arc[3].clear_flag(lv.obj.FLAG.CLICKABLE)
        totalTime = lv.label(funcContainer)
        totalTime.set_text(str(timeCount[3]))
        totalTime.set_style_text_font(lv.font_montserrat_18, 0)
        totalTime.set_style_text_color(lv.color_white(), 0)
        totalTime.set_grid_cell(lv.GRID_ALIGN.CENTER, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        totalTime.add_flag(lv.obj.FLAG.CLICKABLE)
        totalTime.add_event_cb(lambda e: arc_event_handler(e, 3), lv.EVENT.CLICKED, None)
        totalTime.set_ext_click_area(30)

        # load content
        lv.scr_load(scr)
