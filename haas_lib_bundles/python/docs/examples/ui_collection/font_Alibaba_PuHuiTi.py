import lvgl as lv

font_not_load_16 = True
info_16 = None
style_16 = None

def set_text_size_16(parent):
    global font_not_load_16
    global info_16
    global style_16
    if (font_not_load_16):
        info_16 = lv.ft_info_t()
        info_16.name ="/data/pyamp/font/AlibabaPuHuiTiM_16.ttf"
        info_16.weight = 16
        info_16.style = lv.FT_FONT_STYLE.NORMAL
        info_16.font_init()

        style_16 = lv.style_t()
        style_16.init()
        style_16.set_text_font(info_16.font)
        font_not_load_16 = False
    parent.add_style(style_16, 0)

font_not_load_20 = True
info_20 = None
style_20 = None
def set_text_size_20(parent):
    global font_not_load_20
    global info_20
    global style_20
    if (font_not_load_20):
        info_20 = lv.ft_info_t()
        info_20.name ="/data/pyamp/font/AlibabaPuHuiTiM_20.ttf"
        info_20.weight = 20
        info_20.style = lv.FT_FONT_STYLE.NORMAL
        info_20.font_init()

        style_20 = lv.style_t()
        style_20.init()
        style_20.set_text_font(info_20.font)
        font_not_load_20 = False
    parent.add_style(style_20, 0)

font_not_load_22 = True
info_22 = None
style_22 = None
def set_text_size_22(parent):
    global font_not_load_22
    global info_22
    global style_22
    if (font_not_load_22):
        info_22 = lv.ft_info_t()
        info_22.name ="/data/pyamp/font/AlibabaPuHuiTiM_22.ttf"
        info_22.weight = 22
        info_22.style = lv.FT_FONT_STYLE.NORMAL
        info_22.font_init()

        style_22 = lv.style_t()
        style_22.init()
        style_22.set_text_font(info_22.font)
        font_not_load_22 = False
    parent.add_style(style_22, 0)

font_not_load_38 = True
info_38 = None
style_38 = None
def set_text_size_38(parent):
    global font_not_load_38
    global info_38
    global style_38
    if (font_not_load_38):
        info_38 = lv.ft_info_t()
        info_38.name ="/data/pyamp/font/AlibabaPuHuiTiM_38.ttf"
        info_38.weight = 38
        info_38.style = lv.FT_FONT_STYLE.NORMAL
        info_38.font_init()

        style_38 = lv.style_t()
        style_38.init()
        style_38.set_text_font(info_38.font)
        font_not_load_38 = False
    parent.add_style(style_38, 0)

def set_text_size(parent, size):
    if (size == 38):
        set_text_size_38(parent)
    elif (size == 22):
        set_text_size_22(parent)
    elif (size == 20):
        set_text_size_20(parent)
    else:
        set_text_size_16(parent)

