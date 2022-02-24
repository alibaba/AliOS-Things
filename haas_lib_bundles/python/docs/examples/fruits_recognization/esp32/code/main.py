#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Description:    水果识别案例
@Author     :    luokui
@version    :    1.0
'''

from aliyunIoT import Device
import display      # 显示库
import network      # 网络库
import ucamera      # 摄像头库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import ujson as json

# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'

# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

g_lk_connect = False
g_lk_service = False
g_score = 0

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

# 因暂时不支持中文，将识别结果从中文转为英文
def translateName(name):
    if name == '橙子' or name == '桔子':
        name = 'Orange'
    elif name == '苹果':
        name = 'Apple'
    elif name == '香蕉':
        name = 'Banana'
    else:
        name = 'Fruits'
    return name

def connect_wifi(ssid, pwd):
    # 引用全局变量
    global disp
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 显示网络连接中
        disp.text(20, 30, 'Wi-Fi is connecting...', disp.RED)
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            disp.textClear(20, 30, 'Wi-Fi is connecting...')
            disp.text(20, 30, 'Wi-Fi is connected', disp.RED)
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            disp.text(20, 50, ip, disp.RED)

            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start')
            disp.text(20, 70, 'NTP start...', disp.RED)
            sntp.setTime()
            print('NTP done')
            disp.textClear(20, 70, 'NTP start...')
            disp.text(20, 70, 'NTP done', disp.RED)
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_service(data):
    global g_lk_service, g_score, detected, name
    # dev.publish(compare_reply)
    # print(data)
    #resp = json.loads(data)
    if data != None:
        params = data['params']
        params_dict = json.loads(params)
        print(params_dict)
        ext = params_dict['ext']
        ext_dict = json.loads(ext)
        result = ext_dict['result']
        if result == 'success':
            i = 0
            fruits_list = ext_dict['fruitList']
            while (i < len(fruits_list)) :
                g_score = fruits_list[i]['score']
                name = fruits_list[i]['name']
                name = translateName(name)
                if g_score > 0.6:
                    print('detect: ' + name)
                    detected = True
                else:
                    detected = False
                i += 1
        else:
            print('do not detect!')
            detected = False
    g_lk_service = True

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True


def detectFruits(frame, fileName, productKey, devName, devSecret) :
    start = utime.ticks_ms()
    global dev
    # 上传图片到LP
    fileid = dev.uploadContent(fileName, frame, None)
    if fileid != None:
        ext = { 'filePosition':'lp', 'fileName': fileName, 'fileId': fileid }
        ext_str = json.dumps(ext)
        all_params = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': 'detectFruits', 'argInt': 1, 'ext': ext_str }}
        all_params_str = json.dumps(all_params)
        upload_file = {
            'topic': '/sys/' + productKey + '/' + deviceName + '/thing/event/hli_event/post',
            'qos': 1,
            'payload': all_params_str
        }

        # 上传完成通知HaaS聚合平台
        # print(upload_file)
        dev.publish(upload_file)

        while g_lk_service == False:
            continue
    else:
        print('filedid is none, upload content fail')
    time_diff = utime.ticks_diff(utime.ticks_ms(), start)
    print('get response time : %d' % time_diff)

# 单线程案例
def main():
    global disp, dev, g_score
    dev = Device()
    disp = display.TFT()

    connect_wifi(SSID, PWD)
    dev.on(Device.ON_CONNECT, cb_lk_connect)
    dev.on(Device.ON_SERVICE, cb_lk_service)
    dev.connect(key_info)

    while True:
        if g_lk_connect:
            break

    ucamera.init('uart', 33, 32)
    ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

    while True:
        g_lk_service = False
        start = utime.ticks_ms()
        frame = ucamera.capture()
        time_diff = utime.ticks_diff(utime.ticks_ms(), start)
        print('capture time : %d' % time_diff)

        start = utime.ticks_ms()
        disp.image(0, 0, frame, 0)
        time_diff = utime.ticks_diff(utime.ticks_ms(), start)
        print('image time : %d' % time_diff)

        detectFruits(frame, 'fruits.jpg', productKey, deviceName, deviceSecret)

        if (g_score > 0.6):
            disp.clear()
            disp.font(disp.FONT_DejaVu40)
            # 显示识别结果
            disp.text(40, 80, name, disp.RED)
            disp.text(40, 120, 'Deteted!!!', disp.RED)
            print('Fruits Detected!!!')
            utime.sleep_ms(1000)
            g_score = 0

if __name__ == '__main__':
    main()
