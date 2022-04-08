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

gLpConnect = False
gLpService = False
gScore = 0
gDebugLogFlag = True

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    # 引用全局变量
    global gDisp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 显示网络连接中
        gDisp.text(20, 30, 'Wi-Fi is connecting...', gDisp.RED)
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            gDisp.textClear(20, 30, 'Wi-Fi is connecting...')
            gDisp.text(20, 30, 'Wi-Fi is connected', gDisp.RED)
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            gDisp.text(20, 50, ip, gDisp.RED)
            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            gDisp.text(20, 70, 'NTP start...', gDisp.RED)
            sntp.setTime()
            print('NTP done')
            gDisp.textClear(20, 70, 'NTP start...')
            gDisp.text(20, 70, 'NTP done', gDisp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_service(data):
    global gLpService, gScore, gDetected, gName,gStartRecognize
    if data != None:
        params = data['params']
        paramsDict = json.loads(params)
        ext = paramsDict['ext']
        extDict = json.loads(ext)
        result = extDict['result']
        gDetected = False
        if gDebugLogFlag == True :  print(paramsDict)
        if result == 'success':
            i = 0
            elements = extDict['elements']
            while (i < len(elements)) :
                gScore = elements[i]['score']
                gName = elements[i]['type']
                i += 1
                if gDebugLogFlag == True :  print('detect: ' + gName)
                if gScore > 0.25:
                    gDetected = True
                    break
    gLpService = True
    # 识别结束,复位识别标识符
    gStartRecognize = False
    print('识别结束')
    
def cb_lk_connect(data):
    global gLpConnect
    print('link platform connected')
    gLpConnect = True

def detect_object(frame, fileName, productKey, devName, devSecret) :
    start = utime.ticks_ms()
    global gDev
    # 上传图片到云
    fileid = gDev.uploadContent(fileName, frame, None)
    if fileid != None:
        ext = { 'filePosition':'lp', 'fileName': fileName, 'fileId': fileid }
        extStr = json.dumps(ext)
        allParams = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': 'detectObject', 'argInt': 1, 'ext': extStr }}
        allParamsStr = json.dumps(allParams)
        uploadFile = {
            'topic': '/sys/' + productKey + '/' + deviceName + '/thing/event/hli_event/post',
            'qos': 1,
            'payload': allParamsStr
        }
        # 上传完成通知HaaS云端
        gDev.publish(uploadFile)
        while gLpService == False:
            continue
    else:
        print('filedid is none, upload content fail')
    timeDiff = utime.ticks_diff(utime.ticks_ms(), start)
    if gDebugLogFlag == True :  print('get response time : %d' % timeDiff)

# 显示线程函数
def display_thread():
    # 引用全局变量
    global gDisp, gFrame, gDetected,gStartRecognize
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        # 采集摄像头画面
        gFrame = ucamera.capture()
        if gFrame != None:
            if gDetected == True:
                # 清除屏幕内容
                gDisp.clear()
                # 设置文字字体
                gDisp.font(gDisp.FONT_DejaVu40)
                # 显示识别结果
                gDisp.text(20, 100, 'Object Deteted!', gDisp.RED)
                utime.sleep_ms(2000)
                textShowFlag = False
                gDetected = False
            else:
                # 显示图像
                gDisp.image(0, 20, gFrame, 0)
                utime.sleep_ms(100)
                if textShowFlag == False:
                    # 设置显示字体
                    gDisp.font(gDisp.FONT_DejaVu18)
                    # 显示文字
                    gDisp.text(2, 0, 'Recognizing ...', gDisp.WHITE)
                    textShowFlag = True

# 按键检测
def key_event_thread():
    global gStartRecognize,gpio,gFrame
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
                    detect_object(gFrame, 'detectObject.jpg', productKey, deviceName, deviceSecret)
                    utime.sleep_ms(1000)
            elif status < 0 :
                gStartRecognize == False
        utime.sleep_ms(1000)

# 多线程案例
def main():
    # 全局变量
    global gDisp, gDev, gFrame, gDetected,gStartRecognize,gpio
    # 创建lcd display对象
    gDisp = display.TFT()
    gFrame = None
    gDetected = False
    gStartRecognize = False
    # 连接网络
    connect_wifi(SSID, PWD)
    # 设备初始化
    gDev = Device()
    gDev.on(Device.ON_CONNECT, cb_lk_connect)
    gDev.on(Device.ON_SERVICE, cb_lk_service)
    gDev.connect(key_info)
    while True:
        if gLpConnect:
            break
    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)
    # 初始化拍照按钮
    gpio = GPIO()
    gpio.open("photoButton")
    try:
        # 启动显示线程
        _thread.start_new_thread(display_thread, ())
        # 启动按键监控线程
        _thread.start_new_thread(key_event_thread, ())
    except:
       print("Error: unable to start thread")
    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
