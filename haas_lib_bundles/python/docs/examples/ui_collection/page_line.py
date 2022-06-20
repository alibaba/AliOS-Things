import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    line_points = [ {"x":5, "y":5},
                {"x":70, "y":70},
                {"x":120, "y":10},
                {"x":180, "y":60},
                {"x":240, "y":10}]

    # Create style
    style_line = lv.style_t()
    style_line.init()
    style_line.set_line_width(8)
    style_line.set_line_color(lv.palette_main(lv.PALETTE.BLUE))
    style_line.set_line_rounded(True)

    # Create a line and apply the new style
    line1 = lv.line(scr)
    line1.set_points(line_points, 5)     # Set the points
    line1.add_style(style_line, 0)
    line1.center()

    lv.scr_load(scr)
