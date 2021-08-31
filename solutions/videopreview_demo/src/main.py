# coding=utf-8
# This is a sample Python script.

import utime
from video import camera

print("start video preview test")

# 打开摄像头
camera = camera()
camera.open(0)

# 开启视频采集和预览功能，持续10000秒
camera.preview(10000)

# 关闭摄像头
camera.close()

print("end video preview test")
