"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for CHT8305

    Author: HaaS
    Date:   2022/05/11
"""

import lvgl as lv
import lvgl_display

print("display_driver init")
if not lv.is_initialized():
    #print("lv.init")
    lv.init()

if not lvgl_display.is_initialized():
    #print("lvgl_display.init")
    lvgl_display.init()
