#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    guoliang.wgl
@version    :    1.0
@Description:    smart_fan案例 - 智能控制小风扇
                 board.json - 硬件资源配置文件
'''

from fan import Fan
from aht21b import AHT21B
from driver import PWM, I2C
import time
from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import json

# 物联网平台连接标志位
iot_connected = False
wlan = None
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
# 物联网设备实例
device = None
# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"
# 警报开关以及时间段控制（大于等于alarm_start 或者小于等于alarm_end ）
gear1_temp = 22
gear2_temp = 27
gear3_temp = 32
FLAG_CUR_TEMP = "cur_temp"
FLAG_GEAR1 = "gear1"
FLAG_GEAR2 = "gear2"
FLAG_GEAR3 = "gear3"
cur_gear = 0


# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False
    wlan.active(True)  #激活界面
    wlan.scan()  #扫描接入点
    #print("start to connect ", wifiSsid)
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)
    while True:
        wifi_connected = wlan.isconnected()  # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:  # Wi-Fi连接成功则退出while循环
            break
        else:
            time.sleep(0.5)
            print("wifi_connected:", wifi_connected)
    ifconfig = wlan.ifconfig()  #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    time.sleep(0.5)


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True


# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global FLAG_GEAR1, FLAG_GEAR2, FLAG_GEAR3, gear1_temp, gear2_temp, gear3_temp
    try:
        props = eval(request['params'])
        if FLAG_GEAR1 in props.keys():
            gear1_temp = props[FLAG_GEAR1]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR1, gear1_temp))
        elif FLAG_GEAR2 in props.keys():
            gear2_temp = props[FLAG_GEAR2]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR2, gear2_temp))
        elif FLAG_GEAR3 in props.keys():
            gear3_temp = props[FLAG_GEAR3]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR3, gear3_temp))
        post_default_value()
    except Exception as e:
        print(e)


def post_props(data):
    global device
    if isinstance(data, dict):
        data = {'params': json.dumps(data)}
    ret = device.postProps(data)
    return ret


def connect_lk(productKey, deviceName, deviceSecret):
    global device, iot_connected
    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    # 将三元组信息设置到iot组件中
    device = Device()
    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)
    # 配置收到云端属性控制指令的回调函数
    # 如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)
    # 启动连接阿里云物联网平台过程
    device.connect(key_info)
    # 等待设备成功连接到物联网平台
    while True:
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            time.sleep(1)
    time.sleep(2)


def post_default_value():
    global FLAG_GEAR1, FLAG_GEAR2, FLAG_GEAR3, gear1_temp, gear2_temp, gear3_temp
    value = {FLAG_GEAR1: gear1_temp}
    post_props(value)
    value = {FLAG_GEAR2: gear2_temp}
    post_props(value)
    value = {FLAG_GEAR3: gear3_temp}
    post_props(value)


def upload_temp(temp):
    value = {FLAG_CUR_TEMP: temp}
    post_props(value)


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)  #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息
    # global productKey, deviceName, deviceSecret ,on_request, on_play
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    post_default_value()
    # 初始化风扇控制pwm
    pwmObj = PWM()
    pwmObj.open("fan")
    fan = Fan(pwmObj)
    fan.control(0)
    # 初始化温度传感器
    i2c = I2C()
    i2c.open('aht21b')
    aht = AHT21B(i2c)
    while True:
        temp = aht.getTemperature()
        print('cur temp is {}'.format(temp))
        upload_temp(temp)
        if temp <= gear1_temp and cur_gear != 0:
            cur_gear = 0
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear1_temp and temp <= gear2_temp and cur_gear != 1:
            cur_gear = 1
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear2_temp and temp <= gear3_temp and cur_gear != 2:
            cur_gear = 2
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear3_temp and cur_gear != 3:
            cur_gear = 3
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
