#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    helloworld案例 - 周期性打印"helloworld"到console中
@Author     :    ethan.lcz
@version    :    1.0
'''

# import utime   # 延时函数在utime库中

# if __name__ == '__main__':
#     while True:             # 无限循环
#         print("hellworld")  # 打印"helloworld"字串到console中
#         utime.sleep(1)      # 打印完之后休眠1秒

from axp192 import *
if __name__ == '__main__':
    axp = Axp192()
    # axp.powerAll()
    # axp.setLCDBrightness(80) # 设置背光亮度 0～100
    axp.setSpkEnable(1) # 打开speaker

# from smart_panel import *

import smart_panel

smart_panel.init()
smart_panel.load_smart_panel()
