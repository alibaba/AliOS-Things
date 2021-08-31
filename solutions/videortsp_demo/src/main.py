# coding=utf-8
# This is a sample Python script.

import usys
import utime
import network
from video import Recorder

# 检查参数
if len(usys.argv) < 3:
    print("Usage: %s <ssid> <password>" % usys.argv[0])
    usys.exit(1)

# 定义网络事件回调函数
def network_evt_cb(eid):
    print('%s:%d' %('eid', eid))

# 连接网络
network.init(network.WIFI)
network.connect(usys.argv[1], usys.argv[2], network_evt_cb)
network.close()

# 等待网络连接
utime.sleep_ms(10000)
print("start recorder with rtsp test")

# 开启视频流功能
recorder = Recorder()
recorder.open(0, recorder.H264)
recorder.start()
utime.sleep_ms(100000000)

# 关闭视频流功能
recorder.stop()
recorder.close()

print("end recorder test")

