#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    物体识别案例
@Author     :    杭漂
@version    :    1.0
'''

from aliyunIoT import Device
import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import _thread      # 线程库
import ujson as json
from driver import GPIO

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

detected = False
gStartRecognize = False

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    # 引用全局变量
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
            print('IP: %s' %ip)
            disp.text(20, 50, ip, disp.RED)
            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            disp.text(20, 70, 'NTP start...', disp.RED)
            sntp.setTime()
            print('NTP done')
            disp.textClear(20, 70, 'NTP start...')
            disp.text(20, 70, 'NTP done', disp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def recognize_cb(commandReply, result) :
    global detected, object, gStartRecognize
    detected = False
    object = 'NA'

    if commandReply == 'detectObjectReply' :
        if result != 'NA' :
            object = result
            detected = True
    else :
        print('unknown command reply')
    # 识别结束,复位识别标识符
    gStartRecognize = False
    print('识别结束')

# 按键检测
def key_event_thread():
    global gStartRecognize,gpio,gFrame, engine
    print('启动按钮监控线程')
    status = -1
    while True:
        if gStartRecognize == False :
            status = gpio.read()
            # 通过接入GND模拟按钮被按
            if status == 0:
                print('按下拍照按钮')
                if gFrame != None :
                    # 开始识别
                    gStartRecognize = True
                    engine.detectObject(gFrame)
                    utime.sleep_ms(1000)
            elif status < 0 :
                gStartRecognize == False
        utime.sleep_ms(1000)

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, gFrame, detected, num
    # 定义清屏局部变量
    clearFlag = False
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        # 采集摄像头画面
        # print('start to capture')
        gFrame = ucamera.capture()
        # print('end to capture')
        if gFrame != None:
            if detected == True:
                # 清除屏幕内容
                disp.clear()
                # 设置文字字体
                disp.font(disp.FONT_DejaVu40)
                # 显示识别结果
                disp.text(20, 100, 'Object Deteted!', disp.RED)
                utime.sleep_ms(1000)
                textShowFlag = False
                detected = False
            else:
                # 显示图像
                # print('start to display')
                disp.image(0, 20, gFrame, 0)
                utime.sleep_ms(100)
                if textShowFlag == False:
                    # 设置显示字体
                    disp.font(disp.FONT_DejaVu18)
                    # 显示文字
                    disp.text(2, 0, 'Recognizing...', disp.WHITE)
                    textShowFlag = True

def main():
    # 全局变量
    global disp, detected, engine, gStartRecognize, gpio
    # 创建lcd display对象
    disp = display.TFT()
    frame = None
    detected = False

    # 连接网络
    connect_wifi(SSID, PWD)

    engine = CloudAI(key_info, recognize_cb)

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    # 初始化拍照按钮
    gpio = GPIO()
    gpio.open("photoButton")

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
        # 设置比对线程stack
        _thread.stack_size(20 * 1024)
        # 启动按键监控线程
        _thread.start_new_thread(key_event_thread, ())
    except:
       print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
