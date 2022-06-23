#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
import utime
import page_airpressure

if __name__ == '__main__':
    # 初始化气压计
    page_airpressure.qmp6988_init()
    import display_driver

    # 显示气压值
    page_airpressure.load_page()

    while True:
        utime.sleep(2)
