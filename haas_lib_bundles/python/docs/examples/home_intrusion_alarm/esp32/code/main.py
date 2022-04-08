#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    guoliang.wgl
@version    :    1.0
@Description:    home_intrusion_alarm案例 - 家庭红外入侵检测和报警系统
                 board.json - 硬件资源配置文件
'''


from buzzer import BUZZER
from ir import IR
from driver import PWM,GPIO
import time
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import sntp
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
do_alarm = 1
alarm_start = 23
alarm_end = 6
alarming = False
FLAG_ALARM_CONTROL = "alarm_control"
FLAG_ALARM_START = "alarm_start"
FLAG_ALARM_END = "alarm_end"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False
    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点
    #print("start to connect ", wifiSsid)
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


# 判断当前时间是否在有效时间段内
def time_valid():
    global alarm_start,alarm_end
    cur_hour = time.localtime()[3]
    return cur_hour >= alarm_start or cur_hour <= alarm_end


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True


# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global do_alarm,alarm_start,alarm_end,post_default_value,FLAG_ALARM_CONTROL,FLAG_ALARM_START,FLAG_ALARM_END
    try:
        props = eval(request['params'])
        if FLAG_ALARM_CONTROL in props.keys():
            do_alarm = props[FLAG_ALARM_CONTROL]
            print('on_props: do_alarm {}'.format(do_alarm))
        elif FLAG_ALARM_START in props.keys():
            alarm_start = props[FLAG_ALARM_START]
            print('on_props: alarm_start {}'.format(alarm_start))
        elif FLAG_ALARM_END in props.keys():
            alarm_end = props[FLAG_ALARM_END]
            print('on_props: alarm_end {}'.format(alarm_end))
        post_default_value()
    except Excaption as e:
        print(e)


def post_props(data):
    global device
    if isinstance(data,dict):
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
    global do_alarm,alarm_start,alarm_end,FLAG_ALARM_CONTROL,FLAG_ALARM_START,FLAG_ALARM_END
    value = {FLAG_ALARM_CONTROL : do_alarm}
    post_props(value)
    value = {FLAG_ALARM_START : alarm_start}
    post_props(value)
    value = {FLAG_ALARM_END : alarm_end}
    post_props(value)


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息,可以参考文章:https://blog.csdn.net/HaaSTech/article/details/114360517
    # global productKey, deviceName, deviceSecret ,on_request, on_play
    get_wifi_status()
    sntp.setTime()
    connect_lk(productKey, deviceName, deviceSecret)
    post_default_value()
    # 初始化蜂鸣器
    pwmObj = PWM()
    pwmObj.open("buzzer")
    pwm_init_data = {'freq':2000, 'duty': 0}
    buzzer =  BUZZER(pwmObj,data=pwm_init_data)
    # 初始化热释电红外传感器
    gpioirDev = GPIO()
    gpioirDev.open("ir")
    ir = IR(gpioirDev)
    while True:
        if do_alarm == 1 and time_valid() and ir.irDetect() == 1:
            print('human detected, start buzzer')
            pwm_start_data = {'freq':2000, 'duty': 5}
            buzzer.start(pwm_start_data)
            alarming = True
        if do_alarm == 0 and alarming:
            print('close buzzer')
            pwm_start_data = {'freq':2000, 'duty': 0}
            buzzer.start(pwm_start_data)
            alarming = False
        time.sleep(0.1)


