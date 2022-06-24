import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"
calendar = None

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(evt):
    global calendar
    code = evt.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = evt.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:
            calendar.set_today_date(date.year, date.month, date.day)
            print("Clicked date: %02d.%02d.%02d"%(date.day, date.month, date.year))

def load_page():
    global calendar
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    calendar = lv.calendar(scr)
    calendar.set_size(280, 200)
    calendar.align(lv.ALIGN.CENTER, 0, 0)
    calendar.add_event_cb(event_handler, lv.EVENT.ALL, None)

    calendar.set_today_date(2022, 06, 06)
    calendar.set_showed_date(2022, 06)

    # Highlight a few days
    highlighted_days=[
        lv.calendar_date_t({'year':2022, 'month':6, 'day':3}),
        lv.calendar_date_t({'year':2022, 'month':6, 'day':4}),
        lv.calendar_date_t({'year':2022, 'month':6, 'day':5})
    ]

    calendar.set_highlighted_dates(highlighted_days, len(highlighted_days))

    lv.calendar_header_dropdown(calendar)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
