import lvgl as lv

# RESOURCES_ROOT = "S:/Users/liujuncheng/workspace/iot/esp32/solution/MicroPython/smart_panel/smart_panel/"
RESOURCES_ROOT = "S:/data/pyamp/"

environment_alive = False

functionImage = [
        RESOURCES_ROOT + "images/refrigeration.png",
        RESOURCES_ROOT + "images/heating.png",
        RESOURCES_ROOT + "images/dehumidification.png",
        RESOURCES_ROOT + "images/ventilation.png"]

functionImageSelected = [
        RESOURCES_ROOT + "images/refrigeration_selected.png",
        RESOURCES_ROOT + "images/heating_selected.png",
        RESOURCES_ROOT + "images/dehumidification_selected.png",
        RESOURCES_ROOT + "images/ventilation_selected.png"]

currentFunc = 0
currentSelected = None

hvac_alive = False

def hvac_back_click_callback(e, win):
    global hvac_alive

    if (hvac_alive):
        from smart_panel import load_smart_panel
        load_smart_panel()
        hvac_alive = False

def hvac_back_press_callback(e, image):
    image.set_zoom(280)

def hvac_back_release_callback(e, image):
    image.set_zoom(250)

def sub_pressed_cb(e, self):
    print("sub")

    if (self.value > 16):
        self.value -= 1
        print("value %d" % (self.value))
        self.label.set_text(str(self.value))

def add_pressed_cb(e, self):
    print("add")

    if (self.value < 30):
        self.value += 1
        print("value %d" % (self.value))
        self.label.set_text(str(self.value))

def func_pressed_cb(e, index):
    global currentFunc
    global currentSelected

    print(index)

    if (index != currentFunc):
        currentSelected.set_src(functionImage[currentFunc])
        selectedImage = e.get_target()
        currentFunc = index
        selectedImage.set_src(functionImageSelected[currentFunc])
        currentSelected = selectedImage


class Hvac:
    value = 25

    def createPage(self):
        global currentFunc
        global currentSelected
        global hvac_alive

        print("Enter Hvac")

        # init scr
        scr = lv.obj()

        win = lv.obj(scr)
        win.set_size(scr.get_width(), scr.get_height())
        win.set_style_border_opa(0, 0)
        win.set_style_radius(0, 0)
        win.set_style_bg_color(lv.color_black(), 0)
        win.clear_flag(lv.obj.FLAG.SCROLLABLE)

        # --------- value container ---------
        col_dsc = [60, 65, 40, 60, lv.GRID_TEMPLATE.LAST]
        row_dsc = [48, lv.GRID_TEMPLATE.LAST]
        valueLayout = lv.obj(win)
        valueLayout.set_layout(lv.LAYOUT_GRID.value)
        valueLayout.set_style_bg_opa(0, 0)
        valueLayout.set_style_border_opa(0, 0)
        valueLayout.set_size(lv.SIZE.CONTENT, lv.SIZE.CONTENT)
        valueLayout.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)
        valueLayout.set_style_grid_column_dsc_array(col_dsc, 0)
        valueLayout.set_style_grid_row_dsc_array(row_dsc, 0)
        valueLayout.set_style_pad_top(20, 0)
        valueLayout.set_style_align(lv.ALIGN.TOP_MID, 0)
        # -----------  -  --------------
        subImage = lv.img(valueLayout)
        subImage.set_src(RESOURCES_ROOT + "images/subtraction.png")
        subImage.set_grid_cell(lv.GRID_ALIGN.START, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        subImage.add_flag(lv.obj.FLAG.CLICKABLE)
        subImage.add_event_cb(lambda e: sub_pressed_cb(e, self), lv.EVENT.PRESSED, None)
        # subImage.add_event_cb(lambda e: hvac_back_press_callback(e, subImage), lv.EVENT.PRESSED, None)
        # subImage.add_event_cb(lambda e: hvac_back_release_callback(e, subImage), lv.EVENT.RELEASED, None)
        subImage.set_ext_click_area(40)

        # ----------- value -----------
        self.label = lv.label(valueLayout)
        self.label.set_text(str(self.value))
        self.label.set_style_text_color(lv.color_white(), 0)
        self.label.set_style_text_font(lv.font_montserrat_48, 0)
        self.label.set_grid_cell(lv.GRID_ALIGN.CENTER, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        # ----------- ºC ------------
        centigradeImage = lv.img(valueLayout)
        centigradeImage.set_src(RESOURCES_ROOT + "images/centigrade_s.png")
        centigradeImage.set_style_pad_bottom(8, 0)
        centigradeImage.set_grid_cell(lv.GRID_ALIGN.START, 2, 1, lv.GRID_ALIGN.END, 0, 1)

        # ----------- + ----------------
        addImage = lv.img(valueLayout)
        addImage.set_src(RESOURCES_ROOT + "images/addition.png")
        addImage.set_grid_cell(lv.GRID_ALIGN.CENTER, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        addImage.add_flag(lv.obj.FLAG.CLICKABLE)
        addImage.add_event_cb(lambda e: add_pressed_cb(e, self), lv.EVENT.PRESSED, None)
        # addImage.add_event_cb(lambda e: hvac_back_press_callback(e, addImage), lv.EVENT.PRESSED, None)
        # addImage.add_event_cb(lambda e: hvac_back_release_callback(e, addImage), lv.EVENT.RELEASED, None)
        addImage.set_ext_click_area(40)

        # ----------- tips ----------
        tips = lv.label(win)
        tips.set_text("Temperature")
        tips.set_style_text_color(lv.color_make(0xCC, 0xCC, 0xCC), 0)
        tips.set_style_pad_bottom(20, 0)
        tips.set_align(lv.ALIGN.CENTER)

        # ----------- function ----------
        func_col_dsc = [70, 70, 70, 70, lv.GRID_TEMPLATE.LAST]
        func_row_dsc = [40, lv.GRID_TEMPLATE.LAST]

        funcContainer = lv.obj(win)
        funcContainer.set_style_bg_opa(0, 0)
        funcContainer.set_style_border_opa(0, 0)
        funcContainer.set_grid_dsc_array(func_col_dsc, func_row_dsc)
        funcContainer.set_width(300)
        funcContainer.set_height(90)
        funcContainer.set_layout(lv.LAYOUT_GRID.value)
        funcContainer.set_align(lv.ALIGN.BOTTOM_MID)
        funcContainer.set_grid_align(lv.GRID_ALIGN.SPACE_BETWEEN, lv.GRID_ALIGN.SPACE_BETWEEN)

        image = lv.img(funcContainer)
        image.set_src(functionImage[0])
        image.add_flag(lv.obj.FLAG.CLICKABLE)
        image.set_ext_click_area(20)
        image.add_event_cb(lambda e: func_pressed_cb(e, 0), lv.EVENT.PRESSED, None)
        image.set_grid_cell(lv.GRID_ALIGN.CENTER, 0, 1, lv.GRID_ALIGN.CENTER, 0, 1)
        currentSelected = image
        currentSelected.set_src(functionImageSelected[0])

        image1 = lv.img(funcContainer)
        image1.set_src(functionImage[1])
        image1.add_flag(lv.obj.FLAG.CLICKABLE)
        image1.set_ext_click_area(20)
        image1.add_event_cb(lambda e: func_pressed_cb(e, 1), lv.EVENT.PRESSED, None)
        image1.set_grid_cell(lv.GRID_ALIGN.CENTER, 1, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        image2 = lv.img(funcContainer)
        image2.set_src(functionImage[2])
        image2.add_flag(lv.obj.FLAG.CLICKABLE)
        image2.set_ext_click_area(20)
        image2.add_event_cb(lambda e: func_pressed_cb(e, 2), lv.EVENT.PRESSED, None)
        image2.set_grid_cell(lv.GRID_ALIGN.CENTER, 2, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        image3 = lv.img(funcContainer)
        image3.set_src(functionImage[3])
        image3.add_flag(lv.obj.FLAG.CLICKABLE)
        image3.set_ext_click_area(20)
        image3.add_event_cb(lambda e: func_pressed_cb(e, 3), lv.EVENT.PRESSED, None)
        image3.set_grid_cell(lv.GRID_ALIGN.CENTER, 3, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        # for i in range(4):
        #     image = lv.img(funcContainer)
        #     image.set_src(functionImage[i])
        #     image.add_flag(lv.obj.FLAG.CLICKABLE)
        #     image.set_ext_click_area(20)
        #     image.add_event_cb(lambda e: func_pressed_cb(e, i), lv.EVENT.PRESSED, None)
        #     image.set_grid_cell(lv.GRID_ALIGN.CENTER, i, 1, lv.GRID_ALIGN.CENTER, 0, 1)

        #     if (currentFunc == i):
        #         currentSelected = image
        #         currentSelected.set_src(functionImageSelected[i])

        backImg=lv.img(win)
        backImg.set_src(RESOURCES_ROOT + "images/back.png")
        backImg.set_style_align(lv.ALIGN.LEFT_MID, 0)
        backImg.add_flag(lv.obj.FLAG.CLICKABLE)
        backImg.add_event_cb(lambda e: hvac_back_click_callback(e, win), lv.EVENT.CLICKED, None)
        backImg.add_event_cb(lambda e: hvac_back_press_callback(e, backImg), lv.EVENT.PRESSED, None)
        backImg.add_event_cb(lambda e: hvac_back_release_callback(e, backImg), lv.EVENT.RELEASED, None)
        backImg.set_ext_click_area(20)

        from smart_panel import needAnimation
        if (needAnimation):
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.MOVE_LEFT, 500, 0, True)
        else:
            lv.scr_load_anim(scr, lv.SCR_LOAD_ANIM.NONE, 0, 0, True)

        hvac_alive = True
