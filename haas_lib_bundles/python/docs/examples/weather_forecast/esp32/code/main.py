#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    zhangheng
@version    :    1.0
@Description:    天气预报案例 - 通过gps定位获取该位置近几天的天气状况
                 board.json - 硬件资源配置文件，详情请参考：https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/driver
'''

from aliyunIoT import Device
from driver import UART
import display      # 显示库
import network      # 网络库
import utime        # 延时函数在utime库中
import sntp         # 网络时间同步库
import _thread      # 线程库
import gnss         # gps
import ujson as json


# Wi-Fi SSID和Password设置
SSID='***'
PWD='***'

# HaaS设备三元组
productKey = "***"
deviceName  = "***"
deviceSecret  = "***"

g_lk_connect = False
g_lk_service = False

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

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
            #print(wlan.ifconfig())
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

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True

def weather_forecast(latitude, longitude):
    start = utime.ticks_ms()
    global dev
    ext = {'latitude':latitude, 'longitude': longitude}
    ext_str = json.dumps(ext)
    all_params = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': 'weatherForecast', 'argInt': 1, 'ext': ext_str}}
    all_params_str = json.dumps(all_params)
    forecast_param = {
        'topic': '/sys/' + productKey + '/' + deviceName + '/thing/event/hli_event/post',
        'qos': 1,
        'payload': all_params_str
    }
    dev.publish(forecast_param)
    while g_lk_service == False:
        continue
    time_diff = utime.ticks_diff(utime.ticks_ms(), start)
    print('get response time : %d' % time_diff)

def cb_lk_service(data):
    global g_lk_service, callFlag, weatherResult
    if data != None:
        params = data['params']
        #print(params)
        params_dict = json.loads(params)
        ext = params_dict['ext']
        ext_dict = json.loads(ext)
        result = ext_dict['data']
        resultJson = json.loads(result)
        infocode = resultJson['infocode']
        if infocode == '10000':
            casts = resultJson['forecasts'][0]['casts']
            try:
                print("casts:" + json.dumps(casts))
                weatherResult = casts
                for index in range(len(casts)):
                    cast = casts[index]
                    date = cast['date']
                    daytemp = cast['daytemp']
                    nighttemp = cast['nighttemp']
            except:
                print("Error: casts")
            callFlag = True
        else:
            callFlag = False
    else:
        callFlag = False
    g_lk_service = True

# 循环定位获取天气信息线程
def weather_forecast_thread():
    global gnssDev
    while True:
        location = gnssDev.getLocation()
        if location:
            print("The gnss infor 纬度-%d 经度-%d 海拔-%d",location.longitude, location.latitude, location.altitude)
            latitude = str(location.latitude[0])
            longitude = str(location.longitude[0])
            print('latitude: ' + latitude + ", longitude: " + longitude)
            if(latitude != '0.0' and longitude != '0.0'):
                weather_forecast(latitude, longitude)
        # 间隔5秒后再重新定位
        utime.sleep_ms(5000)

def display_thread():
    # 引用全局变量
    global disp, callFlag, weatherResult
    while True:
        if callFlag == True:
            # 清除屏幕内容
            disp.clear()
            # 设置文字字体
            #disp.font(disp.FONT_DejaVu40)
            # 显示识别结果
            height = 30
            for index in range(len(weatherResult)):
                cast = weatherResult[index]
                date = cast['date']
                daytemp = cast['daytemp']
                nighttemp = cast['nighttemp']
                result = "date=" + date + ",daytemp=" + daytemp + ",nighttemp=" + nighttemp
                print(result)
                disp.text(10, height, result, disp.RED)
                height = height + 30
            utime.sleep_ms(1000)
        else:
            # 清除屏幕内容
            disp.clear()
            # 设置文字字体
            #disp.font(disp.FONT_DejaVu40)
            # 显示识别结果
            disp.text(40, 20, 'no weather!!!', disp.RED)
            print('no weather ...!!!')
            utime.sleep_ms(1000)

def init_gps():
    global gnssDev
    print("gnss init...")
    uartDev = UART()
    uartDev.open("gnss")
    gnssDev = gnss.GNSS(uartDev)

def init_device():
    # 设备初始化
    global dev
    dev = Device()
    dev.on(Device.ON_CONNECT, cb_lk_connect)
    dev.on(Device.ON_SERVICE, cb_lk_service)
    dev.connect(key_info)
    while True:
        if g_lk_connect:
            break

def init():
    # 全局变量
    global disp, frame, gnssDev, callFlag, weatherResult
    callFlag = False
    # 创建lcd display对象
    disp = display.TFT()
    # 连接网络
    connect_wifi(SSID, PWD)
    # 初始化gps
    init_gps()
    # 初始化设备
    init_device()

def execute():
    try:
        # 启动显示线程
        _thread.start_new_thread(display_thread, ())
        # 设置线程stack
        _thread.stack_size(10 * 1024)
        # 启动定位线程
        _thread.start_new_thread(weather_forecast_thread, ())
    except:
       print("Error: unable to start thread")
    while True:
        utime.sleep_ms(1000)

def main():
    init()
    execute()

if __name__ == '__main__':
    main()
