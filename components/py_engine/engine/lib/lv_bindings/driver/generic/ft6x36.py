# Pure Python LVGL indev driver for the FocalTech FT6X36 capacitive touch IC
#
#       from ft6x36 import ft6x36 
# 
#       touch = ft6x36(sda=<pin>, scl=<pin>)
#
# If you set the size of the touchpad, you have the option to invert each 
# axis, and you'll get some extra robustness against occasional read errors
# as values outside the touchpad are quietly rejected. If you select to swap
# the axes, width and height as well as the inversions refer to the situation
# before the swap.
#
# The nice thing about this driver is that it allows access to the second
# finger, as the FT6X36 is multi-touch. (Two fingers max, with caveats on
# some boards.)
#
# The number of presses is in touch.presses, touch.points[0] and points[1]
# hold the positions. LVGL is not (yet) multi-touch, so all it sees is the
# position in points[0].


import lvgl as lv
from machine import I2C, Pin

class ft6x36:

    def __init__(self, i2c_dev=0, sda=21, scl=22, freq=400000, addr=0x38, width=-1, height=-1, 
                 inv_x=False, inv_y=False, swap_xy=False):

        if not lv.is_initialized():
            lv.init()

        self.width, self.height = width, height
        self.inv_x, self.inv_y, self.swap_xy = inv_x, inv_y, swap_xy
        self.i2c = I2C(i2c_dev, sda=Pin(sda), scl=Pin(scl), freq=freq)
        self.addr = addr
        try:
            print("FT6X36 touch IC ready (fw id 0x{0:X} rel {1:d}, lib {2:X})".format( \
                int.from_bytes(self.i2c.readfrom_mem(self.addr, 0xA6, 1), "big"), \
                int.from_bytes(self.i2c.readfrom_mem(self.addr, 0xAF, 1), "big"), \
                int.from_bytes(self.i2c.readfrom_mem(self.addr, 0xA1, 2), "big") \
            ))
        except:
            print("FT6X36 touch IC not responding")
            return
        self.point = lv.point_t( {'x': 0, 'y': 0} )
        self.points = [lv.point_t( {'x': 0, 'y': 0} ), lv.point_t( {'x': 0, 'y': 0} )]
        self.state = lv.INDEV_STATE.RELEASED
        self.indev_drv = lv.indev_drv_t()
        self.indev_drv.init()
        self.indev_drv.type = lv.INDEV_TYPE.POINTER
        self.indev_drv.read_cb = self.callback
        self.indev_drv.register()

    def callback(self, driver, data):

        def get_point(offset):
            x = (sensorbytes[offset    ] << 8 | sensorbytes[offset + 1]) & 0x0fff
            y = (sensorbytes[offset + 2] << 8 | sensorbytes[offset + 3]) & 0x0fff
            if (self.width != -1 and x >= self.width) or (self.height != -1 and y >= self.height):
                raise ValueError
            x = self.width - x - 1 if self.inv_x else x
            y = self.height - y - 1 if self.inv_y else y
            (x, y) = (y, x) if self.swap_xy else (x, y)
            return { 'x': x, 'y': y }

        data.point = self.points[0]
        data.state = self.state
        sensorbytes = self.i2c.readfrom_mem(self.addr, 2, 11)
        self.presses = sensorbytes[0]
        if self.presses > 2:
            return False
        try:
            if self.presses:
                self.points[0] = get_point(1)
            if self.presses == 2:
                self.points[1] = get_point(7)
        except ValueError:
            return False
        if sensorbytes[3] >> 4:
            self.points[0], self.points[1] = self.points[1], self.points[0]
        data.point = self.points[0]
        data.state = self.state = lv.INDEV_STATE.PRESSED if self.presses else lv.INDEV_STATE.RELEASED
        return False
