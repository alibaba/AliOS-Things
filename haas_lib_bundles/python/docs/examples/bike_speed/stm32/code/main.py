#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    KY XU
@version    :    1.0
@Description:    使用霍尔传感器实现测速
'''

from ulinksdk.aliyunIoT import Device
import network
import ujson
import utime   
from driver import GPIO
import hal55695
from micropython import const

# 快速的阈值，单位是5m/s
ALARM_DISTANCE = const(5)

halDev = 0
gpiodev = 0
buzzerdev = 0
#物联网连接标志
iot_connected = False
wlan = None
# 三元组信息
productKey = "hb2adMQbJPE"
deviceName = "esjPaIYRFxvE5ktjGj5V"
deviceSecret = "90bceb3d8da27f01c7522b8058aaaf96"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"

#等待wifi连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)
    wlan.scan()
    wlan.connected(wifiSsid, wifiPassword)
    #print("start to connect ", wifiSsid)

    while True:
        wifi_connected = wlan.isconnected
        if wifi_connected:
            break
        else:
            utime.sleep(0.5)
            print("wifi connected:", wifi_connected)
    ifconfig = wlan.ifconfig()
    print(ifconfig)
    utime.sleep(0.5)

#物联网平台的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

#云平台如果要下发属性实现该函数
def on_props(request):
    pass

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

    # 配置收到云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)

    # 启动连接阿里云物联网平台过程
    device.connect(key_info)

    # 等待设备成功连接到物联网平台
    while(True):
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print("sleep time 1 s")
            utime.sleep(1)
    print("sleep time 2s")
    utime.sleep(2)

def buzzer_init():
    global buzzerdev
    buzzerdev = GPIO()
    buzzerdev.open('buzzer')
    print("buzzer inited!")

def hall_sensor_init():
    global halDev, gpiodev
    
    gpiodev = GPIO()
    gpiodev.open("hal55695")
    
    halDev = hal55695.HAL55695(gpiodev)
    print("HAL55695 sensor init success!")
    
#上传云端
def calculae_upload_speed():
    global halDev, gpiodev
    while True:
        speed = halDev.speed_calulate()
        print("speed is", speed)
        upload_data = {'params': ujson.dumps({
            'speed': speed,
        })
        }
        if(speed > ALARM_DISTANCE):
            print("The speed is so fast!!!please control yourself!!!")
            buzzerdev.write(1)
        else:
            buzzerdev.write(0)
        device.postProps(upload_data)
        utime.sleep(1)
    gpiodev.close()

if __name__ == '__main__':
    hall_sensor_init()
    buzzer_init()
    nic = network.LAN()
    nic.active(1)
    connect_lk(productKey, deviceName, deviceSecret)
    calculae_upload_speed()
