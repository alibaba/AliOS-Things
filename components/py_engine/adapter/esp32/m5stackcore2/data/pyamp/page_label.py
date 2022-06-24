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

    #
    # Show line wrap, re-color, line align and text scrolling.
    #
    label1 = lv.label(scr)
    label1.set_long_mode(lv.label.LONG.WRAP)      # Break the long lines*/
    label1.set_recolor(True)                      # Enable re-coloring by commands in the text
    label1.set_text("#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center"
                            "and  wrap long text automatically.")
    label1.set_width(150)                         # Set smaller width to make the lines wrap
    label1.set_style_text_align(lv.ALIGN.CENTER, 0)
    label1.align(lv.ALIGN.CENTER, 0, -40)

    label2 = lv.label(scr)
    label2.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR) # Circular scroll
    label2.set_width(150)
    label2.set_text("It is a circularly scrolling text. ")
    label2.align(lv.ALIGN.CENTER, 0, 40)

    lv.scr_load(scr)
