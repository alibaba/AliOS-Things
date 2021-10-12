#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    time_record.py
@Description:    file description   
@Date       :    2021/08/12 13:55:47
@Author     :    guoliang.wgl
@version    :    1.0
'''
import utime as time

class TimeRecorder(object):

    def __init__(self, timeout_ms):
        self.start_time = time.ticks_ms()
        self.timeout_ms = timeout_ms

    def is_timeout(self):
        return time.ticks_diff(time.ticks_ms(),self.start_time) > self.timeout_ms
