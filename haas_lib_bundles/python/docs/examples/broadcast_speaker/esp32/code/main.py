#!/usr/bin/env python3
'''
@File       :    main.py
@Description:    播报音箱
@Date       :    2021年09月28日
@Author     :    ethan.lcz
@version    :    1.0.3
'''
from aliyunIoT import Device
import network
import http
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
# 回调函数状态
on_request = False
on_play = False
iot_connected = False
# Wi-Fi SSID和Password设置
wifi_ssid = "请填写您的路由器名称"
wifi_password = "请填写您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    wifi_connected = False
    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
    if not wifi_connected: 
        wlan.active(True)                  #激活界面
        wlan.scan()                        #扫描接入点
        #print("start to connect ", wifiSsid)
        wlan.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

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

    print ("set on_connect and on_service callback, start connect")

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
            speaker.download_resource_file(on_request, resDir)
            on_request = False
        elif on_play:
            speaker.play_voice(on_play,resDir)
            on_play = False
        time.sleep(0.01)
    # 断开连接
    device.close()


if __name__ == '__main__':
    print("remote speaker demo version - 1.0.3")
    speaker = Speaker(resDir)                                       # 初始化speaker
    speaker.play(tonepathPowerOn)                                   # 播放开机启动提示音
    get_wifi_status()                                               # 确保wifi连接成功
    do_connect_lk(productKey, deviceName, deviceSecret,speaker)     # 启动千里传音服务
