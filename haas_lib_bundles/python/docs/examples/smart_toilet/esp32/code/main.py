#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    guoliang.wgl
@version    :    1.0
@Description:    smart_toilet案例 - 智能洗手间
                 board.json - 硬件资源配置文件
'''
from ir import IR
from mq136 import MQ136
from driver import ADC, GPIO
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
# 红外检测是否存在人和硫化氢检测
FLAG_IR = "human_detect"
FLAG_H2S = "h2s_value"


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
    pass


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


def upload_h2s(value):
    global FLAG_H2S
    value = {FLAG_H2S: value}
    post_props(value)


def upload_human_status(value):
    global FLAG_IR
    value = {FLAG_IR: value}
    post_props(value)


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)  #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    # 初始化mq136硫化氢检测传感器
    adcObj = ADC()  # ADC通道对象
    adcObj.open("mq136")  # 按照board.json中名为"mq136"节点的配置初始化ADC设备对象
    mq136Dev = MQ136(adcObj)
    # 初始化热释电红外传感器
    gpioirDev = GPIO()
    gpioirDev.open("ir")
    ir = IR(gpioirDev)
    while True:
        h2s_value = mq136Dev.getVoltage()
        print("current  h2s is :{}".format(h2s_value))
        upload_h2s(h2s_value)
        human_exist = ir.irDetect()
        upload_human_status(human_exist)
        if human_exist:
            print("human detected")
        else:
            print("no human found")
        time.sleep(5)
