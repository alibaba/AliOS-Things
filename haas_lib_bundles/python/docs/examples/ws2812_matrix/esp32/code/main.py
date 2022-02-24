# -*- coding:gbk -*-

import random                   # 随机数生成库
import ujson                    # json字串解析库
import utime                    # 延时API所在组件
import network                  # Wi-Fi功能所在库
from aliyunIoT import Device    # 连接阿里云物联网平台的组件
from machine import Pin         # 引脚控制
from neoPixelMatrix import NeoPixelMatrix   # ws2812点阵显示组件
import framebuf                 # 帧缓冲区组件
import sntp                     # 获取时间

# 屏幕显示模式
screenMode = 0
# 屏幕显示亮度
screenBright = 255
# 文字状态下显示内容
screenText = '福'

# 设置字体库存放路径
framebuf.set_font_path(framebuf.FONT_HZK12, '/data/pyamp/font/HZK12')
framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/pyamp/font/ASC12_8')

# 创建点阵屏幕对象
screen = NeoPixelMatrix(Pin(19, Pin.OUT), 16, 16, 'w')

# 清空点阵缓冲区
screen.fill(0x000000)

# 物联网平台连接标志位
iot_connected = False

# 物联网设备实例
device = None

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    wlan = network.WLAN(network.STA_IF)
    wifi_connected = False

    wlan.active(True)  # 激活界面
    wlan.disconnect()
    #print("start to connect ", wifiSsid)
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()  # 获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected, screenBright
    iot_connected = True

    upload_data = {'params': ujson.dumps({
        'brightness':screenBright
    })}

    # 上报到云端
    device.postProps(upload_data)

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global device,screen,screenMode,screenBright

    # e.g. {"mode":0} or {"mode":0}
    payload = ujson.loads(request['params'])
    print(payload)

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "mode" in payload.keys():
        screenMode = payload["mode"]

    if "brightness" in payload.keys():
        screenBright = payload["brightness"]
        screen.setBrightness(screenBright)

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'mode': screenMode,
        'brightness':screenBright
    })
    upload_data = {'params': prop}

    # 上报到云端
    device.postProps(upload_data)

# 连接物联网平台
def connect_lk(productKey, deviceName, deviceSecret):

    global device, iot_connected

    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    # 将三元组信息设置到iot组件中
    device = Device()

    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)

    # 配置收到云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)

    # 启动连接阿里云物联网平台过程
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            utime.sleep(1)
    print('sleep for 2s')
    utime.sleep(2)

if __name__ == '__main__':

    get_wifi_status()

    sntp.setTime('CST-8', 'cn.pool.ntp.org')

    connect_lk(productKey, deviceName, deviceSecret)

    frame = 0

    while True:

        frame += 1

        if screenMode == 0:
            # 0 - 关闭
            screen.fill(0)
            screen.show()
            utime.sleep(1)

        elif screenMode == 1:
            # 1 - 显示图片
            try:
                f = open('/data/pyamp/img/tiger.bin', 'rb')
                for line in range(0, screen.width):
                    pixel = f.read(screen.width * 3)
                    screen.buffer[line * screen.width * 3: (line + 1) * screen.width * 3] = pixel[:]
                f.close()

            except Exception as e:
                print(e)

            screen.show()

            utime.sleep(1)

        elif screenMode == 2:
            # 2 - 显示动画
            try:
                f = open('/data/pyamp/img/fire_{}.bin'.format(frame%14), 'rb')
                for line in range(0, screen.width):
                    pixel = f.read(screen.width * 3)
                    screen.buffer[line * screen.width * 3: (line + 1) * screen.width * 3] = pixel[:]
                f.close()

            except Exception as e:
                print(e)

            screen.show()

        elif screenMode == 3:
            # 3 - 显示光效
            rb = ((255, 0, 0), (255, 127, 0), (255, 255, 0), (0, 255, 0),
                (0, 255, 255), (0, 0, 255), (136, 0, 255), (255, 0, 0))
            for i in range(screen.width // 2):
                t = 7*i/(screen.width // 2)
                t0 = int(t)
                r = round(rb[t0][0] + (t-t0)*(rb[t0+1][0]-rb[t0][0]))
                g = round(rb[t0][1] + (t-t0)*(rb[t0+1][1]-rb[t0][1]))
                b = round(rb[t0][2] + (t-t0)*(rb[t0+1][2]-rb[t0][2]))
                offset = (i + frame) % (screen.width // 2)
                screen.rect(offset,offset,screen.width-offset*2,screen.height-offset*2,(r << 16 | g << 8 | b))
            screen.show()

        elif screenMode == 4:
            # 4 - 显示文字
            screen.fill(0)
            screen.text(screenText,2,2,0xff0000, size = 12)
            screen.show()

        elif screenMode == 5:
            # 5 - 时钟
            print(utime.localtime())
            screen.fill(0)
            screen.text(str(utime.localtime()[3]),0,0,0xffffff)
            screen.text(str(utime.localtime()[4]),0,8,random.randint(0,0xffffff))
            utime.sleep(1)
            screen.show()

        else:
            # 6 - 自定义
            print("Do something yourself here.")
            utime.sleep(1)
