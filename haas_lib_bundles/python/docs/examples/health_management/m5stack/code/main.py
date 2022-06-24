#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    菜品识别
@Author     :    aoyi
@version    :    1.0
'''

import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import _thread      # 线程库
import sntp         # 网络时间同步库
from cloudAI import *
from bodyCalculator import *

DIET_SUGGEST = "Diet_suggest"
UPLOAD_BMI = "BMI"
UPLOAD_FOOD_NAME = "Food_name"
UPLOAD_CALORIES = "Calories"
UPLOAD_START_ENROLL = "Start_enroll"
UPLOAD_DATE = "Date"

# Wi-Fi SSID和Password设置
SSID='请输入您的路由器名称'
PWD='请输入您的路由器密码'
# HaaS设备三元组
productKey = "产品密钥"
deviceName  = "设备名称"
deviceSecret  = "设备密钥"
detected = False
food_name = ''

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

def food_cb(commandReply, result, calorie):
    global detected, food_name, total_food_calorie, Start_enroll
    food_name = 'NA'
    detected = False
    if commandReply == 'recognizeFoodReply' :
        if result != 'NA' :
            food_name = result
            Start_enroll = 0
            total_food_calorie = total_food_calorie + int(calorie)
            print('detect food: ' + food_name + 'total calorie:' + str(total_food_calorie))
            detected = True
    elif commandReply == 'on_props':
        body_calc.set_body_data(result)

        if 'Start_enroll' in result.keys():
            Start_enroll = result['Start_enroll']
    else:
        print('unknown command reply')

    post_default_value()

# 识别线程函数
def recognizeThread():
    global frame, Start_enroll
    while True:
        if frame != None and Start_enroll:
            engine.recognizeFood(frame)
            post_default_value()
            #engine.recognizeGesture(frame)
            utime.sleep_ms(6000)
        else:
            utime.sleep_ms(1000)

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, frame, detected, food_name
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
                disp.text(10, 50, 'detect: ' + food_name, disp.RED)
                #disp.text(10, 50, 'detect pedestrain', disp.RED)
                utime.sleep_ms(2000)
                textShowFlag = False
                detected = False
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

def post_props(data):
    global engine
    if isinstance(data,dict):
        data = {'params': json.dumps(data)}
        print('upload props-->' + str(data))
    ret = engine.getDevice().postProps(data)
    return ret

def post_default_value():
    global food_name, total_food_calorie, Start_enroll, body_calc, last_date
    diet_suggestion = body_calc.generate_diet_suggestion(total_food_calorie)
    bmi = body_calc.get_body_bmi()
    date = str(utime.localtime()[1]) + '月' + str(utime.localtime()[2]) + '日'
    print(date)
    if last_date != date:
        total_food_calorie = 0 #日期变了，清空今日摄入卡路里数值
        last_date = date
    value = {DIET_SUGGEST : diet_suggestion,
             UPLOAD_BMI : bmi,
             UPLOAD_FOOD_NAME : food_name,
             UPLOAD_CALORIES : total_food_calorie,
             UPLOAD_START_ENROLL: Start_enroll,
             UPLOAD_DATE: date}
    post_props(value)

def main():
    # 全局变量
    global disp, frame, detected, food_name, engine, Start_enroll, body_calc, total_food_calorie, last_date
    # 创建lcd display对象
    disp = display.TFT()
    frame = None
    detected = False
    Start_enroll = False
    last_date = ' '
    total_food_calorie = 0
    # 连接网络
    connect_wifi(SSID, PWD)

    engine = CloudAI(key_info, food_cb)
    body_calc = BodyCalculator()

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
