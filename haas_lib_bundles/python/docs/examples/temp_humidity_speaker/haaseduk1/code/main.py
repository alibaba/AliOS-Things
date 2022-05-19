#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

'''
基于HaaS Python框架的本地语音播报温湿度系统
'''

from aliyunIoT import Device
import netmgr as nm
import utime
import ujson
from speech_utils import (
    Speaker,
    AUDIO_HEADER
)
import time
from cht8305 import CHT8305
from driver import I2C
import sh1106              # SH1106 OLED驱动库
from driver import SPI     # 引入SPI总线库
from driver import GPIO    # 引入GPIO库
import framebuf            # framebuf基类，用于设置字体库

# 语音播放相关的音频资源文件定义
resDir = "/data/pyamp/resource/"
tonepathConnected = AUDIO_HEADER + resDir + "connected.wav"
tonepathPowerOn = AUDIO_HEADER + resDir + "poweron.wav"

# 三元组信息
productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# Wi-Fi SSID和Password设置
wifi_ssid = "请输入您的路由器名称"
wifi_password = "请输入您的路由器密码"

# 回调函数状态
on_request = False
on_play = False
iot_connected = False

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    wifi_connected = nm.getStatus()
    nm.disconnect()
    print("start to connect " , wifi_ssid)
    # 连接到指定的路由器（路由器名称为wifi_ssid, 密码为：wifi_password）
    nm.connect(wifi_ssid, wifi_password)

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

def post_data_to_cloud(device, temphumidity):
    # 上报温湿度到云平台
    prop = ujson.dumps({
        "CurrentTemperature": temphumidity[0],
        "CurrentHumidity": temphumidity[1],
    })

    upload_data = {"params": prop}
    device.postProps(upload_data)

def play_display_temperature_humidity(cht8305Dev):
    play_data = {"format":0, "speechs":[]}
    temphumidity = cht8305Dev.getTempHumidity()
    print("当前温度:", temphumidity[0], "当前湿度:", temphumidity[1])

    if (temphumidity[0] < 0):
        temperature = "{$%.2f}" % -temphumidity[0]
        humidity = "{$%.2f}" % temphumidity[1]
        play_data["speechs"] = ["temperature", "negative", temperature, "centigrade", "humidity", humidity]
    else:
        temperature = "{$%.2f}" % temphumidity[0]
        humidity = "{$%.2f}" % temphumidity[1]
        play_data["speechs"] = ["temperature", temperature, "centigrade", "humidity", humidity]

    temp_str = "T:%.2f" % temphumidity[0]
    humi_str = "H:%.2f%%" % temphumidity[1]
    oledShowText(temp_str, 3, 1, 1, True, 12)
    oledShowText(humi_str, 3, 16, 1, False, 12)

    speaker.play_voice(play_data,resDir)
    return temphumidity

# OLED初始化
def oledInit():
    global oled

    # 字库文件存放于项目目录 font, 注意若用到了中英文字库则都需要放置
    framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/font/ASC12_8')
    framebuf.set_font_path(framebuf.FONT_ASC16_8, '/data/font/ASC16_8')
    framebuf.set_font_path(framebuf.FONT_ASC24_12, '/data/font/ASC24_12')
    framebuf.set_font_path(framebuf.FONT_ASC32_16, '/data/font/ASC32_16')

    oled_spi = SPI()
    oled_spi.open("oled_spi")

    oled_res = GPIO()
    oled_res.open("oled_res")

    oled_dc = GPIO()
    oled_dc.open("oled_dc")

    #oled像素132*64
    oled = sh1106.SH1106_SPI(132, 64, oled_spi, oled_dc, oled_res)

# OLED显示
# text:显示的文本
# x:水平坐标 y:垂直坐标
# color:颜色
# clear: True-清屏显示 False-不清屏显示
# sz:字体大小
def oledShowText(text, x, y, color, clear, sz):
    global oled
    if clear:
        oled.fill(0) # 清屏
    oled.text(text, x, y, color, size = sz)
    oled.show()

# 连接物联网平台
def do_connect_lk(productKey, deviceName, deviceSecret,speaker):
    global device, iot_connected, on_request, on_play, oled
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

    # 打开CHT8305温湿度传感器
    i2cDev = I2C()
    i2cDev.open("cht8305")
    cht8305Dev = CHT8305(i2cDev)

    oledInit()

    # 检测温湿度语音播报并上报云端
    while True:
        temphumidity = play_display_temperature_humidity(cht8305Dev)
        post_data_to_cloud(device, temphumidity)
        time.sleep(10)

    oled.spi.close()
    oled.res.close()
    oled.dc.close()
    i2cDev.close()
    # 断开连接
    device.close()

if __name__ == '__main__':
    print("local speaker demo version")
    speaker = Speaker(resDir)                                       # 初始化speaker
    speaker.play(tonepathPowerOn)                                   # 播放开机启动提示音
    get_wifi_status()                                               # 确保wifi连接成功
    do_connect_lk(productKey, deviceName, deviceSecret,speaker)     # 启动千里传音服务
