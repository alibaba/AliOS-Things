#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    交通红绿灯
@Author     :    zhangheng
@version    :    1.0
方案设计
初始红灯60秒、黄灯3秒、绿灯30秒
每4次拍摄为一组
摄像头10秒拍摄1次,连续4次拍摄的车辆数都是大于M量车,判断当前是红灯或黄灯,则将接下来的绿灯加10秒,下一次的红灯减去10秒,如果当前就是绿灯,则不处理,继续下一组拍摄
如果有一次拍摄不满足大于M量车,则下一次灯的周期恢复为原始设置周期
如果云端有强制设置红绿灯时间周期或者强制恢复本地默认周期,则以云端为准
要求红灯最低只能减到30秒,绿灯最多60秒,其中一个达到限制点后,不再继续调整
'''

from aliyunIoT import Device
import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import _thread      # 线程库
import ujson as json
from cloudAI import *
from rgb import *

# Wi-Fi SSID和Password设置
SSID='***'
PWD='***'

# HaaS设备三元组
productKey = "***"
deviceName  = "***"
deviceSecret  = "***"

g_lk_connect = False
g_lk_service = False
g_score = 0

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

# 红灯最低时长(秒)
red_min_period = 30
# 绿灯最低时长(秒)
green_max_period = 60
# 默认红黄绿灯时长周期,下标0为红灯,下标1为黄灯且固定3秒不变,下标2位绿灯
rgb_current_period = [60, 3, 30]
gNumber=0
# 当前灯的颜色 0-red/1-green/2-blue
curGNumber = 0
# 拍摄4次一组
detectTime = 4
# 识别间隔周期
detectPeriod = 3
# 车辆数量阈值
warningVehicleNum = 3
# 是否云端强制设置的值
setting_from_server = False
# 还原初始值，即红绿灯时间重新变为rgb_current_period对应的值
setting_resume_default = 0

# 获取灯的颜色
def set_color_reference():
    global gLedColor,gNumber,curGNumber
    if gNumber == 0:
        # 红色
        ledColor = 0xff0000
    elif gNumber == 1:
        # 黄色
        ledColor = 0xFFFF00
    else:
        # 绿色
        ledColor = 0x00ff00
    gLedColor = ledColor
    curGNumber = gNumber
    gNumber += 1
    gNumber %= 3
    return gLedColor

# 开启Led灯
def led_open(color):
    print('current_time:', utime.time())
    global gLed
    gLed.setColor(color)
    print(curGNumber)
    sleepSeconds = rgb_current_period[curGNumber]
    print('当前灯的颜色:%#x'%color,'亮灯时间:',str(sleepSeconds) + 's')
    utime.sleep(sleepSeconds)

# 关闭Led灯
def led_close():
    global gLed,gLedColor
    print('关灯')
    gLedColor = 0x000000
    gLed.setColor(gLedColor)

# 接收到的云端下发的属性设置,data格式: {"code": 0, "params_len": 17, "msg_id": 1828542828, "params": "{\"red_period\":0}"}
def props_cb(data):
    global setting_from_server,setting_resume_default,rgb_current_period
    print(data)
    try:
        params = data['params']
        params_dict = json.loads(params)
        red_period = params_dict.get('red_period')
        print(red_period)
        green_period = params_dict.get('green_period')
        print(green_period)
        resume_default = params_dict.get('resume_default')
        if red_period is not None:
            print("setting red_period:", red_period)
            rgb_current_period[0] = red_period
            setting_from_server = True
        if green_period is not None:
            print("setting green_period:", green_period)
            rgb_current_period[2] = green_period
            setting_from_server = True
        if resume_default is not None:
            setting_resume_default = resume_default
            print("server setting_resume_default:", setting_resume_default)
            if setting_resume_default == 1:
                setting_from_server = False
                rgb_current_period = [60, 3, 30]
                print("rgb_current_period:", rgb_current_period)
    except Exception as e:
        print(e)

# 识别线程函数
def recognizeThread():
    global frame
    while True:
        if frame != None:
            engine.detectVehicleCongestion(frame)
            utime.sleep(detectPeriod)
        else:
            utime.sleep_ms(1000)

def action_cb(commandReply, result) :
    global detected, num
    num = result
    detected = False
    if commandReply == 'recognizeLogoReply' :
        if result != 'NA' :
            # print('detect num: ', num)
            detected = True
    else :
        print('unknown command reply')

# 显示线程函数
def display_thread():
    # 引用全局变量
    global disp, frame, detected, num, curGNumber, rgb_current_period,setting_from_server,setting_resume_default
    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)
    # 定义清屏局部变量
    clearFlag = False
    # 定义显示文本局部变量
    textShowFlag = False
    while True:
        needChangePeriod = True
        for i in range(1, detectTime):
            # 采集摄像头画面
            utime.sleep(detectPeriod)
            # print('start to capture')
            frame = ucamera.capture()
            # print('end to capture')
            if frame != None:
                if detected == True:
                    disp.clear()
                    # 设置文字字体
                    disp.font(disp.FONT_DejaVu40)
                    # 显示识别结果
                    # disp.text(40, 80, name, disp.RED)
                    disp.text(10, 60, 'Vehicle Num:' + str(num), disp.RED)
                    print('Vehicle Num:' + str(num) +' !!!')
                    utime.sleep_ms(1000)
                    # 判断车辆数量是否超过阈值
                    if num < warningVehicleNum:
                        needChangePeriod = False
                        print('needChangePeriod changed to', needChangePeriod)
                    textShowFlag = False
                    #detected = False
                else:
                    needChangePeriod = False
                    print('needChangePeriod changed', needChangePeriod)
                    # 显示图像
                    # print('start to display')
                    disp.image(0, 20, frame, 0)
                    utime.sleep_ms(100)
                    if textShowFlag == False:
                        # 设置显示字体
                        disp.font(disp.FONT_DejaVu18)
                        # 显示文字
                        disp.text(2, 0, 'Recognizing ...', disp.WHITE)
                        textShowFlag = True
        print('now needChangePeriod is ', needChangePeriod)
        if needChangePeriod == True:
            print('curGNumber:', curGNumber, 'setting_from_server:', setting_from_server, 'setting_resume_default:',setting_resume_default)
            # 没有从服务端下发配置
            if setting_from_server == False and setting_resume_default == 0:
                # 需要调整时间周期，当前是红灯或黄灯，则将接下来的绿灯加10秒，下一次的红灯减去10秒，如果当前就是绿灯，则不处理，继续下一组拍摄，如果本组4次拍摄不满足都大于M量车，则恢复为原始设置周期
                if curGNumber == 0 or curGNumber == 1:
                    if rgb_current_period[2] < green_max_period:
                        rgb_current_period[2] = rgb_current_period[2] + 10
                    if rgb_current_period[0] > red_min_period:
                        rgb_current_period[0] = rgb_current_period[0] - 10
            print("rgb_current_period after change:", rgb_current_period)
        else:
            print('false setting_from_server:', setting_from_server, 'setting_resume_default:',setting_resume_default)
            # 没有从服务端下发配置，则恢复为默认值
            if setting_from_server == False and setting_resume_default == 0:
                rgb_current_period = [60, 3, 30]
            print('rgb_current_period change to:', rgb_current_period)

# rgb灯变换线程
def rgb():
    print('rgb灯线程')
    while True:
        # 开灯
        led_open(set_color_reference())
        # 关灯
        led_close()

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

# 多线程案例
def main():
    # 全局变量
    global disp, dev, frame, detected,gLed,gLedColor,engine
    # 创建lcd display对象
    disp = display.TFT()
    frame = None
    detected = False
    # 开启RGB,必须在摄像头初始化前开启,否则先开启摄像头再开启RGB可能通道被占用导致RGB开启失败
    rgbR = PWM()
    rgbR.open("rgbR")
    rgbG = PWM()
    rgbG.open("rgbG")
    rgbB = PWM()
    rgbB.open("rgbB")
    gLed = RgbLed(rgbR, rgbG, rgbB)
    # 连接网络
    connect_wifi(SSID, PWD)
    # 初始化识别服务
    engine = CloudAI(key_info, action_cb, props_cb)
    try:
        # 启动显示线程
        _thread.start_new_thread(display_thread, ())
        # 设置识别线程stack
        _thread.stack_size(12 * 1024)
        # 启动识别线程
        _thread.start_new_thread(recognizeThread, ())
        # rgb变化
        rgb()
    except:
       print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
