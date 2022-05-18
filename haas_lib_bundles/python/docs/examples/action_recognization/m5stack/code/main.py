#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    人体动作行为识别案例
@Author     :    jinouxiang
@version    :    2.0
'''

import display      # 显示库
import uai          # AI识别库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import _thread      # 线程库
import sntp         # 网络时间同步库
from cloudAI import *

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'

# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

detected = False
action = ''

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

action_dic = {
    '举手' : 'lift hand',
    '吃喝': 'eat and drink',
    '吸烟': 'smoking',
    '打电话': 'phone call',
    '玩手机': 'play cell-phone',
    '趴桌睡觉' : 'sleep',
    '跌倒': 'fall down',
    '洗手': 'wash hand',
    '拍照': 'take picture'
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

def action_cb(commandReply, result) :
    global detected, action
    action = 'NA'
    detected = False
    if commandReply == 'recognizeActionReply' :
        if result != 'NA' :
            action = action_dic[result]
            print('detect action: ' + action)
            detected = True
    else :
        print('unknown command reply')

# 识别线程函数
def recognizeThread():
    global frame
    while True:
        if frame != None:
            engine.recognizeAction(frame)
            #engine.recognizeGesture(frame)
            utime.sleep_ms(6000)
        else:
            utime.sleep_ms(1000)

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, frame, detected, action
    # 定义清屏局部变量
    clearFlag = False
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        # 采集摄像头画面
        # print('start to capture')
        frame = ucamera.capture()
        # print('end to capture')
        if frame != None:
            if detected == True:
                # 清除屏幕内容
                disp.clear()
                # 设置文字字体
                disp.font(disp.FONT_DejaVu40)
                # 显示识别结果
                disp.text(10, 50, 'detect: ' + action, disp.RED)
                #disp.text(10, 50, 'detect pedestrain', disp.RED)
                utime.sleep_ms(1000)
                textShowFlag = False
            else:
                # 显示图像
                # print('start to display')
                disp.image(0, 20, frame, 0)
                utime.sleep_ms(100)
                if textShowFlag == False:
                    # 设置显示字体
                    disp.font(disp.FONT_DejaVu18)
                    # 显示文字
                    disp.text(2, 0, 'Recognizing...', disp.WHITE)
                    textShowFlag = True


def main():
    # 全局变量
    global disp, frame, detected, action, engine
    # 创建lcd display对象
    disp = display.TFT()
    frame = None
    detected = False

    # 连接网络
    connect_wifi(SSID, PWD)

    engine = CloudAI(key_info, action_cb)

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
        # 设置比对线程stack
        _thread.stack_size(20 * 1024)
        # 启动比对线程
        _thread.start_new_thread(recognizeThread, ())
    except:
       print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
