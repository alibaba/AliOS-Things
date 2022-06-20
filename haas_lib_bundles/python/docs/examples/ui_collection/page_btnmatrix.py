import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

def back_click_callback(e, win):
    import page_basic
    page_basic.load_page()

def event_handler(evt):
    code = evt.get_code()
    obj  = evt.get_target()

    if code == lv.EVENT.VALUE_CHANGED :
        id = obj.get_selected_btn()
        txt = obj.get_btn_text(id)

        print("%s was pressed"%txt)

def load_page():
    scr = lv.obj()
    scr.set_style_bg_color(lv.color_black(), 0)

    backImg=lv.img(scr)
    backImg.set_src(RESOURCES_ROOT + "back.png")
    backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
    backImg.add_flag(lv.obj.FLAG.CLICKABLE)
    backImg.add_event_cb(lambda e: back_click_callback(e, scr), lv.EVENT.CLICKED, None)
    backImg.set_ext_click_area(20)

    btnm_map = ["1", "2", "3", "4", "5", "\n",
            "6", "7", "8", "9", "0", "\n",
            "Action1", "Action2", ""]

    # 创建组合按钮，设置按钮大小，按钮上要显示的文字及按钮属性、按钮动作回调函数等
    btnm1 = lv.btnmatrix(scr)
    btnm1.set_map(btnm_map)
    btnm1.set_btn_width(10, 2)        # Make "Action1" twice as wide as "Action2"
    btnm1.set_btn_ctrl(10, lv.btnmatrix.CTRL.CHECKABLE)
    btnm1.set_btn_ctrl(11, lv.btnmatrix.CTRL.CHECKED)
    btnm1.align(lv.ALIGN.CENTER, 0, 0)
    btnm1.add_event_cb(event_handler, lv.EVENT.ALL, None)

    # 将控件显示在屏幕上
    lv.scr_load(scr)
