import lvgl as lv
from driver import I2C
from qmp6988 import QMP6988
from airpressure import Airpressure
import _thread
import utime

RESOURCES_ROOT = "S:/data/pyamp/images/"

# 设备实例
i2cDev = None
qmp6988Dev = None

# 全局变量
pressure_value = 0

def qmp6988_init():
    global i2cDev, qmp6988Dev
    i2cDev = I2C()
    i2cDev.open("qmp6988")
    qmp6988Dev = QMP6988(i2cDev)

def qmp6988_deinit():
    global i2cDev
    i2cDev.close()

def th0():
    global qmp6988Dev, pressure_value
    while True:
        pressure_value = qmp6988Dev.getPressure()
        utime.sleep(2)

def th1():
    global airPressureObj
    while True:
        time = utime.localtime()
        airPressureObj.setDate(time)
        utime.sleep_ms(900)

def update_air_pressure(obj, x, y):
    global pressure_value
    obj.setValue(pressure_value)
    obj.setXY(x, y)

def set_scale(obj, v):
    obj.setScale(v)

def load_page():
    global airPressureObj

    scr = lv.obj()
    scr.align(lv.ALIGN.TOP_LEFT, 0, 0)
    scr.set_style_bg_color(lv.color_black(), 0)
    airPressureObj = Airpressure(scr)

    # 动画
    a1 = lv.anim_t()
    a1.init()
    a1.set_var(airPressureObj)
    a1.set_values(10, 80)
    a1.set_time(10000)
    a1.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
    a1.set_custom_exec_cb(lambda a, val: update_air_pressure(airPressureObj, val, 2*val))
    lv.anim_t.start(a1)

    try:
        th0_id = _thread.start_new_thread(th0, ())
        th1_id = _thread.start_new_thread(th1, ())
    except:
        print("Create new thread failed!")

    lv.scr_load(scr)
