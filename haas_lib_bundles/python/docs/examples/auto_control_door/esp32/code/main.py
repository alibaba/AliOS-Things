#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    zhangheng
@version    :    1.0
@Description:    通过红外检测是否有人靠近，有人靠近则启动舵机执行开门操作，没人，则进行关门操作
                 board.json - 硬件资源配置文件，详情请参考：https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/driver
'''

from aliyunIoT import Device
from driver import GPIO
from driver import PWM
import servo
import irdistance
import utime   # 延时函数在utime库中
import sntp         # 网络时间同步库
import network      # 网络库
import _thread      # 线程库
import ujson as json


# Wi-Fi SSID和Password设置
SSID='xxx'
PWD='xxx'

# HaaS设备三元组
productKey = "xxx"
deviceName  = "xxx"
deviceSecret  = "xxx"

g_lk_connect = False
g_lk_service = False

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    # 引用全局变量
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start...')
            sntp.setTime()
            print('NTP done')
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True

# 上报统计人数到云端
def postDeviceProps():
    global dev,count
    value = {'total_person_count' : count}
    data = {'params': json.dumps(value)}
    ret = dev.postProps(data)
    if ret == 0 :
        print('累计人员数量上报成功')
    else :
        print('累计人员数量上报失败')

# 接收云端下发的属性设置
# request格式: {"code": 0, "params_len": 17, "msg_id": 1828542828, "params": "{\"door_status\":0}"}
def on_props(request):
    global door_status
    try:
        props = eval(request['params'])
        door_status = props['door_status']
        print("door_status set value : " + str(door_status))
    except Exception as e:
        print(e)


def operatorDoorThread():
    global detected,servoObj, closed,door_status
    closed = True
    # 开门后判断是否还有人
    while True:
        if door_status == 1:
            openDoor()
        elif door_status == -1:
            closeDoor()
        else:
            if detected == True:
                 openDoor()
            else:
                 closeDoor()


def openDoor():
    global closed, servoObj
    if closed == True:
        print("open the door")
        servoObj.setOptionSero(90)
        # TODO 开门操作
        closed = False

def closeDoor():
    global closed, servoObj
    if closed == False:
        utime.sleep_ms(200)
        print("close the door")
        # 操作关门
        servoObj.setOptionSero(0)
        closed = True

def objDetectThread():
     global detected, count, door_status
     while True:             # 无限循环
        status = irDev.objectDetection()
        # 检测到物体
        if status == 0:
            detected = True
            # 非常闭状态的才上报
            if door_status != -1:
                count = count + 1
                print("object detected, count = " + str(count))
                postDeviceProps()
            # 检测到人后停5秒再检测，相当于模拟行人通过的时间
            utime.sleep(5)
        # 没有检测到
        elif status == 1:
            detected = False
            print('no object detected')
            # 没检测到人，则间隔500ms检测一次
            utime.sleep_ms(500)


def main():
    global dev,irDev, servoObj, detected,count, door_status
     # 连接网络
    connect_wifi(SSID, PWD)

    # 设备初始化
    dev = Device()
    dev.on(Device.ON_CONNECT, cb_lk_connect)
    # 配置收到云端属性控制指令的回调函数
    # 如果收到物联网平台发送的属性控制消息，则调用on_props函数
    dev.on(Device.ON_PROPS, on_props)
    dev.connect(key_info)
    while True:
        if g_lk_connect:
            break

    detected = False
    count = 0
    door_status = 0

    print("init ir...")
    # 初始化红外
    gpioDev = GPIO()
    gpioDev.open("ir")
    irDev = irdistance.IRDISTANCE(gpioDev)
    print("ir inited!")
    # 初始化舵机
    print("init servo...")
    pwmObj = PWM()
    pwmObj.open("servo")
    print("buzzer inited!")
    servoObj = servo.SERVO(pwmObj)
    servoObj.setOptionSero(0)
    print("0")
    utime.sleep(2)

    try:
        # 启动红外检测
        _thread.start_new_thread(objDetectThread, ())
        # 启动舵机模拟开门/关门操作线程
        _thread.start_new_thread(operatorDoorThread, ())
    except Exception as e:
        print(e)
        print("Error: unable to start thread")

    while True:
        utime.sleep_ms(1000)

if __name__ == '__main__':
    main()
