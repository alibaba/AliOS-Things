import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/images/"


class Waterlevel:
    scr = None
    iconImg = None
    waterlevelLable = None
    unityLabel = None
    tipLabel = None

    def __init__(self, screen):
        self.scr = screen
        self.createWaterlevelItem(self.scr, RESOURCES_ROOT + "waterlevel.png", "Water level")

    def createWaterlevelItem(self, parent, iconPath, tips):
        self.iconImg = lv.img(parent)
        self.iconImg.set_src(iconPath)
        self.iconImg.align(lv.ALIGN.TOP_LEFT, 0, 0)

        self.waterlevelLable = lv.label(parent)
        self.waterlevelLable.set_text("None")
        self.waterlevelLable.set_style_text_color(lv.color_white(), 0)
        self.waterlevelLable.set_style_text_font(lv.font_montserrat_48, 0)
        self.waterlevelLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)

        self.unityLabel = lv.label(parent)
        self.unityLabel.set_text(" M")
        self.unityLabel.set_style_text_color(lv.color_white(), 0)
        self.unityLabel.set_style_text_font(lv.font_montserrat_18, 0)
        self.unityLabel.align_to(self.waterlevelLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, -5)

        self.tipLabel = lv.label(parent)
        self.tipLabel.set_text(tips)
        self.tipLabel.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        self.tipLabel.set_style_text_font(lv.font_montserrat_14, 0)
        self.tipLabel.align_to(self.waterlevelLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    def setValue(self, humidity):
        self.waterlevelLable.set_text(str(int(humidity)))

    def setXY(self, x, y):
        self.iconImg.align(lv.ALIGN.TOP_LEFT, x, y)
        self.waterlevelLable.align_to(self.iconImg, lv.ALIGN.OUT_RIGHT_TOP, 0, 0)
        self.unityLabel.align_to(self.waterlevelLable, lv.ALIGN.OUT_RIGHT_BOTTOM, 0, -5)
        self.tipLabel.align_to(self.waterlevelLable, lv.ALIGN.OUT_BOTTOM_LEFT, 0, 0)

    def setScale(self, scale):
        print("To be done")
