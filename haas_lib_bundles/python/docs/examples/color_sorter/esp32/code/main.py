#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
from aliyunIoT import Device
from driver import I2C
import utime   # 延时函数在utime库中
from tcs34725 import *
from driver import PWM
import servo
import utime   # 延时函数在utime库中
import sntp         # 网络时间同步库
import network      # 网络库
import _thread      # 线程库
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
red_cnt = 0
orange_cnt = 0
yellow_cnt = 0
blue_cnt = 0
green_cnt = 0
other_cnt = 0

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

def connect_wifi(ssid, pwd):
    # 初始化网络
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, pwd)
    while True:
        print('Wi-Fi is connecting...')
        # 网络连接成功后，更新显示字符
        if (wlan.isconnected() == True):
            print('Wi-Fi is connected')
            ip = wlan.ifconfig()[0]
            print('IP: %s' %ip)
            # NTP时间更新，如果更新不成功，将不能进行识别
            print('NTP start...')
            sntp.setTime()
            print('NTP done')
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True

# 上报统计人数到云端
def postDeviceProps():
    global dev, red_cnt, orange_cnt, yellow_cnt, blue_cnt, green_cnt, other_cnt, Sorter_switch
    value = {'Sorter_switch' : Sorter_switch,
            'Red_cnt': red_cnt,
            'Orange_cnt': orange_cnt,
            'Yellow_cnt': yellow_cnt,
            'Blue_cnt': blue_cnt,
            'Green_cnt':green_cnt,
            'Other_cnt': other_cnt}
    data = {'params': json.dumps(value)}
    ret = dev.postProps(data)
    print('upload props-->' + str(data))
    if ret == 0 :
        print('颜色识别结果上报成功')
    else :
        print('颜色识别结果上报失败')

# 接收云端下发的属性设置
def on_props(request):
    global Sorter_switch
    try:
        props = eval(request['params'])
        Sorter_switch = props['Sorter_switch']
        print("sorter_switch convert to : " + str(Sorter_switch))
    except Exception as e:
        print(e)

if __name__ == '__main__':
    global Sorter_switch, dev
    Sorter_switch = 0
    # 连接网络
    connect_wifi(SSID, PWD)

    # 设备初始化
    dev = Device()
    dev.on(Device.ON_CONNECT, cb_lk_connect)
    # 配置收到云端属性控制指令的回调函数
    # 如果收到物联网平台发送的属性控制消息，则调用on_props函数
    dev.on(Device.ON_PROPS, on_props)
    dev.connect(key_info)
    while True:
        if g_lk_connect:
            break

    i2cObj = I2C()                    # 创建I2C设备对象
    i2cObj.open('colorSensor')        # 打开名为colorSensor的I2C设备节点

    colorSensor = TCS34725(i2cObj)    # 创建颜色传感器对象
    print('colorSensor init done')
    # 初始化上层舵机
    print("init top servo...")
    top_pwmObj = PWM()
    top_pwmObj.open("top_servo")
    top_servoObj = servo.SERVO(top_pwmObj)
    top_servoObj.setOptionSero(0)
    utime.sleep(1)
    print("top_servo inited")

    # 初始化下层舵机
    print("init bottom servo...")
    bottom_pwmObj = PWM()
    bottom_pwmObj.open("bottom_servo")
    bottom_servoObj = servo.SERVO(bottom_pwmObj)
    bottom_servoObj.setOptionSero(0)
    utime.sleep(1)
    print("bottom_servo inited")

    while True:
        if Sorter_switch:
            top_servoObj.setOptionSero(-63)
            utime.sleep(2)
            top_servoObj.setOptionSero(-10)
            utime.sleep(2)
            r, g, b = colorSensor.getRGB()                # 读取RGB测量结果
            print('r:%d, g:%d, b:%d' % (r, g, b))

            if r > 240 and r < 255 and g > 90 and g < 130 and b > 75 and b < 110:#red
                bottom_servoObj.setOptionSero(-50)
                red_cnt += 1
                print('detect red, cnt:', str(red_cnt))
            elif r > 240 and r < 255 and g > 205 and g < 245 and b > 105 and b < 145:#orange
                bottom_servoObj.setOptionSero(-30)
                orange_cnt += 1
                print('detect orange, cnt:', str(orange_cnt))
            elif r > 240 and r < 255 and g > 240 and g < 254 and b > 240 and b < 255:#yellow
                bottom_servoObj.setOptionSero(-10)
                yellow_cnt += 1
                print('detect yellow, cnt:', str(yellow_cnt))
            elif r > 240 and r < 255 and g > 250 and g < 255 and b > 180 and b < 220:#green
                bottom_servoObj.setOptionSero(10)
                green_cnt += 1
                print('detect green, cnt:', str(green_cnt))
            elif r > 125 and r < 170 and g > 230 and g < 255 and b > 220 and b < 255:#blue
                bottom_servoObj.setOptionSero(30)
                blue_cnt += 1
                print('detect blue, cnt:', str(blue_cnt))
            else:
                bottom_servoObj.setOptionSero(50)
                other_cnt += 1
                print('detect others, cnt:', str(other_cnt))
            utime.sleep(1)
            top_servoObj.setOptionSero(30)
            utime.sleep(2)
            postDeviceProps()
        else:
            utime.sleep(2)
