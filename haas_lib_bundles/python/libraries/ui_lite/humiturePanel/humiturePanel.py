import lvgl as lv

RESOURCES_ROOT = "S:/data/pyamp/"

lvglInitialized = False

class HumiturePanel:
    container = None
    scr = None
    humidityValue = None
    humidityLable = None

    def __init__(self):
        self.createPage()

    def createPage(self):
        global lvglInitialized

        if lvglInitialized == False:
            import display_driver
            lvglInitialized = True

        # init scr
        self.scr = lv.obj()
        self.scr.set_style_bg_color(lv.color_black(), 0)

        self.container = lv.obj(self.scr)
        self.container.set_style_bg_opa(0, 0)
        self.container.set_style_border_opa(0, 0)
        self.container.set_style_radius(0, 0)
        self.container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        self.container.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        self.container.set_flex_flow(lv.FLEX_FLOW.COLUMN)
        self.container.set_style_align(lv.ALIGN.CENTER, 0)
        self.container.set_style_pad_left(0, 0)

        # create temperature item
        self.createTemperatureItem(self.container, RESOURCES_ROOT + "temperature.png",
                RESOURCES_ROOT + "centigrade_l.png", "Temperature")

        self.createInterval(self.container, 25)

        # create humidity item
        self.createHumidityItem(self.container, RESOURCES_ROOT + "humidity.png", "Humidity")

        # load content
        lv.scr_load(self.scr)

    def showHumiture(self, temperature, humidity):
        self.showTemperature(temperature)
        self.showHumidity(humidity)

    def showTemperature(self, temperature):
        self.temperatureLable.set_text(str(int(temperature)))

    def showHumidity(self, humidity):
        self.humidityLable.set_text(str(int(humidity)) + " %")

    def createTemperatureItem(self, parent, iconPath, unityPath, tips):
        col_dsc = [lv.GRID.CONTENT, 5, lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]
        row_dsc = [lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]

        cont = lv.obj(parent)
        cont.set_style_bg_opa(0, 0)
        cont.set_style_border_opa(0, 0)
        cont.set_style_pad_all(0, 0)
        cont.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        cont.set_style_grid_column_dsc_array(col_dsc, 0)
        cont.set_style_grid_row_dsc_array(row_dsc, 0)
        cont.set_layout(lv.LAYOUT_GRID.value)

        img = lv.img(cont)
        img.set_src(iconPath)
        img.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 2)

        self.temperatureLable = lv.label(cont)
        self.temperatureLable.set_text("None")
        self.temperatureLable.set_style_text_color(lv.color_white(), 0)
        self.temperatureLable.set_style_text_font(lv.font_montserrat_48, 0)
        self.temperatureLable.set_style_pad_all(0, 0)
        self.temperatureLable.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)


        iconImg = lv.img(cont)
        iconImg.set_src(unityPath)
        iconImg.set_zoom(205)
        iconImg.set_style_pad_bottom(0, 0)
        iconImg.set_grid_cell(lv.GRID_ALIGN.START, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        tip = lv.label(cont)
        tip.set_text(tips)
        tip.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        tip.set_grid_cell(lv.GRID_ALIGN.START, 2, 2, lv.GRID_ALIGN.START, 1, 1)

    def createHumidityItem(self, parent, iconPath, tips):
        col_dsc = [lv.GRID.CONTENT, 5, lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]
        row_dsc = [lv.GRID.CONTENT, lv.GRID.CONTENT, lv.GRID_TEMPLATE.LAST]

        cont = lv.obj(parent)
        cont.set_style_bg_opa(0, 0)
        cont.set_style_border_opa(0, 0)
        cont.set_style_pad_all(0, 0)
        cont.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        cont.set_style_grid_column_dsc_array(col_dsc, 0)
        cont.set_style_grid_row_dsc_array(row_dsc, 0)
        cont.set_layout(lv.LAYOUT_GRID.value)

        img = lv.img(cont)
        img.set_src(iconPath)
        img.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 2)

        self.humidityLable = lv.label(cont)
        self.humidityLable.set_text("None %")
        self.humidityLable.set_style_text_color(lv.color_white(), 0)
        self.humidityLable.set_style_text_font(lv.font_montserrat_48, 0)
        self.humidityLable.set_style_pad_all(0, 0)
        self.humidityLable.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        tip = lv.label(cont)
        tip.set_text(tips)
        tip.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        tip.set_grid_cell(lv.GRID_ALIGN.START, 2, 2, lv.GRID_ALIGN.START, 1, 1)


    def createInterval(self, parent, size):
        interval = lv.obj(parent)
        interval.set_style_bg_opa(0, 0)
        interval.set_style_border_opa(0, 0)
        interval.set_height(int(size))
        interval.set_width(0)
