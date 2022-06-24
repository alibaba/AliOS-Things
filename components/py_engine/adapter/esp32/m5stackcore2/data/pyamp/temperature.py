import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"

class Temperature:
    scr = None
    iconImg = None
    temperatureLable = None
    unityImg = None
    tipLabel = None

    def __init__(self, screen):
        self.scr = screen
        self.createTemperatureItem(self.scr, RESOURCES_ROOT + "temperature.png",
                RESOURCES_ROOT + "centigrade_l.png", "Temperature")

    def createTemperatureItem(self, parent, iconPath, unityPath, tips):
        self.iconImg = lv.img(parent)
        self.iconImg.set_src(iconPath)
        self.iconImg.align(lv.ALIGN.TOP_LEFT, 0, 0)

        self.temperatureLable = lv.label(parent)
        self.temperatureLable.set_text("None")
        self.temperatureLable.set_style_text_color(lv.color_white(), 0)
        self.temperatureLable.set_style_text_font(lv.font_montserrat_48, 0)
        self.temperatureLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)

        self.unityImg = lv.img(parent)
        self.unityImg.set_src(unityPath)
        self.unityImg.set_zoom(100)
        self.unityImg.align_to(self.temperatureLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, 0)

        self.tipLabel = lv.label(parent)
        self.tipLabel.set_text(tips)
        self.tipLabel.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        self.tipLabel.set_style_text_font(lv.font_montserrat_14, 0)
        self.tipLabel.align_to(self.temperatureLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    def setValue(self, temperature):
        self.temperatureLable.set_text(str(int(temperature)))

    def setXY(self, x, y):
        self.iconImg.align(lv.ALIGN.TOP_LEFT, x, y)
        self.temperatureLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.unityImg.align_to(self.temperatureLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, 0)
        self.tipLabel.align_to(self.temperatureLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    def setScale(self, scale):
        print("To be done")
