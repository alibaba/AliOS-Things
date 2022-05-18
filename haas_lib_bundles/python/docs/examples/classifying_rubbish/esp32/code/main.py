#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    垃圾分类识别案例
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
from cloudAI import *
import irdistance
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
gName = ''

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
    global detected, gName, gStartRecognize
    detected = False
    gName = 'NA'
    if commandReply == 'classifyingRubbishReply' :
        if result != 'NA' :
            gName = result
            detected = True
    else :
        print('unknown command reply')
    # 识别结束,复位识别标识符
    gStartRecognize = False
    print('识别结束')

# 红外检测线程
def infraRed_thread():
    global gStartRecognize, gIrDev
    print('启动红外检测线程')
    while True:
        if gStartRecognize == False :
            status = gIrDev.objectDetection()
            if status == 0:
                gStartRecognize = True
                print('有物体进入')
        utime.sleep_ms(50)

# 识别线程函数
def recognizeThread():
    global gFrame
    while True:
        if gFrame != None and gStartRecognize == True:
            engine.classifyRubbish(gFrame)
            utime.sleep_ms(1000)
        else:
            utime.sleep_ms(1000)

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, gFrame, detected, gName
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
                disp.text(20, 60, gName, disp.RED)
                disp.text(20, 100, 'Deteted!', disp.RED)
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
    global disp, gFrame, detected, engine, gStartRecognize, gIrDev
    # 创建lcd display对象
    disp = display.TFT()
    gFrame = None
    detected = False
    gStartRecognize = False

    # 连接网络
    connect_wifi(SSID, PWD)

    engine = CloudAI(key_info, recognize_cb)

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    # 初始化红外传感器
    gpioDev = GPIO()
    gpioDev.open("ir")
    gIrDev = irdistance.IRDISTANCE(gpioDev)
    print("IR detector ...")

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
        # 启动红外检测线程
        _thread.start_new_thread(infraRed_thread, ())
        # 设置比对线程stack
        _thread.stack_size(16 * 1024)
        # 启动识别线程
        _thread.start_new_thread(recognizeThread, ())
    except:
       print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
