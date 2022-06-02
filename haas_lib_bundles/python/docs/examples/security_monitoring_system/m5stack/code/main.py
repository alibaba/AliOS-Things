#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    aoyi
@version    :    1.0
@Description:    security_monitoring_system案例 - 防盗报警系统
                 board.json - 硬件资源配置文件
'''


from buzzer import BUZZER
import irdistance
from driver import PWM,GPIO
import time
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import json
import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import _thread      # 线程库
from cloudAI import *

# 物联网平台连接标志位
iot_connected = False
wlan = None
# 三元组信息
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
# 物联网设备实例
device = None
# Wi-Fi SSID和Password设置
wifiSsid = "Your-AP-SSID"
wifiPassword = "Your-AP-Password"
# 警报开关以及时间段控制（大于等于alarm_start 或者小于等于alarm_end ）
alarm_switch = 1
alarm_status = 0
alarm_count = 0
alarm_pic_url = ''
alarming = False
FLAG_ALARM_STATUS = "alarm_status"
FLAG_ALARM_COUNT = "alarm_count"
FLAG_ALARM_SWITCH = "magnetic_switch"
UP_PICTURE_URL = "PictureURL"

cloud_key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName':  deviceName,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

# 显示线程函数
def displayThread():
    # 引用全局变量
    global disp, gFrame, alarming
    while True:
        # 采集摄像头画面
        gFrame = ucamera.capture()
        if gFrame != None:
            # 显示图像
            disp.image(0, 20, gFrame, 0)
            time.sleep(0.1)
            if alarming:
                # 设置显示字体
                disp.font(disp.FONT_DejaVu40)
                # 显示文字
                disp.text(10, 50, 'WARNING...', disp.RED)

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False
    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)
    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            time.sleep(0.5)
            print("wifi_connected:", wifi_connected)
    ifconfig = wlan.ifconfig()                    #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    time.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

def recognize_cb(commandReply, result) :
    global alarm_pic_url, gStartRecognize, alarm_switch
    if commandReply == 'saveImageTestReply' and gStartRecognize == True:
        if result != 'NA' :
            alarm_pic_url = result
            print('imageurl: ' + result)
            utime.sleep(2)
    elif commandReply == 'magnetic_switch':
        alarm_switch = result
    else :
        print('main: unknown command reply')
    post_default_value()
    gStartRecognize = False
    print('识别结束')

def post_props(data):
    global engine, device
    if isinstance(data,dict):
        data = {'params': json.dumps(data)}
        print('upload props-->' + str(data))
    ret = engine.getDevice().postProps(data)
    #ret = device.postProps(data)
    return ret

def post_default_value():
    global alarm_switch,alarm_status,alarm_count,alarm_pic_url,gFrame,FLAG_ALARM_SWITCH,FLAG_ALARM_STATUS,UP_PICTURE_URL
    value = {FLAG_ALARM_SWITCH : alarm_switch,
             FLAG_ALARM_STATUS : alarm_status,
             FLAG_ALARM_COUNT : alarm_count,
             UP_PICTURE_URL : alarm_pic_url}
    post_props(value)


if __name__ == '__main__':
    global disp, gFrame, engine, gStartRecognize
    # 创建lcd display对象
    disp = display.TFT()
    gFrame = None
    gStartRecognize = False
    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    get_wifi_status()
    #初始化cloudai
    engine = CloudAI(cloud_key_info, recognize_cb)

    # 初始化摄像头
    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)
    # 初始化蜂鸣器
    pwmObj = PWM()
    pwmObj.open("buzzer")
    pwm_init_data = {'freq':2000, 'duty': 0}
    buzzer =  BUZZER(pwmObj,data=pwm_init_data)
    # 初始化干簧管传感器
    gpioirDev = GPIO()
    gpioirDev.open("ir")
    gIrDev = irdistance.IRDISTANCE(gpioirDev)
    print("megnetic detector ...")

    try:
        # 启动显示线程
        _thread.start_new_thread(displayThread, ())
    except:
       print("Error: unable to start thread")
    while True:
        if alarm_switch == 1 and gIrDev.objectDetection() == 1:
            wait_cnt = 0
            if alarming == False:
                print('human detected, start buzzer')
                pwm_start_data = {'freq':2000, 'duty': 5}
                buzzer.start(pwm_start_data)
                alarm_count = alarm_count + 1
                alarming = True
                alarm_status = 1
                post_default_value()

            if gFrame != None:
                gStartRecognize = True
                engine.saveImageTest(gFrame)

            while gStartRecognize == True and wait_cnt < 50:
                time.sleep(0.1)
                wait_cnt = wait_cnt + 1
            post_default_value()
        if alarm_switch == 0 or gIrDev.objectDetection() == 0:
            if alarming:
                print('close buzzer')
                pwm_start_data = {'freq':2000, 'duty': 0}
                buzzer.start(pwm_start_data)
                alarming = False
                alarm_status = 0
                post_default_value()
        time.sleep(0.1)


