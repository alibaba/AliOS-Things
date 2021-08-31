# coding=utf-8
# This is a sample Python script.

import utime
from video import camera

print("start photo save test")

# 打开摄像头
camera = camera()
camera.open(0)

# 拍照并保存图片到SD卡，此处拍摄10张图片，保存格式为JPEG
for i in range(10):
    frame = camera.capture()
    camera.save(frame, camera.JPEG, 'photo')
    utime.sleep_ms(1000)
    print("save photo %d to file prefix %s" % (i, 'photo'))

# 关闭摄像头
camera.close()

print("end photo save test")
