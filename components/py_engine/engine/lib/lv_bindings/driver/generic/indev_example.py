# This is the minimal pure-python lvgl pointer-type driver. It does not read
# any hardware and instead just presses at location (50,100)
#
# In a real driver, always return the last position in data.point, even if
# the data.state goes to 'released' (lv.INDEV_STATE.RELEASED), so maybe keep
# that in self.point.
#
# The 'return False' just means there is no further data to read, which is
# what you want for a pointer-type driver.
#
# To use your driver (after you've made it read some real hardware):
#
#       from my_indev import my_indev
#       touch = my_indev()


import lvgl as lv

class my_indev:

    def __init__(self):
        self.indev_drv = lv.indev_drv_t()
        self.indev_drv.init()
        self.indev_drv.type = lv.INDEV_TYPE.POINTER
        self.indev_drv.read_cb = self.callback
        self.indev_drv.register()

    def callback(self, driverptr, dataptr):
        # This is where you need to get the actual position from
        # the hardware. This just simulates pressed at 50, 100.
        data.point = lv.point_t( {'x': 50, 'y': 100} )
        data.state = lv.INDEV_STATE.PRESSED
        return False
