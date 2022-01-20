import sys
sys.path.append('') # See: https://github.com/micropython/micropython/issues/6419

import lvgl as lv
lv.init()
# lv.log_register_print_cb(lambda msg: print(msg))

# Initialize ILI9341 display

import espidf as esp
from ili9XXX import ili9341,COLOR_MODE_BGR,LANDSCAPE,PORTRAIT
    
disp = ili9341(dc=32, cs=33, power=-1, backlight=-1)

# The call below is for a Lolin TFT-2.4 board
# disp = ili9341(miso=19,mosi=23,clk=18, cs=26, dc=5, rst=-1, power=-1, backlight=-1, backlight_on=0, power_on=0,
#               spihost=esp.VSPI_HOST, mhz=40, factor=4, hybrid=True, width=240, height=320,
#               colormode=COLOR_MODE_BGR, rot=PORTRAIT, invert=False, double_buffer=True, half_duplex=True)

# In order to calibrate in landscape mode please use:
# disp = ili9341(rot=LANDSCAPE,width=320,height=240))

HRES =  lv.scr_act().get_disp().driver.hor_res
VRES =  lv.scr_act().get_disp().driver.ver_res

# Register xpt touch driver
from xpt2046 import xpt2046
touch = xpt2046(cal_x0=0, cal_x1 = HRES, cal_y0=0, cal_y1 = VRES)

# The call below is for a Lolin TFT-2.4 board
# touch = xpt2046(spihost=esp.VSPI_HOST,cal_x0=0, cal_x1 = HRES, cal_y0=0, cal_y1 = VRES, transpose=True)

# In order to calibrate in landscape mode please use:
# touch = xpt2046(cal_x0=0, cal_x1 = HRES, cal_y0=0, cal_y1 = VRES,transpose=False)

# Point class, with both display and touch coordiantes

class Tpcal_point():
    def __init__(self, x, y, name):
        self.display_coordinates = lv.point_t({'x': x, 'y': y})
        self.touch_coordinate = None
        self.name = name

    def __repr__(self):
        return "%s: (%d, %d)" % (self.name, 
                self.touch_coordinate.x,
                self.touch_coordinate.y)

# Calibration helper class

class Tpcal():
        
    # Create a screen with a button and a label

    CIRCLE_SIZE = const(20)
    CIRCLE_OFFSET = const(20)
    TP_MAX_VALUE = const(10000)

    LV_COORD_MAX = const((1 << (8 * 2 - 1)) - 1000)
    LV_RADIUS_CIRCLE = const(LV_COORD_MAX) # TODO use lv.RADIUS_CIRCLE constant when it's available!

    def __init__(self, points, calibrate, touch_count = 5):

        self.points = points
        self.calibrate = calibrate
        self.touch_count = touch_count

        self.med = [lv.point_t() for i in range(0,self.touch_count)] # Storage point to calculate median

        self.cur_point = 0
        self.cur_touch = 0

        self.scr = lv.obj(None)
        self.scr.set_size(TP_MAX_VALUE, TP_MAX_VALUE)
        lv.scr_load(self.scr)

        # Create a big transparent button screen to receive clicks
        style_transp = lv.style_t()
        style_transp.init()
        style_transp.set_bg_opa(lv.OPA.TRANSP)
        self.big_btn = lv.btn(lv.scr_act())
        self.big_btn.set_size(TP_MAX_VALUE, TP_MAX_VALUE)
        self.big_btn.add_style(style_transp, lv.PART.MAIN)
        self.big_btn.add_style(style_transp, lv.PART.MAIN)
        #self.big_btn.set_layout(lv.LAYOUT.OFF)
        self.big_btn.add_event_cb(lambda event, self=self: self.calibrate_clicked(),lv.EVENT.CLICKED, None) 

        # Create the screen, circle and label

        self.label_main = lv.label(lv.scr_act())

        style_circ = lv.style_t()
        style_circ.init()
        style_circ.set_radius(LV_RADIUS_CIRCLE)
                              
        self.circ_area = lv.obj(lv.scr_act())
        self.circ_area.set_size(CIRCLE_SIZE, CIRCLE_SIZE)
        self.circ_area.add_style(style_circ, lv.STATE.DEFAULT)
        self.circ_area.clear_flag(lv.obj.FLAG.CLICKABLE) # self.circ_area.set_click(False)

        self.show_circle()

    def show_text(self, txt):
        self.label_main.set_text(txt)
        # self.label_main.set_align(lv.label.ALIGN.CENTER)
        self.label_main.set_pos((HRES - self.label_main.get_width() ) // 2,
                           (VRES - self.label_main.get_height()) // 2)
    def show_circle(self):
        point = self.points[self.cur_point]
        self.show_text("Click the circle in\n" + \
              point.name + "\n" + \
              "%d left" % (self.touch_count - self.cur_touch))
        if point.display_coordinates.x < 0: point.display_coordinates.x += HRES
        if point.display_coordinates.y < 0: point.display_coordinates.y += VRES
        # print("Circle coordinates: x: %d, y: %d"%(point.display_coordinates.x - self.CIRCLE_SIZE // 2,
        #                                           point.display_coordinates.y - self.CIRCLE_SIZE // 2))
        self.circ_area.set_pos(point.display_coordinates.x - CIRCLE_SIZE // 2,
                               point.display_coordinates.y - CIRCLE_SIZE // 2)

    def calibrate_clicked(self):
        point = self.points[self.cur_point]
        indev = lv.indev_get_act()
        indev.get_point(self.med[self.cur_touch])
        # print("calibrate_clicked: x: %d, y: %d"%(self.med[self.cur_touch].x,self.med[self.cur_touch].y))        
        self.cur_touch += 1

        if self.cur_touch == self.touch_count:
            med_x = sorted([med.x for med in self.med])
            med_y = sorted([med.y for med in self.med])
            x = med_x[len(med_x) // 2]
            y = med_y[len(med_y) // 2]
            point.touch_coordinate = lv.point_t({
                'x': x,
                'y': y})

            self.cur_point += 1
            self.cur_touch = 0

        if self.cur_point == len(self.points):
            self.calibrate(self.points)
            self.cur_point = 0
            self.show_text("Click/drag on screen\n" + \
                           "to check calibration")
            self.big_btn.set_event_cb(lambda event, self=self: self.check(), lv.EVENT.PRESSING, None) 
        else:
            self.show_circle()

    def check(self):
        point = lv.point_t()
        indev = lv.indev_get_act()
        indev.get_point(point)
        # print("click position: x: %d, y: %d"%(point.x,point.y))
        self.circ_area.set_pos(point.x - CIRCLE_SIZE // 2,
                               point.y - CIRCLE_SIZE // 2)

# Calculate calibration, and calibrate

def calibrate(points):
    visual_width = points[1].display_coordinates.x - points[0].display_coordinates.x
    visual_height = points[1].display_coordinates.y - points[0].display_coordinates.y
    touch_width = points[1].touch_coordinate.x - points[0].touch_coordinate.x
    touch_height = points[1].touch_coordinate.y - points[0].touch_coordinate.y

    pixel_width = touch_width / visual_width
    pixel_height = touch_height / visual_height

    x0 = points[0].touch_coordinate.x - points[0].display_coordinates.x * pixel_width
    y0 = points[0].touch_coordinate.y - points[0].display_coordinates.y * pixel_height

    x1 = points[1].touch_coordinate.x + (HRES - points[1].display_coordinates.x) * pixel_width
    y1 = points[1].touch_coordinate.y + (VRES - points[1].display_coordinates.y) * pixel_height

    print("Calibration result: x0=%d, y0=%d, x1=%d, y1=%d" % (round(x0), round(y0), round(x1), round(y1)))
    touch.calibrate(round(x0), round(y0), round(x1), round(y1))
    
# Run calibration

tpcal = Tpcal([
        Tpcal_point(20,  20, "upper left-hand corner"),
        Tpcal_point(-40, -40, "lower right-hand corner"),
    ], calibrate)

# while True:
#     pass
