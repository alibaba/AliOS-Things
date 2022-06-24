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

    # Create span
    style = lv.style_t()
    style.init()
    style.set_border_width(1)
    style.set_border_color(lv.palette_main(lv.PALETTE.ORANGE))
    style.set_pad_all(2)

    spans = lv.spangroup(scr)
    spans.set_width(300)
    spans.set_height(200)
    spans.center()
    spans.add_style(style, 0)

    spans.set_align(lv.TEXT_ALIGN.CENTER)
    spans.set_overflow(lv.SPAN_OVERFLOW.CLIP)
    spans.set_indent(20)
    spans.set_mode(lv.SPAN_MODE.BREAK)

    span = spans.new_span()
    span.set_text("china is a beautiful country.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.RED))
    span.style.set_text_decor(lv.TEXT_DECOR.STRIKETHROUGH | lv.TEXT_DECOR.UNDERLINE)
    span.style.set_text_opa(lv.OPA._30)

    span = spans.new_span()
    span.set_text_static("good good study, day day up.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))

    span = spans.new_span()
    span.set_text_static("LVGL is an open-source graphics library.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.BLUE))

    span = spans.new_span()
    span.set_text_static("the boy no name.")
    span.style.set_text_color(lv.palette_main(lv.PALETTE.GREEN))
    span.style.set_text_decor(lv.TEXT_DECOR.UNDERLINE)

    span = spans.new_span()
    span.set_text("I have a dream that hope to come true.")

    spans.refr_mode()

    # 将控件显示在屏幕上
    lv.scr_load(scr)
