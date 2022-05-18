#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    垃圾分类智能垃圾桶案例
@Author     :    翱一
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
from driver import PWM
import servo

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD= 'Your-AP-Password'
# HaaS设备三元组
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

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

#打开垃圾桶盖
def openWetTrashbin():
    global closed, servoObj_wet
    if closed == True:
        print("open the wet transhbin")
        servoObj_wet.setOptionSero(90)
        closed = False

#打开可循环垃圾桶盖
def openRecyclableBin():
    global closed, servoObj_rec
    if closed == True:
        print("open the recycle hbin")
        servoObj_rec.setOptionSero(90)
        closed = False

#打开有害垃圾桶盖
def openHarmBin():
    global closed, servoObj_harm
    if closed == True:
        print("open the harm bin")
        servoObj_harm.setOptionSero(90)
        closed = False

#打开干垃圾桶盖
def openDryBin():
    global closed, servoObj_dry
    if closed == True:
        print("open the dry bin")
        servoObj_dry.setOptionSero(90)
        closed = False

#关闭垃圾桶盖
def closeTrashbin():
    global closed, servoObj_dry, servoObj_rec, servoObj_wet, servoObj_harm
    if closed == False:
        utime.sleep_ms(200)
        print("close the transhbin")
        # 操作关门
        servoObj_dry.setOptionSero(0)
        servoObj_rec.setOptionSero(0)
        servoObj_wet.setOptionSero(0)
        servoObj_harm.setOptionSero(0)
        closed = True

def recognize_cb(commandReply, result) :
    global detected, gName, gStartRecognize, closed
    detected = False
    gName = 'NA'
    if commandReply == 'classifyingRubbishReply' and gStartRecognize == True:
        if result != 'NA' :
            gName = result
            detected = True
            print('detected: ' + gName)
            if gName == 'Recyclable' :
                openRecyclableBin()
            elif gName == 'Harmful' :
                openHarmBin()
            elif gName == 'Dry' :
                openDryBin()
            else :
                openWetTrashbin()
            utime.sleep(2)
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
            if status == 1 :
                gStartRecognize = True
                print('有物体进入')
            else :
                closeTrashbin()
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
    global disp, gFrame, detected, engine, gStartRecognize, gIrDev, closed, servoObj_rec, servoObj_harm, servoObj_dry, servoObj_wet
    # 创建lcd display对象
    disp = display.TFT()
    gFrame = None
    detected = False
    gStartRecognize = False
    closed = True

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

    # 初始化可循环垃圾桶舵机
    print("init recycle servo...")
    pwmObj_rec = PWM()
    pwmObj_rec.open("servo_recycle")
    print("buzzer inited!")
    servoObj_rec = servo.SERVO(pwmObj_rec)
    servoObj_rec.setOptionSero(0)
    print("0")
    utime.sleep(2)

    # 初始化有害垃圾舵机
    print("init harm servo...")
    pwmObj_harm = PWM()
    pwmObj_harm.open("servo_harm")
    print("buzzer inited!")
    servoObj_harm = servo.SERVO(pwmObj_harm)
    servoObj_harm.setOptionSero(0)
    print("1")
    utime.sleep(2)

    # 初始化干垃圾舵机
    print("init dry servo...")
    pwmObj_dry = PWM()
    pwmObj_dry.open("servo_dry")
    print("buzzer inited!")
    servoObj_dry = servo.SERVO(pwmObj_dry)
    servoObj_dry.setOptionSero(0)
    print("2")
    utime.sleep(2)

    # 初始化湿垃圾舵机
    print("init wet servo...")
    pwmObj_wet = PWM()
    pwmObj_wet.open("servo_wet")
    print("buzzer inited!")
    servoObj_wet = servo.SERVO(pwmObj_wet)
    servoObj_wet.setOptionSero(0)
    print("3")
    utime.sleep(2)

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
