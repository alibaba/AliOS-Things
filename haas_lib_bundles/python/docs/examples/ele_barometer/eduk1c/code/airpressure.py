import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

class Airpressure:
    scr = None
    iconImg = None
    airpressureLable = None
    unityLabel = None
    tipLabel = None
    date = None

    def __init__(self, screen):
        self.scr = screen
        self.createAirpressureItem(self.scr, RESOURCES_ROOT + "airpressure.png", "Air pressure")

    def createAirpressureItem(self, parent, iconPath, tips):
        self.iconImg = lv.img(parent)
        self.iconImg.set_src(iconPath)
        self.iconImg.align(lv.ALIGN.TOP_LEFT, 0, 0)

        self.airpressureLable = lv.label(parent)
        self.airpressureLable.set_text("None")
        self.airpressureLable.set_style_text_color(lv.color_white(), 0)
        self.airpressureLable.set_style_text_font(lv.font_montserrat_48, 0)
        self.airpressureLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)

        self.unityLabel = lv.label(parent)
        self.unityLabel.set_text(" PA")
        self.unityLabel.set_style_text_color(lv.color_white(), 0)
        self.unityLabel.set_style_text_font(lv.font_montserrat_18, 0)
        self.unityLabel.align_to(self.airpressureLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, -5)

        self.tipLabel = lv.label(parent)
        self.tipLabel.set_text(tips)
        self.tipLabel.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        self.tipLabel.set_style_text_font(lv.font_montserrat_14, 0)
        self.tipLabel.align_to(self.airpressureLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

        # 设置日期
        self.date = lv.label(parent)
        self.date.align(lv.ALIGN.TOP_LEFT, 0, 0)
        self.date.set_style_text_color(lv.color_white(), 0)
        self.date.set_style_text_font(lv.font_montserrat_18, 0)

    def setValue(self, pressure):
        self.airpressureLable.set_text(str(int(pressure)))

    def setXY(self, x, y):
        self.iconImg.align(lv.ALIGN.TOP_LEFT, x, y)
        self.airpressureLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.unityLabel.align_to(self.airpressureLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, -5)
        self.tipLabel.align_to(self.airpressureLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    def setScale(self, scale):
        print("To be done")

    def setDate(self, time):
        self.date.set_text("{}-{:0>2d}-{:0>2d} {:0>2d}:{:0>2d}:{:0>2d}"\
            .format(time[0], time[1], time[2],time[3], time[4], time[5]))
