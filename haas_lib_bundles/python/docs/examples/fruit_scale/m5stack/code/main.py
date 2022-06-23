#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
from driver import GPIO
from cloudAI import *
import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import ele_scale    # 引入电子秤驱动库
import _thread      # 线程库

# 全局变量
disp = None
frame = None
detected = False
fruit = dict()

weight_prev = 0.0

fruit_name = None
fruit_weight = 0.0
fruit_price = 0.0

# 价格表
unit_price = {
    "苹果":{"Apple": 5.0},
    "香蕉":{"Banana": 3.0},
    "橙子":{"Orange": 2.0},
    "桃":{"Peach": 3.0}
}

# 设备实例
clkDev = None
dataDev = None
scaleDev = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

# 三元组信息
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    global disp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 显示网络连接中
        disp.text(20, 30, 'Wi-Fi is connecting...', disp.RED)
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            disp.textClear(20, 30, 'Wi-Fi is connecting...')
            disp.text(20, 30, 'Wi-Fi is connected', disp.RED)
            ip = wlan.ifconfig()[0]
            disp.text(20, 50, ip, disp.RED)
            print('IP:', ip)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

# NTP时间更新，如果更新不成功，将不能进行识别
def ntp_update():
    global disp

    print('NTP update start...')
    disp.text(20, 70, 'NTP update start...', disp.RED)
    sntp.setTime()
    disp.textClear(20, 70, 'NTP update start...')
    disp.text(20, 70, 'NTP update done', disp.RED)
    print('NTP update done')

# lcd init
def lcd_init():
    global disp
    disp = display.TFT()
    disp.clear()
    disp.font(disp.FONT_DejaVu18)

# 初始化摄像头
def ucamera_init():
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

# 初始化电子秤设备
def scale_init():
    global clkDev, dataDev, scaleDev
    clkDev = GPIO()
    clkDev.open("hx711_clk")
    dataDev = GPIO()
    dataDev.open("hx711_data")
    scaleDev = ele_scale.EleScale(clkDev, dataDev, 430.0)

# AI识别完成回调函数
def recognize_cb(cmd, result) :
    global detected, unit_price, fruit
    detected = False
    if (cmd == 'detectFruitsReply'):
        if result != 'NA' :
            fruit = unit_price[result]
            detected = True
    else:
        print('unknown command reply')

# LCD显示线程
def display_th():
    global disp, frame, detected, fruit, fruit_name, fruit_weight, fruit_price

    while True:
        if frame != None:
            if detected == True:
                fruit_name = list(fruit.keys())[0]
                # 水果价格：xxx元/斤
                fruit_price = list(fruit.values())[0] * fruit_weight / 500.0
                display_msg0 = "{}: {:.1f}g".format(fruit_name, fruit_weight)
                display_msg1 = "price: {:.1f}".format(fruit_price)

                disp.clear()
                disp.text(40, 80, display_msg0, disp.RED)
                disp.text(40, 100, display_msg1, disp.RED)
                utime.sleep(10)
            else:
                disp.image(0, 0, frame, 0)

        utime.sleep_ms(100)

def main():
    global key_info, frame, detected, wifiSsid, wifiPassword
    global scaleDev, weight_prev, fruit_weight

    lcd_init()
    connect_wifi(wifiSsid, wifiPassword)
    ntp_update()

    # 实例化AI引擎
    ai_engine = CloudAI(key_info, recognize_cb)

    ucamera_init()
    scale_init()

    try:
        # 启动显示线程
        _thread.start_new_thread(display_th, ())
    except:
       print("Error: unable to start thread")

    while True:
        frame = ucamera.capture()
        weight_cur = scaleDev.getWeight()
        print("weight_cur:%.1f"%weight_cur)
        if ((weight_prev < 50) and (weight_cur >= 50)):
            weight_prev = weight_cur
            fruit_weight = weight_cur
            print("fruit_weight:%.1f"%weight_cur)
            utime.sleep(1)
            if (frame):
                ai_engine.detectFruits(frame)
        elif ((weight_prev >= 50) and (weight_cur < 50)):
            weight_prev = weight_cur
            detected = False

        utime.sleep_ms(100)

if __name__ == '__main__':
    main()
