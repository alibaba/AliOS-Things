#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    Alibaba
@version    :    1.0
@Description:    通过连接物联网云平台，在收到股票涨停信号时，控制设备LCD显示股票代码，并控制蜂鸣器发出警报。
                 board.json - 硬件资源配置文件，详情请参考：https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/driver
'''

from aliyunIoT import Device
from driver import GPIO
from driver import PWM
import utime   # 延时函数在utime库中
import network      # 网络库
import ujson as json
import lcd1602


# Wi-Fi SSID和Password设置
SSID='请填写您的路由器名称'
PWD='请填写您的路由器密码'

# HaaS设备三元组
productKey = "产品密钥"
deviceName  = "设备名称"
deviceSecret  = "设备密钥"

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
            break
        utime.sleep_ms(500)
    utime.sleep(2)

def cb_lk_connect(data):
    global g_lk_connect
    print('link platform connected')
    g_lk_connect = True


# 接收云端下发的属性设置
# request格式: {'code': 0, 'params_len': 18, 'msg_id': 710513123, 'params': '{"stock_status":1}'}
def on_props(request):
    global stock_status, stock_code, stock_name
    print(request)
    props = json.loads(request['params'])
    stock_code = props.get('stock_code', '')
    stock_name = props.get('stock_name', '')
    stock_status = props.get('stock_status', 0)
    print("set value : %s %s %s" % (stock_status, stock_code, stock_name))

def loopThingModel():
    global stock_status, stock_code, stock_name
    # 判断是否有信号产生
    prev_stock_status = stock_status
    while True:
        if prev_stock_status != stock_status:
            prev_stock_status = stock_status
            if stock_status == 1:
                showStock(stock_code, stock_name)
                startAlarm(5)

def showStock(stock_code, stock_name):
    global lcdDrv
    print(stock_code, stock_name)
    lcdDrv.displayString(0, 0, 'Ur Stock Alarm:')
    lcdDrv.displayString(1, 0, str(stock_code))

def startAlarm(seconds):
    print('start alarm')
    global buzzerDrv
    api_beep(buzzerDrv)
    utime.sleep(seconds)
    api_beep(buzzerDrv, 0, 0)

buzzerDrv = PWM()
lcdDrv = None

def api_beep(buzzerDrv, freq0=440, duty=50):
    print('api_beep %s' % freq0)
    ret = buzzerDrv.setOption({'freq': freq0, 'duty': duty })

def main():
    global buzzerDrv, lcdDrv
    buzzerDrv.open("buzzer")
    api_beep(buzzerDrv, 0, 0)

    RS = GPIO(); RS.open('RS')
    RW = GPIO(); RW.open('RW')
    E = GPIO(); E.open('E')
    D4 = GPIO(); D4.open('D4')
    D5 = GPIO(); D5.open('D5')
    D6 = GPIO(); D6.open('D6')
    D7 = GPIO(); D7.open('D7')
    print('GPIO init done')

    lcdDrv = lcd1602.LCD1602(RS, RW, E, D4, D5, D6, D7)

    global stock_name, stock_code, stock_status
     # 连接网络
    connect_wifi(SSID, PWD)

    stock_status = 0
    stock_name = ''
    stock_code = ''

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

    loopThingModel()

if __name__ == '__main__':
    main()
