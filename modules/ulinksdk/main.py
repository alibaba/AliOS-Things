#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime   # 延时函数在utime库中
import network                    # Wi-Fi功能所在库
from aliyunIoT_test import Device
import json

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

productKey = "产品密钥"     #需要填入物联网云平台申请到的productKey信息
deviceName = "设备名称"     #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "设备密码"   #需要填入物联网云平台申请到的deviceSecret信息
ProductSecret = "产品密码"  #需要填入物联网云平台申请到的productSecret信息

iot_connected = False
ntp_time_received = True
g_register_flag = False

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
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
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()                    #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

def cb_lk_register(data):
    global g_register_flag
    print("device regiter succeed !")
    g_register_flag = True

def on_connect(data):
    global iot_connected
    iot_connected = True

def ntp_cb(data):
    global ntp_time_received

    print('%s-%s-%s %s:%s:%s:%s'%(data['year'],data['month'],\
                data['day'],data['hour'],\
                data['minute'],data['second'],data['msecond']))
    print('timestamp:', data['timestamp'])
    ntp_time_received = True

def on_props(msg):
    print("property set received!")
    payload= msg['params']
    if 'LightSwitch' in payload:
        print('LightSwitch: ', payload['LightSwitch'])

def start_tests():
    global productKey, deviceName, deviceSecret, iot_connected

    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }

    dev_register_info = {
        'deviceName': deviceName,
        'productKey': productKey,
        'productSecret': ProductSecret,
        'region': 'cn-shanghai',
    }

    td_r = Device()

    '''print('start register test...')
    td_r.register(dev_register_info, cb_lk_register)
    i = 0
    while g_register_flag == False:
        utime.sleep(0.5)
        print("waitng for request success...")
        i = i+1
        if (i > 20):
            print("requst failed...")
            break

    if g_register_flag == True:
        print('register test pass!')
    else:
        print('register test fail!')

    del td_r'''

    td = Device()
    td.on(td.ON_CONNECT, on_connect)
    td.connect(key_info)
    while (iot_connected == False):           # wait for connect success
        print('wait for connect success...')
        utime.sleep(0.5)
        pass

    print('start post property test...')
    utime.sleep(1)
    value = {'test_prop' : 100}
    data = {'params': json.dumps(value)}
    td.on(Device.ON_PROPS, on_props)

    ret = td.postProps(data)
    if ret == 0 :
        print('post property test pass!')
    else :
        print('post property test fail!')
    utime.sleep(2)

    print('start post event test...')
    utime.sleep(1)
    value =  {'test_event' : 100}
    param_str = json.dumps(value)
    data = {'id': 'EventTest' ,'params': param_str}
    ret = td.postEvent(data)
    if ret == 0 :
        print('post event test pass!')
    else :
        print('post event test fail!')
    utime.sleep(2)

    print('start post Raw test...')
    utime.sleep(1)
    usertestdata = bytearray([0x00,0x00,0x00,0x00,0x01,0x00,0x32,0x01,0x00,0x00,0x00,0x00])
    postdata={'param':usertestdata}
    ret = td.postRaw(postdata)
    if ret == 0 :
        print('post Raw test pass!')
    else :
        print('post Raw test fail!')

    print('start getNtpTime test...')
    time = td.getNtpTime(ntp_cb)
    if ntp_time_received == True:
        print('getNtpTime test pass!')
    else :
        print('getNtpTime test fail!')

    utime.sleep(2)

    print('start uploadfile test...')
    with open('test_local.txt', "w+") as f:
        f.write("hello world from vic")

    res = td.uploadFile( 'test_up.txt', 'test_local.txt', None)
    if res != None:
        print('uploadfile test pass!, uploadid:', res)
    else:
        print('uploadfile test fail!, uploadid:', res)

    print('start uploadcontent test...')
    f = open('test_local.txt', "w+")
    f.write("hello world again from vic")
    f.seek(0)
    content = f.read()
    f.close()
    res = td.uploadContent( 'test_up.txt', content, None)
    if res != None:
        print('uploadcontent test pass!, uploadid:', res)
    else:
        print('uploadcontent test fail!, uploadid:', res)
    utime.sleep(2)

    print('start subscribe test...')
    topic_test_info = {
        'topic': "/sys/{}/{}/thing/deviceinfo/update_reply".format(productKey, deviceName),
        'qos': 1
    }
    res = td.subscribe(topic_test_info)
    if res == 0:
        print('subscribe test pass!')
    else :
        print('subscribe test fail!')
    utime.sleep(2)

    print('start unsubscribe test...')
    topic_test_info = {
        'topic': "/sys/{}/{}/thing/deviceinfo/update_reply".format(productKey, deviceName),
        'qos': 1
    }
    res = td.unsubscribe(topic_test_info)
    if res == 0:
        print('unsubscribe test pass!')
    else :
        print('unsubscribe test fail!')
    utime.sleep(2)

    td.end()

if __name__ == '__main__':
    print("hello python linksdk")
    get_wifi_status()
    utime.sleep(1)
    start_tests()