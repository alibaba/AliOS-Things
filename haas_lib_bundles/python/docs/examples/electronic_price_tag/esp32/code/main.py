#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    zeta.zz
@version    :    1.0
@Description:    price_tag - 电子价签
                 board.json - 硬件资源配置文件
'''


from showPriceData import ShowData
from machine import Pin
from driver import GPIO
import network
import time
import kv
import gc
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import json
import binascii

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

# 定义电子价签物联网模型参数
sal_update = 0
sal_pic = ''
sal_offerta = '30'
sal_price = '108.50'
sal_name = 'AliOS-Things'
arrayBuf = bytearray(b'')
updating = 0

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


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    # global post_default_value
    global arrayBuf, sal_update, sal_name, sal_price, sal_offerta, sal_pic, updating
    try:
        props = eval(request['params'])
        print(props)

        # 获取 sal_name 参数
        if 'sal_name' in props.keys():
            sal_name = props['sal_name']
            print(sal_name)
        # 获取 sal_price 参数
        if 'sal_price' in props.keys():
            sal_price = props['sal_price']
            print(sal_price)
        # 获取 sal_offerta 参数
        if 'sal_offerta' in props.keys():
            sal_offerta = props['sal_offerta']
            print(sal_offerta)
        # 获取 sal_pic 参数
        if 'sal_pic' in props.keys():
            sal_pic = props['sal_pic']
            # print(sal_pic)
            print("recevied sal pic")

        if 'sal_update' in props.keys():
            sal_update = props['sal_update']
            print(sal_update)
            # 判断是否需要更新
            if sal_update == 1:
                if len(sal_pic) % 2 == 0:
                    picBuf = binascii.unhexlify(sal_pic)
                    # 改变显示图形buf
                    arrayBuf = bytearray(picBuf)
                    # print(arrayBuf)

                # 刷新屏幕
                # 墨水屏刷新慢，加入保护
                if updating == 0:
                    updating = 1
                    gc.collect()
                    priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%', byteBuf=arrayBuf)
                    gc.collect()
                    updating = 0

                # 上传更新设置, 图片更新后才会更新
                sal_update = 0
                post_default_value()

    except Exception as e:
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
            post_default_value()
            kv.set('app_upgrade_channel', 'disable')
            break
        else:
            print('sleep for 1 s')
            time.sleep(1)
    time.sleep(2)


# 初始化物联网平台更新状态
def post_default_value():
    # global sal_update
    value = {'sal_update' : sal_update}
    post_props(value)
    value = {'sal_name' : sal_name}
    post_props(value)
    value = {'sal_price' : sal_price}
    post_props(value)
    value = {'sal_offerta' : sal_offerta}
    post_props(value)


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)


    # 初始化墨水屏
    # epaperDC = GPIO()
    # epaperDC.open("epaper_dc")
    # epaperCS = GPIO()
    # epaperCS.open("epaper_cs")
    # epaperRST = GPIO()
    # epaperRST.open("epaper_rst")
    # epaperBUSY = GPIO()
    # epaperBUSY.open("epaper_busy")
    # priceTagObj = ShowData(14500000, epaperDC, epaperCS, epaperRST, epaperBUSY)
    # priceTagObj = ShowData(14500000, Pin(18), Pin(23), Pin(17), Pin(5), Pin(16), Pin(4))
    priceTagObj = ShowData(14500000, Pin(17), Pin(2), Pin(16), Pin(4))
    # 初始化显示
    updating = 1
    gc.collect()
    priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%')
    gc.collect()
    updating = 0

    # buf test code
    # apple_pic = b'ffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffff1ffeffff1fffffffffffff03f8ffff1fffffffffffff00f8ffff1fffffffffc77fe0f1ffff1fffffffffc71ffcf1ffff1fffffffff8f1ffef1ffff1fffffffff0f8ffff1ffff1fffffffff1f87fff1ffff1fffffffff3fc6fff9ffff1fffffffff3fe2fff8ffff1fffffffff7fe0fff8ffff1fffffffff7ff0fff8ffff1fffffffff7ff07ffcffff1ffffffffffff03ffcffff1ffffffffffff01ffeffff1ffffffffffff00fffffff1ffffffffffff083ffffff1ffffffffffd01c0ffffff1fffffff3fc00080ffffff1fffffff0f000100feffff1fffffff0706080afcffff1fffffffc33f807ff8ffff1fffffffe37fc0fff8ffff1ffffffff1ffe0fff1ffff1ffffffff1ffe0fff1ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff9fff1fff3ffff1ffffffff0ffe0fff1ffff1fffff3ff0ffe0ff81ffff1fffff0fe07fc0ff00feff1fffff07851f043f08fcff1fffffc307000e007cf8ff1fffffe11f801f00fff0ff1ffffff13fe0ffc0fff1ff1ffffff8fff0ffe1ffe3ff1ffffff8fff0fff1ffe3ff1ffffff8fff9ffe1ffe3ff1ffffff8fff1fff3ffe3ff1ffffff8fff8fff1ffe3ff1ffffff8fff0ffe1ffe3ff1ffffff8fff0ffe1ffe3ff1ffffff17fe0ffc0fff1ff1fffffe17fc07fc0fff0ff1fffffc31f863f0c7ff8ff1fffff07020e040e08fcff1fffff0f801f003f00feff1fffff3fe07fc07f80ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1fff3ffff1ffffffff1ffe0fff1ffff1ffffffff1ffe0fff1ffff1fffffffe17fc0fff8ffff1fffffffc33f847ff8ffff1fffffff07060e16fcffff1fffffff0f001f00feffff1fffffff3fc07f80ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff9fff3ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1fffffffffe1fff0ffffff1fffffffffe3fff8ffffff1fffffffff873ffcffffff1fffffffff070ffcffffff1fffffffff0f00ffffffff1fffffffff3f80ffffffff1fffffffffffe9ffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1f'
    # picBuf = binascii.unhexlify(apple_pic)
    # arrayBuf = bytearray(picBuf)
    # priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%', byteBuf=arrayBuf)
    # gc.collect()




