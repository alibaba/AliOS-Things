#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    杭漂
@version    :    1.0
@Description:    通过跟HaaS Cloud控制台中“升级服务-应用升级”配合，实现设备端python应用的升级。
                 HaaS Cloud控制台网址：https://haas.iot.aliyun.com/welcome
'''

import network      # 网络库
import utime        # 延时函数在utime库中
import time         # 获取时间搓
import sntp         # 网络时间同步库
import ujson as json
import kv
import machine
from upgrade import *

# Wi-Fi SSID和Password设置
SSID="Your-AP-SSID"
PWD="Your-AP-Password"
#三元组信息
ProductKey = "Your-ProductKey"
DeviceName  = "Your-DeviceName"
DeviceSecret  = "Your-DeviceSecret"

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': ProductKey ,
    'deviceName': DeviceName ,
    'deviceSecret': DeviceSecret ,
    'keepaliveSec': 60
}

app_info = {
    'appId':'',
    'localAppVersion':'',
    'appNewVersion':'',
    'mac':'10:10:10:10:10:10',
    'ip':'10.10.10.10'
}

def connect_wifi(ssid, pwd):
    global g_wifi_connected
    while True:
        try :
            # 初始化网络
            wlan = network.WLAN(network.STA_IF)
            wlan.active(True)
            wlan.connect(ssid, pwd)
            while True:
                print('Wi-Fi is connecting...')
                # 网络连接成功后，更新显示字符
                if (wlan.isconnected() == True):
                    g_wifi_connected = True
                    print('Wi-Fi is connected')
                    ip = wlan.ifconfig()[0]
                    print('IP: %s' %ip)
                    # NTP时间更新，如果更新不成功，将不能进行识别
                    print('NTP start')
                    sntp.setTime()
                    print('NTP done')
                    return
                utime.sleep_ms(500)
        except(OSError):
            print("OSError except")
        except:
            print("unknow except")
        utime.sleep(3)

def query_upgrade_result_cb(data):
    global engine
    print("upgrade_result_cb needUpgrade")
    #设备升级标志位和所需数据
    kv.set('_amp_app_upgrade','enable')
    kv.set('_amp_wifi_ssid',SSID)
    kv.set('_amp_wifi_passwd',PWD)
    kv.set('_amp_pyapp_url',data['url'])
    #通知云端获取
    app_info['localAppVersion'] = data['localAppVersion']
    app_info['appNewVersion'] = data['version']
    app_info['appId'] = data['appId']
    engine.__pub_upgrade_result_event(app_info,200)
    #立刻重启设备更新应用
    resetTime = 3
    while True:
        print("Reset Device to upgarde app:",resetTime)
        time.sleep(2)
        resetTime = resetTime - 1
        if resetTime == 0:
            print("您也可以使用手动按复位按钮，重启设备。")
            machine.reset()

def get_app_info():
    try:
        file_name = r'/data/pyamp/manifest.json'
        # 以只读方式打开文件
        f = open(file_name)
        # 一次读取整个文件
        content = f.read()
        print(content)
        app_dict = json.loads(content)
        app_info['localAppVersion'] = app_dict['version']
        app_info['appId'] = app_dict['appid']
    finally:
        f.close()

def main() :
    # 全局变量
    global engine,g_wifi_connected
    #清空升级标志位
    kv.set('_amp_app_upgrade','disable')
    g_wifi_connected = False
    # 读取本地配置
    get_app_info()
    # 连接网络
    connect_wifi(SSID, PWD)
    while True:
        if g_wifi_connected == True:
            break
        utime.sleep_ms(50)
    # 链接阿里云物联网平台
    engine = Upgrade(key_info,app_info,query_upgrade_result_cb)
    #发布应用查询请求
    while True:
        utime.sleep_ms(50)

if __name__ == '__main__':
    print("Python App Upgrade Program")
    main()
