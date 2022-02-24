#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    播报音箱案例（本案例符合HaaS Python 2.0 API标准，请务必按照“HaaS EDU K1快速开始”（https://haas.iot.aliyun.com/haasapi/#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup）文章的说明烧录固件
@Date       :    2022年02月11日
@Author     :    ethan.lcz
@version    :    v2.0
'''
from aliyunIoT import Device
import netmgr as nm
import utime
import ujson as json
from speech_utils import (
    Speaker,
    AUDIO_HEADER
)
import time

# 语音播放相关的音频资源文件定义
resDir = "/data/pyamp/resource/"
tonepathConnected = AUDIO_HEADER + resDir + "connected.wav"
tonepathPowerOn = AUDIO_HEADER + resDir + "poweron.wav"

# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifi_ssid = "请填写您的路由器名称"
wifi_password = "请填写您的路由器密码"

# 回调函数状态
on_request = False
on_play = False
iot_connected = False

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    wifi_connected = nm.getStatus()
    nm.disconnect()
    print("start to connect " , wifiSsid)
    nm.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

    while True :
        if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
            break
        else:
            wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)
    # utime.sleep(5)
    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True


# 设置service 事件接收函数（本案例是千里传音）
def on_service(data):
    global on_request, on_play
    print('****** on service ********')
    serviceid = data['service_id']
    data = json.loads(data['params'])
    # 语料下载服务
    if serviceid == "SpeechPost":
        on_request = data
    # 语料播报服务
    elif serviceid == "SpeechBroadcast":
        on_play = data
    else:
        pass


# 连接物联网平台
def do_connect_lk(productKey, deviceName, deviceSecret,speaker):
    global device, iot_connected, on_request, on_play
    key_info = {
        'region' : 'cn-shanghai' ,      #实例的区域
        'productKey': productKey ,      #物联网平台的PK
        'deviceName': deviceName ,      #物联网平台的DeviceName
        'deviceSecret': deviceSecret ,  #物联网平台的deviceSecret
        'keepaliveSec': 60              
    }
    # 将三元组信息设置到iot组件中
    device = Device()

    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)

   # 设定连接到物联网平台的回调函数，如果连接物联网平台下发控制服务请求指令，则调用on_service函数
    device.on(Device.ON_SERVICE, on_service)

    print ("开始连接物联网平台")

    # 启动连接阿里云物联网平台过程
    device.connect(key_info)
    # 等待设备成功连接到物联网平台
    while True:
        if iot_connected:
            print("物联网平台连接成功")
            speaker.play(tonepathConnected)
            break
        else:
            print("sleep for 1 s")
            time.sleep(1)

   # 触发linkit sdk持续处理server端信息
    while True:
        if  on_request:
            print('get on request cmd')
            speaker.download_resource_file(on_request, resDir) # 语料下载
            on_request = False
        elif on_play:
            speaker.play_voice(on_play,resDir)                 # 语料播报
            on_play = False
        time.sleep(0.01)
    # 断开连接
    device.close()


if __name__ == '__main__':
    print("remote speaker demo version - v2.0")
    speaker = Speaker(resDir)                                       # 初始化speaker
    speaker.play(tonepathPowerOn)                                   # 播放开机启动提示音
    get_wifi_status()                                               # 确保wifi连接成功
    do_connect_lk(productKey, deviceName, deviceSecret,speaker)     # 启动千里传音服务
