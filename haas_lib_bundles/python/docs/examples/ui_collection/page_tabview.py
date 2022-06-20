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

    # Create a Tab view object
    tabview = lv.tabview(scr, lv.DIR.TOP, 50)
    tabview.align(lv.ALIGN.CENTER, 0, 0)
    tabview.set_size(280, 220)

    # Add 3 tabs (the tabs are page (lv_page) and can be scrolled
    tab1 = tabview.add_tab("Tab 1")
    tab2 = tabview.add_tab("Tab 2")
    tab3 = tabview.add_tab("Tab 3")

    # Add content to the tabs
    label = lv.label(tab1)
    label.set_text("""This the first tab
If the content
of a tab
becomes too
longer
than the
container
then it
automatically
becomes
scrollable.

Can you see it?""")

    label = lv.label(tab2)
    label.set_text("Second tab")

    label = lv.label(tab3)
    label.set_text("Third tab");

    label.scroll_to_view_recursive(lv.ANIM.ON)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
