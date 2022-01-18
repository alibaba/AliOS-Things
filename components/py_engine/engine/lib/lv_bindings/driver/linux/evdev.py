# LVGL indev driver for evdev mouse device
# (for the unix micropython port)

import ustruct
import select
import lvgl as lv

# Default crosshair cursor
class crosshair_cursor:
    def __init__(self, scr=None):
        self.scr = scr if scr else lv.scr_act()
        self.hor_res = self.scr.get_width()
        self.ver_res = self.scr.get_height()
        self.cursor_style = lv.style_t()
        self.cursor_style.set_line_width(1)
        self.cursor_style.set_line_dash_gap(5)
        self.cursor_style.set_line_dash_width(1)
        self.cursor_hor = lv.line(self.scr)
        self.cursor_hor.add_style(self.cursor_style, lv.PART.MAIN)
        self.cursor_ver = lv.line(self.scr)
        self.cursor_ver.add_style(self.cursor_style, lv.PART.MAIN)

    def __call__(self, data):
        # print("%d : %d:%d" % (data.state, data.point.x, data.point.y))
        self.cursor_hor.set_points([{'x':0,'y':data.point.y},{'x':self.hor_res,'y':data.point.y}],2)
        self.cursor_ver.set_points([{'y':0,'x':data.point.x},{'y':self.ver_res,'x':data.point.x}],2)

    def delete(self):
        self.cursor_hor.delete()
        self.cursor_ver.delete()

# evdev driver for mouse
class mouse_indev:
    def __init__(self, scr=None, cursor=None, device='/dev/input/mice'):

        # Open evdev and initialize members
        self.evdev = open(device, 'rb')
        self.poll = select.poll()
        self.poll.register(self.evdev.fileno())
        self.scr = scr if scr else lv.scr_act()
        self.cursor = cursor if cursor else crosshair_cursor(self.scr)
        self.hor_res = self.scr.get_width()
        self.ver_res = self.scr.get_height()

        # Register LVGL indev driver
        self.indev_drv = lv.indev_drv_t()
        self.indev_drv.init()
        self.indev_drv.type = lv.INDEV_TYPE.POINTER
        self.indev_drv.read_cb = self.mouse_read
        self.indev = self.indev_drv.register()

    def mouse_read(self, indev_drv, data) -> int:
        
        # Check if there is input to be read from evdev
        if not self.poll.poll()[0][1] & select.POLLIN:
            return 0

        # Read and parse evdev mouse data
        mouse_data = ustruct.unpack('bbb',self.evdev.read(3))

        # Data is relative, update coordinates
        data.point.x += mouse_data[1]
        data.point.y -= mouse_data[2]

        # Handle coordinate overflow cases
        data.point.x = min(data.point.x, self.hor_res - 1)
        data.point.y = min(data.point.y, self.ver_res - 1)
        data.point.x = max(data.point.x, 0)
        data.point.y = max(data.point.y, 0)

        # Update "pressed" status
        data.state = lv.INDEV_STATE.PRESSED if ((mouse_data[0] & 1) == 1) else lv.INDEV_STATE.RELEASED

        # Draw cursor, if needed
        if self.cursor: self.cursor(data)
        return 0

    def delete(self):
        self.evdev.close()
        if self.cursor and hasattr(self.cursor, 'delete'):
            self.cursor.delete()
        self.indev.enable(False)
